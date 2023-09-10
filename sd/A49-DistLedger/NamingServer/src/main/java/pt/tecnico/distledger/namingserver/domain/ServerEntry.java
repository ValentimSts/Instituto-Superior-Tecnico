package pt.tecnico.distledger.namingserver.domain;

public class ServerEntry {

    private String qualifier;
    private String address;
    private int id;

    public ServerEntry(String qualifier, String address, int id) {
        this.qualifier = qualifier;
        this.address = address;
        this.id = id;
    }

    public String getQualifier() {
        return qualifier;
    }
    
    public void setQualifier(String qualifier) {
        this.qualifier = qualifier;
    }

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
        this.address = address;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }
    
}
