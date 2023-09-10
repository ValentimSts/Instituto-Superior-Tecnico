package pt.tecnico.distledger.server.domain;

import pt.tecnico.distledger.server.domain.operation.Operation;

import java.util.Comparator;
import java.util.List;

public class OperationComparator implements Comparator<Operation> {

    @Override
    public int compare(Operation op1, Operation op2) {
        return getTSsum(op1.getTS()).compareTo(getTSsum(op2.getTS()));
    }

    private Integer getTSsum(List<Integer> TS) {
        Integer sum = 0;
        for (int i = 0; i < TS.size(); i++) {
            sum += TS.get(i);
        }

        return sum;
    }
}
