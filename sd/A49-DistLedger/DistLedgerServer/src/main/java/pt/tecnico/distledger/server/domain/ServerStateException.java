package pt.tecnico.distledger.server.domain;

public class ServerStateException extends Exception {

    private int errorCode;

    public ServerStateException(int errorCode) {
        this.errorCode = errorCode;
    }

    public int getErrorCode() {
        return errorCode;
    }

    public void setErrorCode(int errorCode) {
        this.errorCode = errorCode;
    }

}
