package pt.tecnico.distledger.namingserver;

import io.grpc.BindableService;
import io.grpc.Server;
import io.grpc.ServerBuilder;
import pt.tecnico.distledger.namingserver.domain.NamingServerState;

import java.io.IOException;

public class NamingServerMain {
    
    private static final int port = 5001;

    public static void main(String[] args) throws IOException, InterruptedException {

        System.out.println(NamingServerMain.class.getSimpleName());

        NamingServerState namingServerState = new NamingServerState();

        final BindableService namingServerImpl = new NamingServerServiceImpl(namingServerState);
        
        // Create a new server to listen on port
        Server namingServer = ServerBuilder.forPort(port).addService(namingServerImpl).build();
        
        // Start the server
        namingServer.start();

        // Server threads are running in the background.
        System.out.println("Naming server started\n");

        // Do not exit the main thread. Wait until server is terminated.
		namingServer.awaitTermination();

		System.out.println("Naming server terminated");
    }
    
}
