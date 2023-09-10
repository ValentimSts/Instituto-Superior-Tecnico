package pt.tecnico.distledger.server;

import io.grpc.stub.StreamObserver;
import pt.ulisboa.tecnico.distledger.contract.user.UserDistLedger.*;
import pt.ulisboa.tecnico.distledger.contract.DistLedgerCommonDefinitions;
import pt.ulisboa.tecnico.distledger.contract.user.UserServiceGrpc;
import pt.tecnico.distledger.server.domain.*;

import static io.grpc.Status.INVALID_ARGUMENT;
import static io.grpc.Status.UNAVAILABLE;
import static io.grpc.Status.ALREADY_EXISTS;

public class UserDistLedgerServiceImpl extends UserServiceGrpc.UserServiceImplBase {

	private static final String INVALID_USER_ID = "Invalid userId.\n";
	private static final String ACCOUNT_ALREADY_EXISTS = "An account for this userId already exists.\n";
	private static final String TRANSFER_CANCELLED = "Transfer cancelled.\n";
	private static final String INACTIVE_SERVER = "The server is inactive.\n";
	private static final String CANNOT_WRITE_TO_SECONDARY = "Unable to write to a secondary server.\n";
	private static final String PROPAGATE_FAILURE = "At least one server is inactive. Operation cancelled.\n";
    private static final String PENDING_REQUEST = "The query request is pending.\n";

	private static final int INACTIVE_ERROR = -2;
    private static final int UNSTABLE_OP_ERROR = -4;

	private final ServerState serverState;
    
	public UserDistLedgerServiceImpl(ServerState serverState) {
		this.serverState = serverState;
	}

	@Override
	public void balance(BalanceRequest request, StreamObserver<BalanceResponse> responseObserver) {
	
		try {
			int result = serverState.getBalance(request.getUserId(), request.getPrevTSList());

			BalanceResponse response = BalanceResponse.newBuilder().setValue(result).addAllValueTS(serverState.getValueTS()).build();

			responseObserver.onNext(response);
			responseObserver.onCompleted();
		}
		catch (ServerStateException e) {
			if (e.getErrorCode() == INACTIVE_ERROR) {
				responseObserver.onError(UNAVAILABLE.withDescription(INACTIVE_SERVER).asRuntimeException());
			}
			else if(e.getErrorCode() == UNSTABLE_OP_ERROR) {
				responseObserver.onError(UNAVAILABLE.withDescription(PENDING_REQUEST).asRuntimeException());
			}
			else {
				responseObserver.onError(INVALID_ARGUMENT.withDescription(INVALID_USER_ID).asRuntimeException());
			}
		}
		catch (InterruptedException e) {
			// An error occured while the replica passively waited.
			
			// Filler code to avoid compilation errors.
			responseObserver.onError(UNAVAILABLE.withDescription(PENDING_REQUEST).asRuntimeException());
		}
	}

	@Override
	public void createAccount(CreateAccountRequest request, StreamObserver<CreateAccountResponse> responseObserver) {

		try {
			serverState.createAccount(request.getUserId(), request.getPrevTSList());

			CreateAccountResponse response = CreateAccountResponse.newBuilder().addAllTS(serverState.getValueTS()).build();
			
			responseObserver.onNext(response);
			responseObserver.onCompleted();
		}
		catch (ServerStateException e) {
			if (e.getErrorCode() == INACTIVE_ERROR) {
				responseObserver.onError(UNAVAILABLE.withDescription(INACTIVE_SERVER).asRuntimeException());
			}
			else {
				responseObserver.onError(ALREADY_EXISTS.withDescription(ACCOUNT_ALREADY_EXISTS).asRuntimeException());
			}
		}
	}

	@Override
	public void transferTo(TransferToRequest request, StreamObserver<TransferToResponse> responseObserver) {
		
		try {
			serverState.transferTo(request.getAccountFrom(), request.getAccountTo(),
								   request.getAmount(), request.getPrevTSList());

			TransferToResponse response = TransferToResponse.newBuilder().addAllTS(serverState.getValueTS()).build();
			
			responseObserver.onNext(response);
			responseObserver.onCompleted();
		}
		catch (ServerStateException e) {
			if (e.getErrorCode() == INACTIVE_ERROR) {
				responseObserver.onError(UNAVAILABLE.withDescription(INACTIVE_SERVER).asRuntimeException());
			}
			else {
				responseObserver.onError(INVALID_ARGUMENT.withDescription(TRANSFER_CANCELLED).asRuntimeException());
			}
		}
	}

}
