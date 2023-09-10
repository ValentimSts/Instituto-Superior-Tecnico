package pt.tecnico.distledger.namingserver.domain;

import java.util.Set;
import java.util.HashSet;

public class ServiceEntry {

    private String serviceName;

    private Set<ServerEntry> serverEntries;

    public ServiceEntry(String serviceName) {
        this.serviceName = serviceName;
        this.serverEntries = new HashSet<ServerEntry>();
    }

    public String getServiceName() {
        return serviceName;
    }
    
    public void setServiceName(String serviceName) {
        this.serviceName = serviceName;
    }

    public Set<ServerEntry> getServerEntries() {
        return serverEntries;
    }

    public void setServerEntries(Set<ServerEntry> serverEntries) {
        this.serverEntries = serverEntries;
    }

    public void addServerEntry(ServerEntry serverEntry) {
        this.serverEntries.add(serverEntry);
    }

    /**
     * Returns the server entry with the given address, or null if it doesn't exist.
     */
    public ServerEntry getServerEntry(String serverAddress) {
        for (ServerEntry serverEntry : this.serverEntries) {
            if (serverEntry.getAddress().equals(serverAddress)) {
                return serverEntry;
            }
        }

        return null;
    }

    /**
     * Returns the server entry with the given qualifier, or null if it doesn't exist.
     */
    public ServerEntry getServerEntryFromQualifier(String serverQualifier) {
        for (ServerEntry serverEntry : this.serverEntries) {
            if (serverEntry.getQualifier().equals(serverQualifier)) {
                return serverEntry;
            }
        }

        return null;
    }

    /**
     * Returns true if the given qualifier is valid.
     *  - (There can't be duplicate qualifiers in the system).
     */
    public boolean isValidQualifier(String qualifier) {

        for (ServerEntry serverEntry : this.serverEntries) {
            if (serverEntry.getQualifier().equals(qualifier)) {
                return false;
            }
        }
        
        return true;
    }

    public void removeServerEntry(ServerEntry serverEntry) {
        this.serverEntries.remove(serverEntry);
    }
    
}
