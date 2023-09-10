package pt.tecnico.distledger.server.domain;

import pt.tecnico.distledger.server.domain.operation.*;
import pt.ulisboa.tecnico.distledger.contract.DistLedgerCommonDefinitions;
import pt.ulisboa.tecnico.distledger.contract.DistLedgerCommonDefinitions.LedgerState;

import static pt.tecnico.distledger.common.Common.MAX_REPLICAS;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Collections;

public class ServerState {

    private static final String BROKER_ID = "broker";
    private static final Integer BROKER_BALANCE = 1000;

    // Error codes.
    private static final int INVALID_ARGUMENT_ERROR = -1;
    private static final int INACTIVE_ERROR = -2;
    private static final int INVALID_ACCOUNT_ERROR = -3;
    private static final int UNSTABLE_OP_ERROR = -4;

    /** 
     * Set flag to true to print debug messages. 
	 * The flag can be set using the -Ddebug command line option.
     */
	private static final boolean DEBUG_FLAG = (System.getProperty("debug") != null);
    
    // Possible server states, determined by the admin.
    // Starts with 'ACTIVE' by default.
    private enum state {ACTIVE, INACTIVE};
    private state currState;

    // List of accepted opperations. 
    private List<Operation> ledger;

    // Map of active accounts (userId, balance). 
    private Map<String, Integer> activeAccounts;

    // Server qualifier
    private String qualifier;

    // Gossip architecture: replica manager atributes.
    private int id;
    private List<Integer> valueTS;
    private List<Integer> replicaTS;
    // lsit containing the TSs of the most recent operation sent via gossip,
    // to each replica.
    private List<List<Integer>> tableTS;

    // List of threads waiting for a timestamp update.
    // (Not working properly),
    private List<ServerStateProcess> waitList;
    

    public ServerState(String qualifier, int id) {
        this.currState = state.ACTIVE;
        this.ledger = new ArrayList<>();
        this.activeAccounts = new HashMap<>();
        this.qualifier = qualifier;

        this.id = id;
        // Initializes the server's timestamps.
        initializeTS(MAX_REPLICAS);
        
        // Initializes server state with broker account.
        activeAccounts.put(BROKER_ID, BROKER_BALANCE);
    }

    /**
     * Prints a debug message if the debug flag is set to true.
     */
    public void debug(String message) {
        if (DEBUG_FLAG) {
            System.err.println("DEBUG: " + message);
        }
    }

    /**
     * Initializes the server's timestamps, setting all its 
     * entries to 0.
     */
    public void initializeTS(int size) {
        this.valueTS = new ArrayList<>(size);
        this.replicaTS = new ArrayList<>(size);

        this.waitList = new ArrayList<>();

        this.tableTS = new ArrayList<>(size);

        for (int i = 0; i < size; i++) {
            valueTS.add(0);
            replicaTS.add(0);

            tableTS.add(new ArrayList<>(size));
        }
    }

    public state getState() {
        return currState;
    }

    public synchronized void setState(state state) {
        this.currState = state;
    }

    public List<Operation> getLedger() {
        return ledger;
    }

    public synchronized void setLedger(List<Operation> ledger) {
        this.ledger = ledger;
    }

    public Map<String, Integer> getActiveAccounts() {
        return activeAccounts;
    }

    public String getQualifier() {
        return qualifier;
    }

    public synchronized void setQualifier(String qualifier) {
        this.qualifier = qualifier;
    }

    public int getId() {
        return id;
    }

    public synchronized void setId(int id) {
        this.id = id;
    }

    public List<Integer> getValueTS() {
        return valueTS;
    }

    public synchronized void setValueTS(List<Integer> valueTS) {
        this.valueTS = valueTS;
    }

    public List<Integer> getReplicaTS() {
        return replicaTS;
    }

    public synchronized void setReplicaTS(List<Integer> replicaTS) {
        this.replicaTS = replicaTS;
    }

    /**
     * Checks if the server is currently in 'ACTIVE' mode.
     */
    public boolean isActive() {
        return currState == state.ACTIVE;
    }

    /*
     * Returns the given userId's active account balance.
     * 
     *  - Returns the account balance if successful
     *  - Throws a ServerStateException if an error occurs.
     */
    public Integer getBalance(String userId, List<Integer> prevTS) throws ServerStateException, InterruptedException {
        debug("Getting the balance for the account: " + userId + ".");

        if (!isActive()) {
            throw new ServerStateException(INACTIVE_ERROR);
        }

        if (!activeAccounts.containsKey(userId)) {
            throw new ServerStateException(INVALID_ACCOUNT_ERROR);
        }

        // Checks if the replica can send a value to the client.
        // If not, an exception is thrown.
        checkValueTS(prevTS);

        return activeAccounts.get(userId);
    }

    /**
     * Updates the given operation's fields and adds it to the ledger.
     */
    private synchronized void addOperation(Operation op, List<Integer> prevTS, boolean isStable) throws ServerStateException {
        op.setPrevTS(prevTS);

        // Updates the replica timestamps.
        replicaTS.set(id, replicaTS.get(id) + 1);
        debug("New replicaTS: " + replicaTS + ".");
        
        List<Integer> TS = new ArrayList<>(MAX_REPLICAS);
        for (int i = 0; i < MAX_REPLICAS; i++) {
            if (i == id) {
                TS.add(replicaTS.get(i));
            }
            else {
                TS.add(prevTS.get(i));
            }
        }

        op.setTS(TS);
        op.setStable(isStable);

        // Be it stable or unstable, the operation is always added to the ledger.
        ledger.add(op);
    }

    /**
     * Increments the server's valueTS and notifies any threads that 
     * can now send a value to the client.
     */
    private void incrementTS() {
        valueTS.set(id, valueTS.get(id) + 1);

        debug("New valueTS: " + valueTS + ".");

        // Method was called to notify any threads that they can now 
        // send a value to the client (Not working).
        // notifyOps();
    }

    /*
     * Creates a new account with the given userId in the system, 
     * while also dealing with all the timestamps.
     */
    public synchronized void createAccount(String userId, List<Integer> prevTS) throws ServerStateException {
        debug("Creating a new account with userId: " + userId + ".");
        debug("Current valueTS: " + valueTS + ".");
        debug("Current replicaTS: " + replicaTS + ".");

        // Checks if the operation is stable or not.
        boolean isStable = checkStableOp(prevTS);

        Operation op = new CreateOp(userId);
        addOperation(op, prevTS, isStable);

        if (isStable) {
            // The operation is only immediatly executed if it's stable
            // and valid.
            createAccountExec(userId);
        }
    }

    /*
     * Does all the verifications needed to create a new account, and 
     * throws a ServerStateException if there's an error.
     * 
     *  - The method is 'synchronized' to prevent memory consistency errors.
     */
    private synchronized void verifyCreateAccount(String userId) throws ServerStateException {
        debug("Verifying if it's possible to create a new account with userId: " + userId + ".");

        if (!isActive()) {
            throw new ServerStateException(INACTIVE_ERROR);
        }

        if (activeAccounts.containsKey(userId)) {
            throw new ServerStateException(INVALID_ARGUMENT_ERROR);
        }
    }

    /*
     * Creates a new account with the given userId, setting its balance to 0.
     * 
     *  - The method is 'synchronized' to prevent memory consistency errors.
     */
    private synchronized void createAccountExec(String userId) throws ServerStateException {
        debug("Executing the createAccount operation, for userId: " + userId + ".");

        verifyCreateAccount(userId);
        // Updates the server's value timestamp, as it will now execute an update.
        incrementTS();
        
        activeAccounts.put(userId, 0);  
    }

    /*
     * In case of an unsuccessful CreateOp state propagation,
     * undoes the previous action.
     */
    public synchronized void undoCreateAccount(String userId) {
        debug("Undoing the recently added account with this userId: " + userId + ".");

        activeAccounts.remove(userId);
        ledger.remove(ledger.size() - 1);
    }


    public void transferTo(String accountFrom, String accountTo, int amount, List<Integer> prevTS) throws ServerStateException {
        debug("Transferring " + amount + " from " + accountFrom + " to " + accountTo + ".");
        debug("Current valueTS: " + valueTS + ".");
        debug("Current replicaTS: " + replicaTS + ".");

        // Checks if the operation is stable or not.
        boolean isStable = checkStableOp(prevTS);

        Operation op = new TransferOp(accountFrom, accountTo, amount);
        addOperation(op, prevTS, isStable);

        if (isStable) {
            // The operation is only immediatly executed if it´s stable
            // and valid
            transferToExec(accountFrom, accountTo, amount);
        }
    
    }

    /*
     * Does all the verifications needed to transfer an amount between accounts, and 
     * throws a ServerStateException if there's an error.
     * 
     *  - The method is 'synchronized' to prevent memory consistency errors.
     */
    private void verifyTransferTo(String accountFrom, String accountTo, int amount) throws ServerStateException {
        debug("Verifying if it's possible to transfer " + amount + " from " + accountFrom + " to " + accountTo + ".");

        if (!isActive()) {
            throw new ServerStateException(INACTIVE_ERROR);
        }

        // A user can't transfer money to himself, nor can he make 
        // a transfer with amount = 0.

        if (!activeAccounts.containsKey(accountFrom) ||
            !activeAccounts.containsKey(accountTo) ||
            amount <= 0 || activeAccounts.get(accountFrom) < amount ||
            accountFrom.equals(accountTo)) {
            
            throw new ServerStateException(INVALID_ARGUMENT_ERROR);
        }
    }

    /*
     * Transfers the given amount between the given accounts.
     * 
     *  - The method is 'synchronized' to prevent memory consistency errors.
     */
    private synchronized void transferToExec(String accountFrom, String accountTo, int amount) throws ServerStateException {  
        debug("Executing the transferTo operation, from: " + accountFrom + 
              " to: " + accountTo + " with amount: " + amount + ".");

        verifyTransferTo(accountFrom, accountTo, amount);
        // Updates the server's value timestamp, as it will now execute an update.
        incrementTS();

        Integer balanceFrom = activeAccounts.get(accountFrom);
        Integer balanceTo = activeAccounts.get(accountTo);
        
        activeAccounts.replace(accountTo, balanceTo + amount);
        activeAccounts.replace(accountFrom, balanceFrom - amount);
    }

    /*
     * In case of an unsuccessful TransferOp state propagation,
     * undoes the previous action.
     *  - There's no need to do any checks on the arguments 
     */
    public synchronized void undoTransferTo(String accountFrom, String accountTo, int amount) {
        debug("Undoing the recently transfer of "+ amount +" from "+ accountFrom +" to "+ accountTo + ".");

        Integer balanceFrom = activeAccounts.get(accountFrom);
        Integer balanceTo = activeAccounts.get(accountTo);
            
        activeAccounts.replace(accountTo, balanceTo - amount);
        activeAccounts.replace(accountFrom, balanceFrom + amount);

        ledger.remove(ledger.size() - 1);
    }

    /**
     * Returns ledger state to propagate to the given replica,
     * based on the information stored in the timestamp table.
     */
    public synchronized LedgerState getLedgerState() {
        debug("Getting the server's ledger state.");

        List<DistLedgerCommonDefinitions.Operation> operations = new ArrayList<>();

        for (Operation op : ledger) {
            operations.add(op.toGrpc());
        }  

        return LedgerState.newBuilder()
                    .addAllLedger(operations)
                    .build();
    }

    /**
     * Returns the last operation in the server's ledger.
     */
    public synchronized DistLedgerCommonDefinitions.Operation getLastOperation() {
        debug("Getting the ledger's last operation, to be propagated to the remaining servers.");

        DistLedgerCommonDefinitions.Operation lastOp = null;
        
        if (ledger.size() > 0) {
            lastOp = ledger.get(ledger.size() - 1).toGrpc();
        }
        
        return lastOp;
    }

    /**
     * Puts the server on ACTIVE mode.
     */
    public synchronized void activate() {
        debug("Activating the server.");
        setState(state.ACTIVE);
    }

    /**
     * Puts the server on INACTIVE mode.
     */
    public synchronized void deactivate() {
        debug("Deactivating the server.");
        setState(state.INACTIVE);
    }

    /**
     * Returns ledger state to propagate to the given replica,
     * based on the information stored in the timestamp table.
     */
    public synchronized LedgerState getCorrectLedgerState(int replicaId) {
        debug("Getting the server's ledger state.");

        List<DistLedgerCommonDefinitions.Operation> operations = new ArrayList<>();

        for (Operation op : ledger) {
            if (tableTS.get(replicaId).size() == 0 ||
                compareTS(tableTS.get(replicaId), op.getPrevTS()) == true) {
                // If the sentTS <= op.prevTS, then the operation hasn't 
                // been propagated yet.
                operations.add(op.toGrpc());
            }
        }

        // Updates the server's tableTS, storing the most recent operation sent to the given replica.
        if (operations.size() > 0) {
            tableTS.set(replicaId, operations.get(operations.size() - 1).getTSList());
        }

        return LedgerState.newBuilder()
                    .addAllLedger(operations)
                    .build();
    }

    /**
     * Returns the most recent operation's prevTS, from the given list of operations.
     * (NOT USED).
     */
    private List<Integer> getMostRecentOpPrevTS(List<DistLedgerCommonDefinitions.Operation> operations) {
        List<Integer> mostRecentTS = new ArrayList<>();
        DistLedgerCommonDefinitions.Operation mostRecentOp = null;

        for (DistLedgerCommonDefinitions.Operation op : operations) {
            if (compareTS(op.getTSList(), mostRecentTS) == false ||
                mostRecentTS.size() == 0) {
                // If the operation's TS is > mostRecentTS, then it's the most recent operation.
                mostRecentTS = op.getTSList();
                mostRecentOp = op;
            }
        }

        return mostRecentOp.getPrevTSList();
    }

    /**
     * Propagates the given ledger state's operations to the server ledger.
     */
    public synchronized void propagateState(LedgerState ledgerStateGossip, List<Integer> replicaTSGossip, int replicaId) throws ServerStateException {
        debug("Propagating the given ledger state to the server.");

        List<Operation> operationsToExec = new ArrayList<>();

        // 1- Add new operations to ledger:
        for (DistLedgerCommonDefinitions.Operation op : ledgerStateGossip.getLedgerList()) {
            if (!opAlreadyInLedger(op.getTSList(), operationsToExec)) {
                // If the operation isn't in the ledger it hasn't been executed in this server, we add it to the ledger.
                propagateOp(op, operationsToExec);
            }
        }

        // 2- Update replica TS:
        merge(replicaTS, replicaTSGossip);

        // 3- Sort the gossip operations in order and execute them:
        Collections.sort(operationsToExec, new OperationComparator());
        for (Operation op : operationsToExec) {
            executeOp(op);
            updateValueTS(replicaTSGossip);
        }
        // Clears the operation list, so they aren't executed twice later.
        operationsToExec.clear();

        // 4- Add previously unstable operations to the list of operations to be executed:
        for (Operation op : getUnstableOperations()) {
            // Checks if any of the previously unstable operations can be executed, now
            // that the server's replicaTS has been updated.

            if (checkStableOp(op.getPrevTS()) && !operationsToExec.contains(op)) {
                // The operation is now stable.
                op.setStable(true);

                operationsToExec.add(op);
            }
        }

        // 5- Sort the previously unstable ledger operations in order and execute them:
        Collections.sort(operationsToExec, new OperationComparator());
        for (Operation op : operationsToExec) {
            executeOp(op);
            updateValueTS(replicaTSGossip);
        }
    }
    
    /**
     * Checks if the given operation is already in the ledger or not.
     * 
     *  - An operation won't be executed if its TS can be found in a stable ledger operation,
     *    or if its TS is <= than the server's replicaTS.
     */
    private synchronized boolean opAlreadyInLedger(List<Integer> opTS, List<Operation> operationsToExec) {
        debug("Checking if the given operation has already been / can be executed.");

        if (checkOpTS(opTS)) {
            // The gossip operation is already in the ledger, but now we
            // need to check if it's stable or not, to possibly add it to 
            // the list of operations to be executed.

            for (Operation op : ledger) {
                // Operation has been executed (is stable).
                if (op.getTS().equals(opTS) && op.isStable()) {
                    return true;
                }
    
                // Operation can be executed but hasn't (isn't stable)
                // and so, we add it to the list of operations to be executed.
                else if (op.getTS().equals(opTS) && !op.isStable()) {
    
                    if (!operationsToExec.contains(op)) {
                        // Mark operation as stable, as it will be executed.
                        op.setStable(true);
                        operationsToExec.add(op);
                    }
    
                    // We return true as it doesn't need to be added to the ledger.
                    return true;
                }
            }
        }

        return false;
    }

    /**
     * Checks if an operation has already been/can be executed, by
     * checking if the given opTS is <= than the server's replicaTS.
     * 
     * Returns true if it has/can (opTS <= replicaTS) and false otherwise (opTS > replicaTS).
     */
    private synchronized boolean checkOpTS(List<Integer> opTS) {
        for (int i = 0; i < MAX_REPLICAS; i++) {
            if (opTS.get(i) > replicaTS.get(i)) {
                return false;
            }
        }

        return true;
    }

    /**
     * Propagates the given operation to the server's ledger. All operations
     * added to the ledger are marked as stable, and added to a list of 
     * operations to be executed.
     *  - Throws a ServerStateException if an error occurs.
     */
    public synchronized void propagateOp(DistLedgerCommonDefinitions.Operation op, List<Operation> operationsToExec) {
        debug("Propagating the given operation:\n" + op.toString() + "\nto the server.");

        List<Integer> prevTS = op.getPrevTSList();
        List<Integer> TS = op.getTSList();

        Operation operation = null;
        
        if (op.getType() == DistLedgerCommonDefinitions.OperationType.OP_CREATE_ACCOUNT) {
            String userId = op.getUserId();

            // Add operation to ledger.
            operation = new CreateOp(userId);
            operation.setStable(true);
            operation.setPrevTS(prevTS);
            operation.setTS(TS);
            
            ledger.add(operation);
        }

        else if (op.getType() == DistLedgerCommonDefinitions.OperationType.OP_TRANSFER_TO) {
            String accountFrom = op.getUserId();
            String accountTo = op.getDestUserId();
            int amount = op.getAmount();

            // Add operation to ledger.
            operation = new TransferOp(accountFrom, accountTo, amount);
            operation.setStable(true);
            operation.setPrevTS(prevTS);
            operation.setTS(TS);
            ledger.add(operation);
        }
        
        // Add the operation to the list of operations to be executed later.
        if (operation != null) {
            operationsToExec.add(operation);
        }
    }

    /**
     * Executes the given operation.
     */
    private synchronized void executeOp(Operation op) throws ServerStateException {
        debug("Executing the given operation.");

        // Increments the replicaTS, as a new operation has been accepted and will
        // be executed.
        replicaTS.set(id, replicaTS.get(id) + 1);

        if (op.getType() == DistLedgerCommonDefinitions.OperationType.OP_CREATE_ACCOUNT) {
            CreateOp createOp = (CreateOp) op;

            createAccountExec(createOp.getAccount());
        }

        else if (op.getType() == DistLedgerCommonDefinitions.OperationType.OP_TRANSFER_TO) {
            TransferOp transferOp = (TransferOp) op;

            transferToExec(transferOp.getAccount(), transferOp.getDestAccount(), transferOp.getAmount());
        } 
          
    }

    /**
     * Merges the given TS_1 and TS_2, and stores the result in TS_1.
     */
    private synchronized void merge(List<Integer> TS_1, List<Integer> TS_2) {
        debug("Merging TS_1: " + TS_1 + " and TS_2: " + TS_2 + ".");
        
        for (int i = 0; i < MAX_REPLICAS; i++) {
            TS_1.set(i, Math.max(TS_1.get(i), TS_2.get(i)));
        }
    }

    /**
     * Compares the given TS_1 and TS_2, and returns true if TS_1 <= TS_2.
     */
    private synchronized boolean compareTS(List<Integer> TS_1, List<Integer> TS_2) {
        for (int i = 0; i < MAX_REPLICAS; i++) {
            if (TS_1.get(i) > TS_2.get(i)) {
                return false;
            }
        }

        return true;
    }

    /**
     * Checks if an operation is stable or not, based on the
     * given prevTS and the server's value TS.
     * - An operation is stable if the prevTS <= valueTS.
     */
    private synchronized boolean checkStableOp(List<Integer> prevTS) {
        debug("Checking if the current operation is stable.");
        
        for (int i = 0; i < MAX_REPLICAS; i++) {
            if (prevTS.get(i) > valueTS.get(i)) {
                return false;
            }
        }

        return true;
    }

    /**
     * Verifies if the latest version of the requested value in the current replica
     * can be sent to the client, based on the replica and client TSs.
     * If not, the replica passively waits until it has a version that can be sent.
     */
    private void checkValueTS(List<Integer> prevTS) throws ServerStateException, InterruptedException {
        debug("Verifying if the server has enough updates to respond to the client.");

        if (checkValueTSAux(prevTS) == false) {
            throw new ServerStateException(UNSTABLE_OP_ERROR);
        }

        return;

        /* 
         * wait() implementation (not working), always throws
         * an 'IllegalMonitorStateException: current thread is not owner'
         * exception.
         * 
        ServerStateProcess waitListOp = null;

        if (checkValueTSAux(prevTS) == false) {
            // Adds the current operation to the wait list.
            waitListOp = new ServerStateProcess(prevTS, true);
            waitList.add(waitListOp);
            waitListOp.processWait();
        }

        return;
        */
    }

    private synchronized boolean checkValueTSAux(List<Integer> prevTS) {
        for (int i = 0; i < MAX_REPLICAS; i++) {
            if (prevTS.get(i) > valueTS.get(i)) {
                return false;
            }
        }

        return true;
    }

    /**
     * Checks if any operation in the wait list can be executed
     * and if so, notify it.
     */
    private void notifyOps() {
        debug("Notifying any operations that can be executed.");

        for (ServerStateProcess op : waitList) {
            if (checkValueTSAux(op.getPrevTS())) {
                op.processNotify();
                waitList.remove(op);
            }
        }
    }

    /**
     * Updates the value TS with the given replica TS.
     */
    private void updateValueTS(List<Integer> replicaTS) {
        debug("Updating the server's value TS.");

        for (int i = 0; i < MAX_REPLICAS; i++) {
            if (replicaTS.get(i) > valueTS.get(i)) {
                valueTS.set(i, replicaTS.get(i));
            }
        }

        debug("New valueTS: " + valueTS + ".");
    }

    /**
     * Returns the unstable operations from the server's ledger.
     */
    private List<Operation> getUnstableOperations() {
        debug("Getting the unstable operations from the server's ledger.");

        List<Operation> unstableOps = new ArrayList<>();

        for (Operation op : ledger) {
            if (!op.isStable()) {
                unstableOps.add(op);
            }
        }

        return unstableOps;
    }

}
