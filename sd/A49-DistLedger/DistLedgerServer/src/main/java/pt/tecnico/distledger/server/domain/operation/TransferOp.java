package pt.tecnico.distledger.server.domain.operation;

import pt.ulisboa.tecnico.distledger.contract.DistLedgerCommonDefinitions;

public class TransferOp extends Operation {
    private String destAccount;
    private int amount;

    public TransferOp(String fromAccount, String destAccount, int amount) {
        super(fromAccount);
        this.destAccount = destAccount;
        this.amount = amount;
    }

    public String getDestAccount() {
        return destAccount;
    }

    public void setDestAccount(String destAccount) {
        this.destAccount = destAccount;
    }

    public int getAmount() {
        return amount;
    }

    public void setAmount(int amount) {
        this.amount = amount;
    }

    public DistLedgerCommonDefinitions.OperationType getType() {
        return DistLedgerCommonDefinitions.OperationType.OP_TRANSFER_TO;
    }

    /**
     * Converts this operation into a GRPC operation object.
     */
    @Override
    public DistLedgerCommonDefinitions.Operation toGrpc() {
        return DistLedgerCommonDefinitions.Operation.newBuilder()
                    .setUserId(super.getAccount())
                    .setDestUserId(this.getDestAccount())
                    .setAmount(this.getAmount())
                    .setType(this.getType())
                    .addAllPrevTS(getPrevTS())
                    .addAllTS(getTS())
                    .build();
    }

}
