package pt.tecnico.distledger.server;

import org.junit.jupiter.api.*;
import static org.junit.jupiter.api.Assertions.assertEquals;

import pt.tecnico.distledger.server.domain.*;

public class ServerStateTest {

    private static final int INVALID_ARGUMENT_ERROR = -1;
    private static final int INACTIVE_ERROR = -2;
    private static final int INVALID_ACCOUNT_ERROR = -3;

    private ServerState serverState;

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
        serverState = new ServerState("A");
    }

    @AfterEach
    public void tearDown() {
        serverState = null;
    }

    @Test
    public void testCreateAccountTwice() {
        try {
            serverState.createAccount("alice");
            serverState.createAccount("alice");
        }
        catch (ServerStateException e) {
            assertEquals(INVALID_ARGUMENT_ERROR, e.getErrorCode());
        }  
    }

    @Test
    public void testDeleteAccountOpCount() {
        try {
            serverState.createAccount("alice");
            serverState.deleteAccount("alice");
        } catch (ServerStateException e) { 
            // No exception should be thrown.
        }
        
        assertEquals(2, serverState.getLedger().size());
    }

    @Test
    public void testDeleteAccount() {
        try {
            serverState.createAccount("alice");
            serverState.deleteAccount("alice");
            serverState.getBalance("alice");
        }
        catch (ServerStateException e) { 
            assertEquals(INVALID_ACCOUNT_ERROR, e.getErrorCode());

        }
    }

    @Test
    public void testDeleteAccountNonExistant() {
        try {
            serverState.deleteAccount("alice");
        }
        catch (ServerStateException e) {
            assertEquals(INVALID_ACCOUNT_ERROR, e.getErrorCode());
        }
    }

    @Test
    public void testGetBalance() {
        try {
            serverState.createAccount("alice");
            int res = serverState.getBalance("alice");

            assertEquals(0, res);
        }
        catch (ServerStateException e) {
            // No exception should be thrown.
        }
    }

    @Test
    public void testGetBalanceNonExistent() {
        try {
            serverState.getBalance("alice");
        }
        catch (ServerStateException e) {
            assertEquals(INVALID_ACCOUNT_ERROR, e.getErrorCode());
        }
    }

    @Test
    public void testBrokerTransfer() {
        try {
            serverState.createAccount("alice");
            serverState.transferTo("broker", "alice", 50);
            int res = serverState.getBalance("alice");

            assertEquals(50, res);
        }
        catch (ServerStateException e) {
            // No exception should be thrown.
        }
    }

    @Test
    public void testBrokerTransferNonExistent() {
        try {
            serverState.transferTo("broker", "alice", 50);
        }
        catch (ServerStateException e) {
            assertEquals(INVALID_ARGUMENT_ERROR, e.getErrorCode());
        }
    }

    @Test
    public void testTransferSuccess() {
        try {
            serverState.createAccount("alice");
            serverState.transferTo("broker", "alice", 50);
            serverState.createAccount("bob");
            serverState.transferTo("alice", "bob", 30);
            int res = serverState.getBalance("bob");

            assertEquals(30, res);
        }
        catch (ServerStateException e) {
            // No exception should be thrown.
        }
    }

    @Test
    public void testTransferInsufficientFunds() {
        try {
            serverState.createAccount("alice");
            serverState.transferTo("broker", "alice", 50);
            serverState.createAccount("bob");
            serverState.transferTo("alice", "bob", 60);
        }
        catch (ServerStateException e) {
            assertEquals(INVALID_ARGUMENT_ERROR, e.getErrorCode());
        }
    }

    @Test
    public void testTransferNonExistent() {
        try {
            serverState.transferTo("alice", "bob", 30);
        }
        catch (ServerStateException e) {
            assertEquals(e.getErrorCode(), INVALID_ARGUMENT_ERROR);
        }  
    }

    @Test
    public void testTransferToSelf() {
        try {
            serverState.createAccount("alice");
            serverState.transferTo("broker", "alice", 50);
            serverState.transferTo("alice", "alice", 30);
        }
        catch (ServerStateException e) {
            assertEquals(INVALID_ARGUMENT_ERROR, e.getErrorCode());
        }
    }

    @Test
    public void testTransferZero() {
        try {
            serverState.createAccount("alice");
            serverState.createAccount("bob");
            serverState.transferTo("broker", "alice", 50);
            serverState.transferTo("alice", "bob", 0);
        }
        catch (ServerStateException e) {
            assertEquals(INVALID_ARGUMENT_ERROR, e.getErrorCode());
        }
    }

    @Test
    public void testInitialState() {
        assertEquals(true, serverState.isActive());
    }    

    @Test
    public void testActivate() {
        serverState.activate();
        assertEquals(true, serverState.isActive());
    }

    @Test
    public void testDeactivate() {
        serverState.deactivate();
        assertEquals(false, serverState.isActive());
    }

    @Test
    public void testCreateAccountDeactivated() {
        try {
            serverState.deactivate();
            serverState.createAccount("alice");
        }
        catch (ServerStateException e) {
            assertEquals(INACTIVE_ERROR, e.getErrorCode());
        }
    }

    @Test
    public void testDeleteAccountDeactivated() {
        try {
            serverState.createAccount("alice");
            serverState.deactivate();
            serverState.deleteAccount("alice");
        }
        catch (ServerStateException e) {
            assertEquals(INACTIVE_ERROR, e.getErrorCode());
        }
    }

    @Test
    public void testGetBalanceDeactivated() {
        try {
            serverState.createAccount("alice");
            serverState.deactivate();
            serverState.getBalance("alice");
        }
        catch (ServerStateException e) {
            assertEquals(INACTIVE_ERROR, e.getErrorCode());
        }
    }

    @Test
    public void testTransferDeactivated() {
        try {
            serverState.createAccount("alice");
            serverState.transferTo("broker", "alice", 50);
            serverState.createAccount("bob");
            serverState.deactivate();
            serverState.transferTo("alice", "bob", 30);
        }
        catch (ServerStateException e) {
            assertEquals(INACTIVE_ERROR, e.getErrorCode());
        }
    }

}
