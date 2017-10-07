package containers;

public class Score {
	private double score;
	private String queryNumber;
	private String documentNumber;
	
	public Score(double computedScore, String qNum, String docNum) {
		score = computedScore;
		queryNumber = qNum;
		documentNumber = docNum;
	}

	public double score() {
		return score;
	}
	
	public String queryNumber() {
		return queryNumber;
	}
	
	public String documentNumber() {
		return documentNumber;
	}
}
