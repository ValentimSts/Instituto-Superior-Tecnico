package pt.tecnico.distledger.adminclient.grpc;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import pt.ulisboa.tecnico.distledger.contract.DistLedgerCommonDefinitions.LedgerState;
import pt.ulisboa.tecnico.distledger.contract.admin.AdminServiceGrpc;
import pt.ulisboa.tecnico.distledger.contract.admin.AdminDistLedger.*;

import pt.ulisboa.tecnico.distledger.contract.namingserver.NamingServerServiceGrpc;
import pt.ulisboa.tecnico.distledger.contract.namingserver.NamingServerDistledger.*;

import java.util.List;
import java.util.Map;
import java.util.HashMap;

public class AdminService implements AutoCloseable {

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

    // Maps used to store the information of the servers that the admin has connected to,
    // to avoid calling lookup unnecessarily.
    private Map<String, ManagedChannel> serverChannels;
    private Map<String, AdminServiceGrpc.AdminServiceBlockingStub> serverStubs;


    public AdminService(String host, int port) {
        this.namingServerAddress = host + ":" + port;

        this.serverChannels = new HashMap<>();
        this.serverStubs = new HashMap<>();
        
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
    public AdminServiceGrpc.AdminServiceBlockingStub getStub(String serverQualifier) {
        return serverStubs.get(serverQualifier);
    }

    /**
     * Adds the new server connection found in the lookup operation
     * to the admin's connection maps.
     */
    public void addServerConnection(String serverQualifier, String host, int port) {
        String target = host + ":" + port;

        debug("Adding new server connection with target: " + target + ".");

        ManagedChannel newServerChannel = ManagedChannelBuilder.forTarget(target).usePlaintext().build();
        AdminServiceGrpc.AdminServiceBlockingStub newServerStub = AdminServiceGrpc.newBlockingStub(newServerChannel);

        this.serverChannels.put(serverQualifier, newServerChannel);
        this.serverStubs.put(serverQualifier, newServerStub);
    }

    /**
     * Creates the channel and stub that connect the admin
     * to the naming server.
     */
    public void connectNamingServer() {
        final String target = namingServerAddress;

        debug("Establishing connection with naming server target: " + target + ".");

        namingServerChannel = ManagedChannelBuilder.forTarget(target).usePlaintext().build();
        namingServerStub = NamingServerServiceGrpc.newBlockingStub(namingServerChannel);
    }

    /**
     * Finds the address of all the servers with the given qualifier.
     */
    private List<String> lookup(String serverQualifier) throws RuntimeException {
        debug("Looking up server with qualifier: " + serverQualifier + ".");

        LookupResponse response = namingServerStub.lookup(LookupRequest.newBuilder()
                                                            .setServerServiceName(DEFAULT_SERVER_SERVICE)
                                                            .setServerQualifier(serverQualifier)
                                                            .build());

        return response.getServerAddressList();
    }

    /**
     * If the admin isn't connected to the server with the given qualifier,
     * it calls the lookup operation to find the server address, and connects to it.
     *  - (essentially adding that address to the connection maps).
     */
    public void lookupAndConnect(String serverQualifier) throws RuntimeException {
        debug("Looking for an available server to connect to.");

        if (!serverStubs.containsKey(serverQualifier)) {
            List<String> serverAddressList = lookup(serverQualifier);

            if (serverAddressList.size() == 0) {
                throw new RuntimeException("No server found for the specified arguments.");
            }
            
            // Always picks the first server returned by the lookup operation.
            String serverAddress = serverAddressList.get(0);

            // Splits the server address into host and port.
            String[] split = serverAddress.split(":");
            String host = split[0];
            int port = Integer.parseInt(split[1]);

            // Adds the new server connection to the admin's connection maps.
            addServerConnection(serverQualifier, host, port);
        }
        else {
            debug("The requested server is already in the admin's connections.");
        }
    }

    /**
     * Puts the server in 'ACTIVE' mode.
     */
    public void activate(String server) {
        debug("Activating server: " + server + ".");

        try {
            lookupAndConnect(server);

            AdminServiceGrpc.AdminServiceBlockingStub stub = getStub(server);

            ActivateResponse response = stub.activate(ActivateRequest.newBuilder().build());
            String responseString = response.toString();

            // If no error occured, the server was put on 'ACTIVE' mode successfully.
            System.out.println(SUCCESS);
            System.out.println(responseString);

        } catch (Exception e) {
            // If an error occured, the admin is disconnected from the server
            // with the given qualifier.
            closeServerConnection(server);

            System.out.println(e.getMessage());
        }
    }

    /**
     * Puts the server in 'INNACTIVE' mode.
     * In this mode, the server will will respond with the error message'UNNAVAILABLE'.
     * to any admin request.
     */
    public void deactivate(String server) {
        debug("Deactivating server: " + server + ".");

        try {
            lookupAndConnect(server);

            AdminServiceGrpc.AdminServiceBlockingStub stub = getStub(server);

            DeactivateResponse response = stub.deactivate(DeactivateRequest.newBuilder().build());
            String responseString = response.toString();

            // If no error occured, the server was put on 'INNACTIVE' mode successfully.
            System.out.println(SUCCESS);
            System.out.println(responseString);

        } catch (Exception e) {
            // If an error occured, the admin is disconnected from the server
            // with the given qualifier.
            closeServerConnection(server);

            System.out.println(e.getMessage());
        }
    }

    /**
     * Gets the current state of the ledger.
     */
    public void getLedgerState(String server) {
        debug("Getting ledger state of server: " + server + ".");

        try {
            lookupAndConnect(server);

            AdminServiceGrpc.AdminServiceBlockingStub stub = getStub(server);

            getLedgerStateResponse response = stub.getLedgerState(getLedgerStateRequest.newBuilder().build());
            String responseString = response.toString();

            // If no error occured, the leger state was returned successfully.
            System.out.println(SUCCESS);
            System.out.println(responseString);

        } catch (Exception e) {
            // If an error occured, the admin is disconnected from the server
            // with the given qualifier.
            closeServerConnection(server);

            System.out.println(e.getMessage());
        }
    }

    /**
     * Forces a replica to make a deferred propagation to
     * the remaining replicas.
     */
    public void gossip(String server) {
        debug(server + " is gossiping.");

        try {
            lookupAndConnect(server);

            AdminServiceGrpc.AdminServiceBlockingStub stub = getStub(server);

            GossipResponse response = stub.gossip(GossipRequest.newBuilder().build());
            String responseString = response.toString();

            // If no error occured, the leger state was returned successfully.
            System.out.println(SUCCESS);
            System.out.println(responseString);

        } catch (Exception e) {
            // If an error occured, the admin is disconnected from the server
            // with the given qualifier.
            closeServerConnection(server);

            System.out.println(e.getMessage());
        }
    }

    /**
     * Forces a replica to make a deferred propagation to
     * the remaining replicas based on the server's tableTS.
     */
    public void gossipTableTS(String server) {
        debug(server + " is gossiping.");

        try {
            // By passing an empty string as the server qualifier, the lookup operation
            // will return the address of all the servers.
            List<String> serverAddressList = lookup("");

            if (serverAddressList.size() == 0) {
                throw new RuntimeException("No server found for the specified arguments.");
            }

            String currServerAddress = getServerAddress(server);
            // Skips the server that is gossiping.
            // serverAddressList.remove(currServerAddress);

            for (String serverAddress : serverAddressList) {

                if (serverAddress.equals(currServerAddress)) {
                    // Skips the server that is gossiping.
                    continue;
                }

                debug("Gossiping to: " + serverAddress + ".");

                // Gets the ledger state to propagate to the replica.
                GetInfoToPropagateResponse replicaInfo = getLedgerToPropagate(server, getServerId(serverAddress));
                LedgerState ledgerState = replicaInfo.getLedgerState();
                List<Integer> replicaTS = replicaInfo.getReplicaTSList();

                // Splits the server address into host and port.
                String[] split = serverAddress.split(":");
                String host = split[0];
                int port = Integer.parseInt(split[1]);

                String target = host + ":" + port;

                // Creates a temporary stub and channel for each of the found servers.
                ManagedChannel newServerChannel = ManagedChannelBuilder.forTarget(target).usePlaintext().build();
                AdminServiceGrpc.AdminServiceBlockingStub newServerStub = AdminServiceGrpc.newBlockingStub(newServerChannel);

                newServerStub.gossip(GossipRequest.newBuilder()
                                        .setReplicaId(getServerId(currServerAddress))
                                        .setLedgerState(ledgerState)
                                        .addAllReplicaTS(replicaTS)
                                        .build());

                // Closes the temporary channel and stub.
                newServerChannel.shutdown();
            }

        } catch (Exception e) {
            // If an error occured, the admin is disconnected from the server
            // with the given qualifier.
            closeServerConnection(server);

            System.out.println(e.getMessage());
        }

        System.out.println(SUCCESS);
        System.out.println();
    }

    /**
     * Gets the necessary info to propagate the right ledger from the current server to the given
     * replica (replicaTS [from the server propagating the state] + ledgerState [to propagate to the
     * given replic, based on the table timestamp values]).
     */
    private GetInfoToPropagateResponse getLedgerToPropagate(String serverQualifier, int replicaId) {
        debug("Getting ledger to propagate from server " + serverQualifier + " to the replica with ID: " + replicaId + ".");

        lookupAndConnect(serverQualifier);

        GetInfoToPropagateRequest request = GetInfoToPropagateRequest.newBuilder()
                                                    .setReplicaId(replicaId)
                                                    .build();

        AdminServiceGrpc.AdminServiceBlockingStub stub = getStub(serverQualifier);

        GetInfoToPropagateResponse response = stub.getLedgerToPropagate(request);

        return response;
    }

    /**
     * Returns the server id of the server with the given address.
     */
    private int getServerId(String serverAddress) {
        debug("Getting server id for server: " + serverAddress + ".");

        GetServerIDRequest request = GetServerIDRequest.newBuilder()
                                        .setServerServiceName(DEFAULT_SERVER_SERVICE)
                                        .setServerAddress(serverAddress)
                                        .build();

        GetServerIDResponse response = namingServerStub.getServerID(request);
        int serverId = response.getServerID();

        return serverId;
    }

    /**
     * Returns the server address of the server with the given qualifier.
     */
    private String getServerAddress(String server) {
        debug("Getting server address for server: " + server + ".");

        GetServerAddressRequest request = GetServerAddressRequest.newBuilder()
                                            .setServerServiceName(DEFAULT_SERVER_SERVICE)
                                            .setServerQualifier(server)
                                            .build();

        GetServerAddressResponse response = namingServerStub.getServerAddress(request);
        String serverAddress = response.getServerAddress();

        return serverAddress;
    }


    /**
     * Returns true if the admin is connected to the naming server, false otherwise.
     */
    private boolean isConnectedToNamingServer() {
        return namingServerStub != null;
    }

    /**
     * Closes the admin's connection to the given server. 
     */
    private void closeServerConnection(String serverQualifier) {
        debug("Closing connection with server: " + serverQualifier + ".");

        ManagedChannel channel = getChannel(serverQualifier);

        if (channel != null) {
            channel.shutdown();

            // Removes the server connection from the admin's connection maps.
            serverChannels.remove(serverQualifier);
            serverStubs.remove(serverQualifier);
        }
    }

    /**
     * Closes all the admin's server connections.
     */
    private void closeCurrServerConnections() {
        debug("Closing connections with the current admin servers.");

        for (String serverQualifier : serverStubs.keySet()) {
            closeServerConnection(serverQualifier);
        }
    }

    /**
     * Closes the admin's channel and stub to the naming server.
     */
    private void closeNamingServerConnection() {
        debug("Closing connection with the naming server.");

        namingServerChannel.shutdown();

        namingServerChannel = null;
        namingServerStub = null;
    }

    /**
     * Closes the admin's channel and stub to the naming server.
     */
    @Override
    public void close() {
        debug("Closing connection with the naming server.");

        closeCurrServerConnections();

        if (isConnectedToNamingServer()) {
            closeNamingServerConnection();
        }
    }

}
