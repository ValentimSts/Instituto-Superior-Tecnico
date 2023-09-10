package pt.tecnico.distledger.namingserver.domain;

import static pt.tecnico.distledger.common.Common.MAX_REPLICAS;

import java.lang.IllegalArgumentException;

import java.util.HashMap;
import java.util.Map;

import java.util.List;
import java.util.ArrayList;

public class NamingServerState {

    private Map<String, ServiceEntry> services;

    // List used to identify the servers in the naming server.
    private List<Integer> serverIDs;

    /** 
     * Set flag to true to print debug messages. 
	 * The flag can be set using the -Ddebug command line option.
     */
	private static final boolean DEBUG_FLAG = (System.getProperty("debug") != null);

    public NamingServerState() {
        this.services = new HashMap<String, ServiceEntry>();

        // Initializes the server IDs list.
        initializeServerIDs(MAX_REPLICAS);
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
     * Initializes the naming server's server ID list, setting all its 
     * entries to 0.
     */
    private void initializeServerIDs(int size) {
        this.serverIDs = new ArrayList<Integer>(size);

        for (int i = 0; i < size; i++) {
            serverIDs.add(0);
        }
    }

    public Map<String, ServiceEntry> getServices() {
        return services;
    }

    public synchronized void setServices(Map<String, ServiceEntry> services) {
        this.services = services;
    }

    public synchronized void addService(ServiceEntry service) {
        this.services.put(service.getServiceName(), service);
    }

    /**
     * Finds and returns the first available server ID.
     * Throws an exception if no server ID is available.
     * 
     *  - An available server ID is set to 0
     */
    private int getAvailableServerID() {
        for (int i = 0; i < serverIDs.size(); i++) {
            if (serverIDs.get(i) == 0) {
                serverIDs.set(i, 1);
                return i;
            }
        }

        throw new IllegalArgumentException("The max amount of servers has been reached.");
    }


    /**
     * Registers a new server in the naming server, and returns
     * the an available ID for it.
     */
    public synchronized int register(String serverServiceName, String serverQualifier, String serverAddress) throws IllegalArgumentException {
        debug("Registering server: (" + serverServiceName + " - " + serverQualifier + " - " + serverAddress + ") in the naming server.");

        ServiceEntry service;

        // Find an available ID for the new server.
        int serverID = getAvailableServerID();
       
        if (services.containsKey(serverServiceName)) {
            // The service already exists.

            service = services.get(serverServiceName);

            if (service.getServerEntry(serverAddress) != null) {
                // The server already exists.
                throw new IllegalArgumentException();
            }
            else if (!service.isValidQualifier(serverQualifier)) {
                // The qualifier is not valid.
                throw new IllegalArgumentException();
            }
        }
        else {
            // The service does not exist.
            service = new ServiceEntry(serverServiceName);
            addService(service);
        }

        ServerEntry server = new ServerEntry(serverQualifier, serverAddress, serverID);
        service.addServerEntry(server);

        return serverID;
    }

    /**
     * Lets the user know the address of a server registered in the naming server.
     * Returns:
     *  - a list of service that provide the given service and have the same qualifier as the one provided;
     *  - a list of all the servers that provide the given service, if the qualifier is ommited;
     *  - an empty list if the qualifier and/or service do not exist.
     */
    public synchronized List<String> lookup(String serverServiceName, String serverQualifier) {
        debug("Finding server address for: (" + serverServiceName + " - " + serverQualifier + ") in the naming server.");

        List<String> serverAddresses = new ArrayList<String>();

        if (serverServiceName == "") {
            // Return an empty list.
        }
        else if (serverQualifier == "") {
            // The server qualifier was ommited in the request.
            // - The user wants all the servers that provide the given service.
            
            ServiceEntry service = services.get(serverServiceName);

            if (service != null) {
                // The service exists.
                for (ServerEntry server : service.getServerEntries()) {
                    serverAddresses.add(server.getAddress());
                }
            }
        }
        else {
            // The server service and qualifier were provided in the request.
            // - The user wants a specific server that provides the given service.
            ServiceEntry service = services.get(serverServiceName);

            if (service != null) {
                // The service exists.
                for (ServerEntry server : service.getServerEntries()) {
                    if (server.getQualifier().equals(serverQualifier)) {
                        serverAddresses.add(server.getAddress());
                    }
                }
                
            }
        }
        
        return serverAddresses;
    }

    /**
     * Removes a server from the naming server.
     */
    public synchronized void delete(String serverServiceName, String serverAddress, int serverID) throws IllegalArgumentException {
        debug("Removing server: (" + serverServiceName + " - " + serverAddress + ") from the naming server.");
        
        // Checks if the given server ID is valid.
        if (serverID < 0 || serverID >= MAX_REPLICAS) {
            throw new IllegalArgumentException();
        }

        if (!services.containsKey(serverServiceName)) {
            throw new IllegalArgumentException();
        }

        ServiceEntry service = services.get(serverServiceName);
        ServerEntry server = service.getServerEntry(serverAddress);

        if (server == null) {
            throw new IllegalArgumentException();
        }
        
        service.removeServerEntry(server);

        // Since the server was removed from the system, we set the
        // server ID to 0, indicating that its entry is now available.
        serverIDs.set(serverID, 0);
    }

    /**
     * Returns the server ID for a given server address.
     */
    public synchronized int getServerId(String serverServiceName, String serverAddress) {
        debug("Getting server ID for: (" + serverServiceName + " - " + serverAddress + ") in the naming server.");

        ServerEntry server = services.get(serverServiceName).getServerEntry(serverAddress);

        if (server == null) {
            throw new IllegalArgumentException();
        }

        return server.getId();
    }

    /**
     * Returns the server address for a given server qualifier.
     */
    public synchronized String getServerAddress(String serverServiceName, String serverQualifier) {
        debug("Getting server address for: (" + serverServiceName + " - " + serverQualifier + ") in the naming server.");

        ServerEntry server = services.get(serverServiceName).getServerEntryFromQualifier(serverQualifier);

        if (server == null) {
            throw new IllegalArgumentException();
        }

        return server.getAddress();
    }

    /**
     * Prints the state of the naming server.
     *  - Used only for debugging purposes.
     */
    @SuppressWarnings("unused")
    private synchronized void printNamingServer() {
        System.out.println("Naming Server State:");

        for (ServiceEntry service : services.values()) {
            System.out.println(service.getServiceName() + ":");

            for (ServerEntry server : service.getServerEntries()) {
                System.out.println("\t[" + server.getQualifier() + " - " + server.getAddress() + "]");
            }
        }
    }

}