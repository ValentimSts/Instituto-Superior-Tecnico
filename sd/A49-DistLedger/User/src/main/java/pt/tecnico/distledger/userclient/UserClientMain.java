package pt.tecnico.distledger.userclient;


import pt.tecnico.distledger.userclient.grpc.UserService;

public class UserClientMain {

    private static final String DEFAULT_HOST = "localhost";
    private static final int NAMING_SERVER_PORT = 5001;

    public static void main(String[] args) {

        System.out.println(UserClientMain.class.getSimpleName());

        // receive and print arguments
        System.out.printf("Received %d arguments%n", args.length);
        for (int i = 0; i < args.length; i++) {
            System.out.printf("arg[%d] = %s%n", i, args[i]);
        }

        final String host = DEFAULT_HOST;
        final int port = NAMING_SERVER_PORT;

        CommandParser parser = new CommandParser(new UserService(host, port));
        parser.parseInput();
    }
}
