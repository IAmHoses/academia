package scheduling;

import java.util.LinkedList;

public class Process {
	private Integer processNumber;
	private Integer startTime;
	private Integer endTime;
	private LinkedList<Subprocess> subprocesses;
	private Boolean started;
	private Boolean idle;
	private Boolean finished;
	
	public Process (Integer processNumber, Integer startTime) {
		this.processNumber = processNumber;
		this.startTime = startTime;
		endTime = startTime;
		subprocesses = new LinkedList<Subprocess>();
		started = false;
		idle = false;
		finished = false;
	}
	
	public Integer getProcessNumber () {
		return processNumber;
	}
	
	public Integer getStartTime () {
		return startTime;
	}
	
	public LinkedList<Subprocess> getSubprocesses () {
		return subprocesses;
	}
	
	public Boolean hasStarted () {
		return started;
	}
	
	public Boolean isIdle () {
		return idle;
	}
	
	public Boolean isFinished () {
		return finished;
	}
	
	public void start () {
		started = true;
	}
	
	public void sleep () {
		idle = true;
	}
	
	public void awaken () {
		idle = false;
	}
	
	public void addSubprocess (Subprocess subprocess) {
		subprocesses.add(subprocess);
	}
	
	public Integer subtractTime (Integer totalTime, Integer difference) {
		startTime = totalTime;
		endTime = totalTime;
		Integer timeRemaining = difference;
		
		while (timeRemaining > 0) {
			if (subprocesses.getFirst().getTime() > timeRemaining) {
				subprocesses.getFirst().subtractTime(timeRemaining);
				startTime = endTime;
				endTime += timeRemaining;
				timeRemaining = 0;
				print();
			}
			else {
				startTime = endTime;
				endTime += subprocesses.getFirst().getTime();
				timeRemaining -= subprocesses.getFirst().getTime();
				
				print();
				
				subprocesses.removeFirst();
				
				if (subprocesses.isEmpty()) {
					finished = true;
					return endTime;
				}
				
				if (subprocesses.getFirst().getType().equals("B")) {
					awaken();
				}
				
				if (subprocesses.getFirst().getType().equals("I")) {
					sleep();
				}
			}
		}
		return endTime;
	}
	
	public void print () {
		if (isIdle()) {
			System.out.println(processNumber + " (Idle) " + startTime + " " + endTime);
		}
		else {
			System.out.println(processNumber + " " + startTime + " " + endTime);
		}
	}
}
