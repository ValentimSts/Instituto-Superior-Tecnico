package pt.tecnico.distledger.userclient;


import org.junit.jupiter.api.*;
import static org.junit.jupiter.api.Assertions.assertEquals;

import pt.tecnico.distledger.userclient.grpc.UserService;

/**
 * Server must be running for the tests to work.
 */
public class UserServiceTest {

    /**
     * Configure host name, server port and server name.
     */
    private static final String host = "localhost";
    private static final int port = 2001;
    private static final String serverName = "A";

    private UserService userService;

    @BeforeAll
    public static void oneTimeSetUp() {
        // one-time initialization code
    }

    @AfterAll
    public static void oneTimeTearDown() {
        // one-time cleanup code
    }

    @BeforeEach
    public void setUp() {
        userService = new UserService(host, port);
    }

    @AfterEach
    public void tearDown() {
        userService = null;
    }

    // TODO: maybe implement tests.
    
}
