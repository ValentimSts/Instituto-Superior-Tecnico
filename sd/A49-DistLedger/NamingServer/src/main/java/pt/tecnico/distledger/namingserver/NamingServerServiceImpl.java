package pt.tecnico.distledger.namingserver;

import io.grpc.stub.StreamObserver;
import pt.ulisboa.tecnico.distledger.contract.namingserver.NamingServerDistledger.*;
import pt.ulisboa.tecnico.distledger.contract.namingserver.NamingServerServiceGrpc;
import pt.tecnico.distledger.namingserver.domain.*;

import java.lang.IllegalArgumentException;
import java.util.List;

import static io.grpc.Status.INVALID_ARGUMENT;
import static io.grpc.Status.ALREADY_EXISTS;

public class NamingServerServiceImpl extends NamingServerServiceGrpc.NamingServerServiceImplBase {

	private static final String COULD_NOT_REGISTER_SERVER = "Not possible to register the server";
	private static final String COULD_NOT_REMOVE_SERVER = "Not possible to remove the server";

	/** Naming server state. */
	private NamingServerState namingServerState;
    
	public NamingServerServiceImpl(NamingServerState namingServerState) {
		this.namingServerState = namingServerState;
	}

	@Override
	public void register(RegisterRequest request, StreamObserver<RegisterResponse> responseObserver) {

		try {
			int serverID = namingServerState.register(request.getServerServiceName(), request.getServerQualifier(), request.getServerAddress());
			
			RegisterResponse response = RegisterResponse.newBuilder().setServerID(serverID).build();
			
			responseObserver.onNext(response);
			responseObserver.onCompleted();
		}
		catch (IllegalArgumentException e) {
			responseObserver.onError(ALREADY_EXISTS.withDescription(COULD_NOT_REGISTER_SERVER).asRuntimeException());
			
		}
	}

	@Override
	public void lookup(LookupRequest request, StreamObserver<LookupResponse> responseObserver) {
		List<String> serverAddresses = namingServerState.lookup(request.getServerServiceName(), request.getServerQualifier());

		LookupResponse response = LookupResponse.newBuilder().addAllServerAddress(serverAddresses).build();

		responseObserver.onNext(response);
		responseObserver.onCompleted();
	}

	@Override
	public void delete(DeleteRequest request, StreamObserver<DeleteResponse> responseObserver) {

		try {
			namingServerState.delete(request.getServerServiceName(), request.getServerAddress(), request.getServerID());
			
			DeleteResponse response = DeleteResponse.newBuilder().build();

			responseObserver.onNext(response);
			responseObserver.onCompleted();
		}
		catch (IllegalArgumentException e) {
			responseObserver.onError(INVALID_ARGUMENT.withDescription(COULD_NOT_REMOVE_SERVER).asRuntimeException());
		}
	}

	@Override
	public void getServerID(GetServerIDRequest request, StreamObserver<GetServerIDResponse> responseObserver) {
		int serverID = namingServerState.getServerId(request.getServerServiceName(), request.getServerAddress());

		GetServerIDResponse response = GetServerIDResponse.newBuilder().setServerID(serverID).build();

		responseObserver.onNext(response);
		responseObserver.onCompleted();
	}

	@Override
	public void getServerAddress(GetServerAddressRequest request, StreamObserver<GetServerAddressResponse> responseObserver) {
		String serverAddress = namingServerState.getServerAddress(request.getServerServiceName(), request.getServerQualifier());

		GetServerAddressResponse response = GetServerAddressResponse.newBuilder().setServerAddress(serverAddress).build();

		responseObserver.onNext(response);
		responseObserver.onCompleted();
	}
}