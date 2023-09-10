package pt.tecnico.distledger.server.domain.operation;

import pt.ulisboa.tecnico.distledger.contract.DistLedgerCommonDefinitions;

import static pt.tecnico.distledger.common.Common.MAX_REPLICAS;

import java.util.List;
import java.util.ArrayList;

public class Operation {
    private String account;

    private boolean isStable;

    private List<Integer> prevTS;
    private List<Integer> TS;

    public Operation(String fromAccount) {
        this.account = fromAccount;

        this.prevTS = new ArrayList<>(MAX_REPLICAS);
        this.TS = new ArrayList<>(MAX_REPLICAS);
    }

    public String getAccount() {
        return account;
    }

    public void setAccount(String account) {
        this.account = account;
    }

    public boolean isStable() {
        return isStable;
    }

    public void setStable(boolean state) {
        isStable = state;
    }

    public List<Integer> getPrevTS() {
        return prevTS;
    }

    public void setPrevTS(List<Integer> prevTS) {
        this.prevTS = prevTS;
    }

    public List<Integer> getTS() {
        return TS;
    }

    public void setTS(List<Integer> TS) {
        this.TS = TS;
    }

    public DistLedgerCommonDefinitions.OperationType getType() {
        // Implemented by subclasses.
        return null;
    }

    /**
     * Converts this operation into a GRPC operation object.
     */
    public DistLedgerCommonDefinitions.Operation toGrpc() {
        return DistLedgerCommonDefinitions.Operation.newBuilder()
                .setUserId(getAccount())
                .addAllPrevTS(getPrevTS())
                .addAllTS(getTS())
                .build();
    }
}
