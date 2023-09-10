package pt.tecnico.distledger.server.domain.operation;

import pt.ulisboa.tecnico.distledger.contract.DistLedgerCommonDefinitions;

public class CreateOp extends Operation {

    public CreateOp(String account) {
        super(account);
    }

    public DistLedgerCommonDefinitions.OperationType getType() {
        return DistLedgerCommonDefinitions.OperationType.OP_CREATE_ACCOUNT;
    }

    /**
     * Converts this operation into a GRPC operation object.
     */
    @Override
    public DistLedgerCommonDefinitions.Operation toGrpc() {
        return DistLedgerCommonDefinitions.Operation.newBuilder()
                    .setUserId(super.getAccount())
                    .setType(this.getType())
                    .addAllPrevTS(getPrevTS())
                    .addAllTS(getTS())
                    .build();
    }

}
