package pt.tecnico.distledger.server.domain;

import java.util.List;

public class ServerStateProcess {
    
    private List<Integer> prevTS;
    boolean isWaiting;

    public ServerStateProcess(List<Integer> prevTS, boolean isWaiting) {
        this.prevTS = prevTS;
        this.isWaiting = isWaiting;	
    }

    public List<Integer> getPrevTS() {
        return prevTS;
    }

    public void setPrevTS(List<Integer> prevTS) {
        this.prevTS = prevTS;
    }

    public boolean getIsWaiting() {
        return isWaiting;
    }

    public void setIsWaiting(boolean isWaiting) {
        this.isWaiting = isWaiting;
    }

    /**
     * Calls wait on the current thread.
     */
    public void processWait() {
        while(isWaiting) {
            try { 
                this.wait();
            }
            catch (InterruptedException e) {
                Thread.currentThread().interrupt(); 
                System.err.println("Thread Interrupted");
            }
        }
    }

    /**
     * Calls notify on the current thread.
     */
    public void processNotify() {
        setIsWaiting(false);
        this.notify();
    }
}
