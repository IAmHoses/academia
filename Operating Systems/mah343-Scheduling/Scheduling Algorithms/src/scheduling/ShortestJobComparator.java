package scheduling;

import java.util.Comparator;

public class ShortestJobComparator implements Comparator<Process> {
	@Override
	public int compare(Process p1, Process p2) {
		
		if (p1.hasStarted() && !p2.hasStarted()) {
			return -1;
		}
		
		else if (!p1.hasStarted() && p2.hasStarted()) {
			return 1;
		}
		
		else if (p1.hasStarted() && p2.hasStarted()) {
			Subprocess p1Head = p1.getSubprocesses().getFirst();
			Subprocess p2Head = p2.getSubprocesses().getFirst();
			
			if (p1Head.getType().equals("B") && p2Head.getType().equals("B") ||
					(p1Head.getType().equals("I") && p2Head.getType().equals("I"))) {
				
				if (p1Head.getTime() < p2Head.getTime()) {
					return -1;
				}
				else if (p1Head.getTime() > p2Head.getTime()) {
					return 1;
				}
				else {
					return 0;	
				}
			}
			
			if (p1Head.getType().equals("B") && p2Head.getType().equals("I")) {
				return -1;
			}
			
			if (p1Head.getType().equals("I") && p2Head.getType().equals("B")) {
				return 1;
			}	
		}
		
		else {
			return 0;
		}
		
		return 0;
	}
}
