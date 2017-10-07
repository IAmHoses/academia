package indexing;

import java.util.Comparator;

public class ExtractedTripleComparator implements Comparator<String>{
    @Override
    public int compare(String x, String y)
    {
        // Assume neither string is null. Real code should
        // probably be more robust
        // You could also just return x.length() - y.length(),
        // which would be more efficient.
        if (x.split(" ")[0].compareTo(y.split(" ")[0]) < 0)
        {
            return -1;
        }
        if (x.split(" ")[0].compareTo(y.split(" ")[0]) > 0)
        {
            return 1;
        }
        return 0;
    }
}
