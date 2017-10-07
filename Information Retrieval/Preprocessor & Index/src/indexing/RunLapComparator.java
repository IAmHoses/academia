package indexing;

import java.util.Comparator;

public class RunLapComparator implements Comparator<Run>{
    @Override
    public int compare(Run x, Run y)
    {
        // Assume neither string is null. Real code should
        // probably be more robust
        // You could also just return x.length() - y.length(),
        // which would be more efficient.
        if (x.lapNumber() < y.lapNumber())
        {
            return -1;
        }
        if (x.lapNumber() > y.lapNumber())
        {
            return 1;
        }
        return 0;
    }
}
