package pt.tecnico.distledger.server.grpc;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;

import pt.ulisboa.tecnico.distledger.contract.distledgerserver.DistLedgerCrossServerServiceGrpc;
import pt.ulisboa.tecnico.distledger.contract.distledgerserver.CrossServerDistLedger.*;

import pt.ulisboa.tecnico.distledger.contract.namingserver.NamingServerServiceGrpc;
import pt.ulisboa.tecnico.distledger.contract.namingserver.NamingServerDistledger.*;

import pt.ulisboa.tecnico.distledger.contract.DistLedgerCommonDefinitions.LedgerState;

import java.util.List;

import java.lang.RuntimeException;

public class CrossServerService implements AutoCloseable {

    private static final String DEFAULT_SERVICE = "DistLedger";
    private static final int NAMING_SERVER_PORT = 5001;

    /** 
     * Set flag to true to print debug messages. 
	 * The flag can be set using the -Ddebug command line option.
     */
	private static final boolean DEBUG_FLAG = (System.getProperty("debug") != null);

    private final String namingServerAddress;

    // Primary server information.
    private final String address;

    private ManagedChannel channel;
    private DistLedgerCrossServerServiceGrpc.DistLedgerCrossServerServiceBlockingStub stub;

    // Channel and stub used to connect to the naming server.
    private ManagedChannel namingServerChannel;
    private NamingServerServiceGrpc.NamingServerServiceBlockingStub namingServerStub;


    public CrossServerService(String host, int port) {
        this.address = host + ":" + port;

        this.namingServerAddress = host + ":" + NAMING_SERVER_PORT;

        // Initially connects to the naming server.
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

    public String getAddress() {
        return this.address;
    }

    public String getNamingServerAddress() {
        return this.namingServerAddress;
    }

    /**
     * Creates the channel and stub that connect the user
     * to the naming server.
     */
    public void connectNamingServer() {
        final String target = getNamingServerAddress();

        debug("Establishing connection with naming server target: " + target + ".");

        namingServerChannel = ManagedChannelBuilder.forTarget(target).usePlaintext().build();
        namingServerStub = NamingServerServiceGrpc.newBlockingStub(namingServerChannel);
    }

    /**
     * Creates a channel and stub that connect the service
     * to the given address (host:port of a secondary server).
     */
    public void connect(String host, int port) {
        final String target = host + ":" + port;

        debug("Establishing connection with server target: " + target + ".");

        channel = ManagedChannelBuilder.forTarget(target).usePlaintext().build();
        stub = DistLedgerCrossServerServiceGrpc.newBlockingStub(channel);
    }

    /**
     * Checks if all the given servers are active.
     * If at least one isn't, we can't propagate the state to any of them.
     */
    private void checkServerStatus(List<String> serverAddressList) throws RuntimeException {
        for (String serverAddress : serverAddressList) {
            debug("Checking server status: " + serverAddress + ".");

            // Skips the primary server.
            if (serverAddress.equals(getAddress())) {
                continue;
            }

            // Splits the server address into host and port.
            String[] split = serverAddress.split(":");
            String host = split[0];
            int port = Integer.parseInt(split[1]);

            // Connects the service to the new server.
            connect(host, port);
            
            boolean isActive = stub.checkServerStatus(CheckServerStatusRequest.newBuilder().build()).getIsActive();
                
            closeCurrServerConnection();

            if (!isActive) {
                throw new RuntimeException("Server " + serverAddress + " is not active.");
            }
        }
    }

    /**
     * Propagates the given operation to all the secondary servers.
     */
    public void propagateState(List<String> serverAddressList, LedgerState ledger, List<Integer> replicaTS, int replicaId) throws RuntimeException {

        try {
            for (String serverAddress : serverAddressList) {
                debug("Propagating state to server: " + serverAddress + ".");

                // Skips the current server.
                if (serverAddress.equals(getAddress())) {
                    continue;
                }

                // Splits the server address into host and port.
                String[] split = serverAddress.split(":");
                String host = split[0];
                int port = Integer.parseInt(split[1]);

                // Connects the service to the new server.
                connect(host, port);
                
                stub.propagateState(PropagateStateRequest.newBuilder()
                                .setLedger(ledger)
                                .addAllReplicaTS(replicaTS)
                                .setReplicaId(replicaId)
                                .build());

                closeCurrServerConnection();
            }
        }
        catch (RuntimeException e) {
            throw new RuntimeException("stub.propagateState() failed.");
        }
    }

    /**
     * Finds all the remaining servers in the naming server, and propagates
     * the given operation to them.
     */
    public void lookupAndPropagate(LedgerState ledger, List<Integer> replicaTS, int replicaId) throws RuntimeException {
        /*
         * In this service, we always call the lookup operation, instead of keeping connection maps
         * like we do in the admin and user services, because we never know when a new server
         * will be added to the system. 
         * 
         *  - (This is assuming an implementation where we can have X [X > 2] replicas,
         *     if that were to happen, our code would still work).
         */
        debug("Looking for an available server to connect to.");

        LookupResponse response = namingServerStub.lookup(LookupRequest.newBuilder()
                                                                .setServerServiceName(DEFAULT_SERVICE)
                                                                .setServerQualifier("")
                                                                .build());

        List<String> serverAddressList = response.getServerAddressList();

        // The lookup response will never contain an empty list, as the curret server
        // will always be returned.

        if (serverAddressList.size() == 1) {
            // If the list is of size 1, we know that the only server in it is the
            // current server, so we can't propagate the state to any other server.
            throw new RuntimeException("No other servers available.");
        }

        // TODO: do we need this check?
        // Checks if all the servers are active (adds extra safety guarantees).
        checkServerStatus(serverAddressList);

        propagateState(serverAddressList, ledger, replicaTS, replicaId);
    }

    /**
     * Returns true if the service is connected to a server from the naming server,
     * false otherwise.
     */
    private boolean isConnectedToServer() {
        return stub != null;
    }

    private boolean isConnectedToNamingServer() {
        return namingServerStub != null;
    }

    /**
     * Closes the service's channel and stub to the current server.
     */
    private void closeCurrServerConnection() {
        debug("Closing connection with the current user server.");

        channel.shutdown();

        channel = null;
        stub = null;
    }

    /**
     * Closes the service's channel and stub to the naming server.
     */
    private void closeNamingServerConnection() {
        debug("Closing connection with the naming server.");

        namingServerChannel.shutdown();

        namingServerChannel = null;
        namingServerStub = null;
    }
  
    @Override
    public void close() {
        debug("Closing down all the service's connections.");

        if (isConnectedToServer()) {
            closeCurrServerConnection();
        }

        if (isConnectedToNamingServer()) {
            closeNamingServerConnection();
        }
    }

}
