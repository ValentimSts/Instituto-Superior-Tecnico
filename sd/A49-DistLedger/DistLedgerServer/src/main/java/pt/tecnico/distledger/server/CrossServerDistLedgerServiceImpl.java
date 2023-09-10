package pt.tecnico.distledger.server;

import io.grpc.stub.StreamObserver;
import pt.ulisboa.tecnico.distledger.contract.distledgerserver.CrossServerDistLedger.*;
import pt.ulisboa.tecnico.distledger.contract.distledgerserver.DistLedgerCrossServerServiceGrpc;
import pt.tecnico.distledger.server.domain.*;

public class CrossServerDistLedgerServiceImpl extends DistLedgerCrossServerServiceGrpc.DistLedgerCrossServerServiceImplBase {
    
    private ServerState serverState;

    public CrossServerDistLedgerServiceImpl(ServerState serverState) {
        this.serverState = serverState;
    }

    @Override
    public void propagateState(PropagateStateRequest request, StreamObserver<PropagateStateResponse> responseObserver) {
        try {
            serverState.propagateState(request.getLedger(), request.getReplicaTSList(), request.getReplicaId());

            PropagateStateResponse response = PropagateStateResponse.newBuilder().build();
            
            responseObserver.onNext(response);
            responseObserver.onCompleted();
        }
        catch (ServerStateException e) {
            responseObserver.onError(e);
        }
    }

    @Override
    public void checkServerStatus(CheckServerStatusRequest request, StreamObserver<CheckServerStatusResponse> responseObserver) {
        CheckServerStatusResponse response = CheckServerStatusResponse.newBuilder().setIsActive(serverState.isActive()).build();
        
        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }
}
