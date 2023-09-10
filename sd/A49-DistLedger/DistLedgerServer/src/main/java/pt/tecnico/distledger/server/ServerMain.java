package pt.tecnico.distledger.server;

import pt.tecnico.distledger.server.domain.ServerState;
import pt.tecnico.distledger.server.grpc.CrossServerService;
import pt.tecnico.distledger.server.grpc.NamingServerService;
import io.grpc.BindableService;
import io.grpc.Server;
import io.grpc.ServerBuilder;

import java.io.IOException;

public class ServerMain {

	public static final String PRIMARY_SERVER_QUALIFIER = "A";
	private static final String DEFAULT_HOST = "localhost";

    public static void main(String[] args) throws IOException, InterruptedException {
        System.out.println(ServerMain.class.getSimpleName());

		// receive and print arguments
		System.out.printf("Received %d argument(s):%n", args.length);
		for (int i = 0; i < args.length; i++) {
			System.out.printf("arg[%d] = %s%n", i, args[i]);
		}

		// check arguments
		if (args.length != 2) {
			System.err.println("Argument(s) missing!");
			System.err.printf("Usage: java %s port%n", ServerMain.class.getName());
			return;
		}

		// Checks if the port is valid.
		if (args[0].equals("5001")) {
			System.err.println("Port 5001 is reserved for the naming server!");
			return;
		}

		// Checks if the qualifier is valid.
		else if (args[1].length() != 1 || !Character.isLetter(args[1].charAt(0))) {
			System.err.println("Qualifier must be a single character!");
			return;
		}

		final int port = Integer.parseInt(args[0]);
		final String qualifier = args[1];

		NamingServerService namingServerService = new NamingServerService(DEFAULT_HOST, port, qualifier);
		int serverID;

		try {
			// Registers the server in the naming server, and
			// gets its designated ID.
			serverID = namingServerService.register();
		} 
		catch (RuntimeException e) {
			namingServerService.close();
			return;
		}

		CrossServerService crossServerService = new CrossServerService(DEFAULT_HOST, port);

		ServerState serverState = new ServerState(qualifier, serverID);
		final BindableService userImpl = new UserDistLedgerServiceImpl(serverState);
		final BindableService adminImpl = new AdminDistLedgerServiceImpl(serverState, crossServerService);
		final BindableService crossServerImpl = new CrossServerDistLedgerServiceImpl(serverState);

		// Since now, every server can receive "write" requests, all the servers	
		// will have a CrossServerService instance, to allow for future gossip messages.
		Server server = ServerBuilder.forPort(port)
				.addService(userImpl)
				.addService(adminImpl)
				.addService(crossServerImpl)
				.build();

		// Start the server
		server.start();

		// Server threads are running in the background.
		System.out.println("\nServer started");
		System.out.println("[Press enter to shutdown]\n");
		

		// Exit the server correctly.
		System.in.read();
		System.out.println("Shutting down server "+ qualifier + "...");

		// Unregisters the server from the naming server.
		namingServerService.delete();
		namingServerService.close();

		if (crossServerService != null) {
			crossServerService.close();
		}
		
		server.shutdown();
    }

}

