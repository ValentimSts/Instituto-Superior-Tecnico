package pt.tecnico.distledger.server;

import io.grpc.stub.StreamObserver;
import pt.ulisboa.tecnico.distledger.contract.admin.AdminDistLedger.*;
import pt.ulisboa.tecnico.distledger.contract.admin.AdminServiceGrpc;
import pt.ulisboa.tecnico.distledger.contract.DistLedgerCommonDefinitions.LedgerState;
import pt.tecnico.distledger.server.grpc.CrossServerService;
import pt.tecnico.distledger.server.domain.*;

import java.util.List;

public class AdminDistLedgerServiceImpl extends AdminServiceGrpc.AdminServiceImplBase {

	/** Server state. */
	private ServerState serverState;
	private final CrossServerService crossServerService;

	public AdminDistLedgerServiceImpl(ServerState serverState, CrossServerService crossServerService) {
		this.serverState = serverState;
		this.crossServerService = crossServerService;
	}

	@Override
	public void activate(ActivateRequest request, StreamObserver<ActivateResponse> responseObserver) {
		serverState.activate();

		ActivateResponse response = ActivateResponse.newBuilder().build();

		responseObserver.onNext(response);
		responseObserver.onCompleted();
	}

	@Override
	public void deactivate(DeactivateRequest request, StreamObserver<DeactivateResponse> responseObserver) {
		serverState.deactivate();
		
		DeactivateResponse response = DeactivateResponse.newBuilder().build();

		responseObserver.onNext(response);
		responseObserver.onCompleted();
	}

	@Override
	public void getLedgerState(getLedgerStateRequest request, StreamObserver<getLedgerStateResponse> responseObserver) {
		LedgerState ledgerState = serverState.getLedgerState();
		
		getLedgerStateResponse response = getLedgerStateResponse.newBuilder().setLedgerState(ledgerState).build();

		responseObserver.onNext(response);
		responseObserver.onCompleted();
	}

	@Override
	public void gossip(GossipRequest request, StreamObserver<GossipResponse> responseObserver) {
		try {
			// Propagates the ledger state to all servers.
			serverState.propagateState(request.getLedgerState(), request.getReplicaTSList(), request.getReplicaId());

			GossipResponse response = GossipResponse.newBuilder().build();

			responseObserver.onNext(response);
			responseObserver.onCompleted();
		}
		catch (ServerStateException e) {
			// Do nothing.
			// If an exception is thrown here, it means that one or more propagated operations
			// weren't valid, which is something out of our control.
		}
		
	}

	@Override
	public void getLedgerToPropagate(GetInfoToPropagateRequest request, StreamObserver<GetInfoToPropagateResponse> responseObserver) {
		LedgerState ledgerState = serverState.getCorrectLedgerState(request.getReplicaId());
		List<Integer> replicaTS = serverState.getReplicaTS();

		GetInfoToPropagateResponse response = GetInfoToPropagateResponse.newBuilder().setLedgerState(ledgerState)
																						 .addAllReplicaTS(replicaTS)
																						 .build();

		responseObserver.onNext(response);
		responseObserver.onCompleted();
	}
}