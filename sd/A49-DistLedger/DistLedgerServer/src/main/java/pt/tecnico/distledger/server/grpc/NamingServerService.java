package pt.tecnico.distledger.server.grpc;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;

import pt.ulisboa.tecnico.distledger.contract.namingserver.NamingServerServiceGrpc;
import pt.ulisboa.tecnico.distledger.contract.namingserver.NamingServerDistledger.*;

import java.lang.RuntimeException;

public class NamingServerService implements AutoCloseable {

    private static final String DEFAULT_SERVICE = "DistLedger";

    private static final String NAMING_SERVER_HOST = "localhost";
    private static final int NAMING_SERVER_PORT = 5001;

    /** 
     * Set flag to true to print debug messages. 
	 * The flag can be set using the -Ddebug command line option.
     */
	private static final boolean DEBUG_FLAG = (System.getProperty("debug") != null);

    private final String namingServerAddress;

    // New server information.
    private final String address;
    private final String qualifier;
    // ID given to the server when it's registered.
    private int id;

    private ManagedChannel namingServerChannel;
    private NamingServerServiceGrpc.NamingServerServiceBlockingStub namingServerStub;


    public NamingServerService(String  host, int port, String qualifier) {
        this.address = host + ":" + port;
        this.qualifier = qualifier;

        this.namingServerAddress = NAMING_SERVER_HOST + ":" + NAMING_SERVER_PORT;

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

    public String getQualifier() {
        return qualifier;
    }

    public String getAddress() {
        return address;
    }

    public String getNamingServerAddress() {
        return namingServerAddress;
    }

    public int getID() {
        return id;
    }

    public void setID(int id) {
        this.id = id;
    }

    /**
     * Creates the channel and stub used to connect
     * to the naming server.
     */
    public void connectNamingServer() {
        final String target = getNamingServerAddress();

        debug("Establishing connection with target: " + target + ".");

        namingServerChannel = ManagedChannelBuilder.forTarget(target).usePlaintext().build();
        namingServerStub = NamingServerServiceGrpc.newBlockingStub(namingServerChannel);
    }
    
    /**
     * Registers the server in the naming server, and
     * returns its designated ID.
     */
    public int register() throws RuntimeException {
        debug("Registering server in the naming server.");

        try {
            RegisterRequest request = RegisterRequest.newBuilder()
                .setServerServiceName(DEFAULT_SERVICE)
                .setServerQualifier(getQualifier())
                .setServerAddress(getAddress())
                .build();

            RegisterResponse response = namingServerStub.register(request);

            int serverID = response.getServerID();
            setID(serverID);

            return serverID;
        }
        catch (RuntimeException e) {
            // Since the server registration failed, we close the connection.
            close();
            System.out.println(e.getMessage());

            throw new RuntimeException(e.getMessage());
        }
    }

    /**
     * Deletes the server from the naming server.
     */
    public void delete() {
        debug("Deleting server from the naming server.");

        try {
            DeleteRequest request = DeleteRequest.newBuilder()
                .setServerServiceName(DEFAULT_SERVICE)
                .setServerAddress(getAddress())
                .setServerID(getID())
                .build();

            namingServerStub.delete(request);
        }
        catch (RuntimeException e) {
            // Since the server deletion failed, we close the connection.
            close();
            System.out.println(e.getMessage());

            throw new RuntimeException(e.getMessage());
        }

    }

    @Override
    public void close() {
        debug("Closing connection with the naming server.");
        
        if (namingServerChannel != null) {
            namingServerChannel.shutdownNow();
        }

        namingServerChannel = null;
        namingServerStub = null;
    }

}
