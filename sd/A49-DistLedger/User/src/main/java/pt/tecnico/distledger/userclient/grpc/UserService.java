package pt.tecnico.distledger.userclient.grpc;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import pt.ulisboa.tecnico.distledger.contract.user.UserServiceGrpc;
import pt.ulisboa.tecnico.distledger.contract.user.UserDistLedger.*;

import pt.ulisboa.tecnico.distledger.contract.namingserver.NamingServerServiceGrpc;
import pt.ulisboa.tecnico.distledger.contract.namingserver.NamingServerDistledger.*;

import static pt.tecnico.distledger.common.Common.MAX_REPLICAS;

import java.util.List;
import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;

public class UserService implements AutoCloseable {

    private static final String SUCCESS = "OK";

    private static final String DEFAULT_SERVER_SERVICE = "DistLedger";

    /** 
     * Set flag to true to print debug messages. 
	 * The flag can be set using the -Ddebug command line option.
     */
	private static final boolean DEBUG_FLAG = (System.getProperty("debug") != null);

    // Address used to connect to the naming server.
    private final String namingServerAddress;

    // Channel and stub used to connect to the naming server.
    private ManagedChannel namingServerChannel;
    private NamingServerServiceGrpc.NamingServerServiceBlockingStub namingServerStub;

    // Maps used to store the information of the servers that the user has connected to,
    // to avoid calling lookup unnecessarily.
    private Map<String, ManagedChannel> serverChannels;
    private Map<String, UserServiceGrpc.UserServiceBlockingStub> serverStubs;

    // Used to store the previous version accessed by the user (user's timestamp).
    private List<Integer> prevTS;
    // Used to store the most recently received replica's timestamp.
    private List<Integer> newTS;


    public UserService(String host, int port) {
        this.namingServerAddress = host + ":" + port;

        this.serverChannels = new HashMap<>();
        this.serverStubs = new HashMap<>();

        // Initialize the user's timestamps.
        initializeTS(MAX_REPLICAS);
        
        // Initiallly connect to the naming server.
        connectNamingServer();
    }

    /**
     * Prints a debug message if the debug flag is set to true.
     */
    public void debug(String message) {
        if (DEBUG_FLAG) {
            System.err.println("DEBUG: " + message);
        }
    }

    /**
     * Initializes the user's timestamps, setting all its 
     * entries to 0.
     */
    public void initializeTS(int size) {
        this.prevTS = new ArrayList<>(size);
        this.newTS = new ArrayList<>(size);

        for (int i = 0; i < size; i++) {
            prevTS.add(0);
        }
    }

    public String getNamingServerAddress() {
        return this.namingServerAddress;
    }

    /**
     * Returns the server channel with the given server qualifier.
     */
    public ManagedChannel getChannel(String serverQualifier) {
        return serverChannels.get(serverQualifier);
    }

    /**
     * Returns the server stub with the given server qualifier.
     */
    public UserServiceGrpc.UserServiceBlockingStub getStub(String serverQualifier) {
        return serverStubs.get(serverQualifier);
    }

    /**
     * Adds the new server connection found in the lookup operation
     * to the user's connection maps.
     */
    public void addServerConnection(String serverQualifier, String host, int port) {
        String target = host + ":" + port;

        debug("Adding new server connection with target: " + target + ".");

        ManagedChannel newServerChannel = ManagedChannelBuilder.forTarget(target).usePlaintext().build();
        UserServiceGrpc.UserServiceBlockingStub newServerStub = UserServiceGrpc.newBlockingStub(newServerChannel);

        this.serverChannels.put(serverQualifier, newServerChannel);
        this.serverStubs.put(serverQualifier, newServerStub);
    }

    /**
     * Creates the channel and stub that connect the user
     * to the naming server.
     *  - The user reconnects to the naming server with every operation.
     */
    public void connectNamingServer() {
        final String target = namingServerAddress;

        debug("Establishing connection with naming server target: " + target + ".");

        namingServerChannel = ManagedChannelBuilder.forTarget(target).usePlaintext().build();
        namingServerStub = NamingServerServiceGrpc.newBlockingStub(namingServerChannel);
    }

    /**
     * If the user isn't connected to the server with the given qualifier,
     * it calls the lookup operation to find the server address, and connects to it.
     *  - (essentially adding that address to the connection maps).
     */ 
    public void lookupAndConnect(String serverQualifier) throws RuntimeException {
        
        if (!serverStubs.containsKey(serverQualifier)) {
            debug("Looking for an available server to connect to.");

            // If the user has never connected to a server with the given qualifier,
            // it asks the naming server for a server address, and stores it in the
            // connection maps.

            LookupResponse response = namingServerStub.lookup(LookupRequest.newBuilder()
                                                            .setServerServiceName(DEFAULT_SERVER_SERVICE)
                                                            .setServerQualifier(serverQualifier)
                                                            .build());                        

            List<String> serverAddressList = response.getServerAddressList();

            if (serverAddressList.size() == 0) {
                throw new RuntimeException("No server found for the specified arguments.");
            }
            
            // Always picks the first server returned by the lookup operation.
            String serverAddress = serverAddressList.get(0);

            // Splits the server address into host and port.
            String[] split = serverAddress.split(":");
            String host = split[0];
            int port = Integer.parseInt(split[1]);

            // Adds the new server connection to the user's connection maps.
            addServerConnection(serverQualifier, host, port);
        }        
        else {
            debug("The requested server is already in the user's connections.");
        }
            
    }

    /**
     * Creates an account for the given user.
     */
    public void createAccount(String server, String username) {
        debug("Creating account for user: " + username + ", on server: " + server + ".");
        debug("Current prevTS: " + prevTS);

        try {
            lookupAndConnect(server);

            UserServiceGrpc.UserServiceBlockingStub stub = getStub(server);

            CreateAccountResponse response = stub.createAccount(CreateAccountRequest.newBuilder().setUserId(username).addAllPrevTS(prevTS).build());
            newTS = response.getTSList();

            // Updates the user's timestamp vector.
            merge(prevTS, newTS);
            debug("New prevTS: " + prevTS);

            // If no error occured, the account was created successfully.
            System.out.println(SUCCESS);
            System.out.println();

        } catch (Exception e) {
            // If an error occured, the user is disconnected from the server
            // with the given qualifier.
            closeServerConnection(server);

            System.out.println(e.getMessage());
        }
    }

    /**
     * Returns the current balance of the user's active account.
     */
    public void balance(String server, String username) {
        debug("Checking " + username + "'s balance on server: " + server + ".");
        debug("Current prevTS: " + prevTS);

        try {
            lookupAndConnect(server);

            UserServiceGrpc.UserServiceBlockingStub stub = getStub(server);

            BalanceResponse response = stub.balance(BalanceRequest.newBuilder().setUserId(username).addAllPrevTS(prevTS).build());
            int balance = response.getValue();
            newTS = response.getValueTSList();
            
            // Updates the user's timestamp vector.
            merge(prevTS, newTS);
            debug("New prevTS: " + prevTS);

            // If no error occured, the balance was returned successfully.
            System.out.println(SUCCESS);
            System.out.println(balance);
            System.out.println();

        } catch (Exception e) {
            // If an error occured, the user is disconnected from the server
            // with the given qualifier.
            closeServerConnection(server);

            System.out.println(e.getMessage());
        }
    }

    /**
     * Transfers an amount from one user to the other.
     */
    public void transferTo(String server, String from, String dest, int amount) {
        debug("Transferring " + amount + " from: " + from + " to: " + dest + " on server: " + server + ".");
        debug("prevTS: " + prevTS.toString());

        try {
            lookupAndConnect(server);
            
            UserServiceGrpc.UserServiceBlockingStub stub = getStub(server);

            TransferToResponse response = stub.transferTo(TransferToRequest.newBuilder().setAccountFrom(from).setAccountTo(dest).setAmount(amount).addAllPrevTS(prevTS).build());
            newTS = response.getTSList();

            // Updates the user's timestamp vector.
            merge(prevTS, newTS);
            debug("New prevTS: " + prevTS);
            
            // If no error occured, the transfer was successful.
            System.out.println(SUCCESS);
            System.out.println();

        } catch (Exception e) {
            // If an error occured, the user is disconnected from the server
            // with the given qualifier.
            closeServerConnection(server);

            System.out.println(e.getMessage());
        }
    }

    /**
     * Returns true if the user is connected to the naming server, false otherwise.
     */
    private boolean isConnectedToNamingServer() {
        return namingServerStub != null;
    }

    /**
     * Closes the user's connection to the given server. 
     */
    private void closeServerConnection(String serverQualifier) {
        debug("Closing connection with server: " + serverQualifier + ".");

        ManagedChannel channel = getChannel(serverQualifier);

        if (channel != null) {
            channel.shutdown();

            // Removes the server connection from the user's connection maps.
            serverChannels.remove(serverQualifier);
            serverStubs.remove(serverQualifier);
        }
    }

    /**
     * Closes all the user's server connections.
     */
    private void closeCurrServerConnections() {
        debug("Closing connections with the current user servers.");

        for (String serverQualifier : serverStubs.keySet()) {
            closeServerConnection(serverQualifier);
        }
    }

    /**
     * Closes the user's channel and stub to the naming server.
     */
    private void closeNamingServerConnection() {
        debug("Closing connection with the naming server.");

        namingServerChannel.shutdown();

        namingServerChannel = null;
        namingServerStub = null;
    }

    /**
     * Closes the user's channel and stub to the naming server.
     */
    @Override
    public void close() {
        debug("Closing connection with the naming server.");

        closeCurrServerConnections();

        if (isConnectedToNamingServer()) {
            closeNamingServerConnection();
        }
    }

    /**
     * Merges the given TS_1 and TS_2, and stores the result in TS_1.
     */
    private synchronized void merge(List<Integer> TS_1, List<Integer> TS_2) {
        debug("Merging TS_1: " + TS_1 + " and TS_2: " + TS_2 + ".");
        
        for (int i = 0; i < MAX_REPLICAS; i++) {
            TS_1.set(i, Math.max(TS_1.get(i), TS_2.get(i)));
        }
    }
    
}
