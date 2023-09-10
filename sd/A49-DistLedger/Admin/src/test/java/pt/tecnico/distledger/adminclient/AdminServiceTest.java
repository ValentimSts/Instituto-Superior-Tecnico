package pt.tecnico.distledger.adminclient;

import org.junit.jupiter.api.*;
import static org.junit.jupiter.api.Assertions.assertEquals;

import pt.tecnico.distledger.adminclient.grpc.AdminService;

/**
 * Server must be running for the tests to work.
 */
public class AdminServiceTest {

    /**
     * Configure host name, server port and server name.
     */
    private static final String host = "localhost";
    private static final int port = 2001;
    private static final String serverName = "A";

    private AdminService adminService;

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
        adminService = new AdminService(host, port);
    }

    @AfterEach
    public void tearDown() {
        adminService = null;
    }

    // TODO: maybe implement tests.
    
}
