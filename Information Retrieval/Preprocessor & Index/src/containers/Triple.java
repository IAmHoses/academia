package containers;

public class Triple {
	private String term;
	private String doc;
	private int frequency;
	
	public Triple(String token, String DOCNO, int initialFrequency) {
		term = token;
		doc = DOCNO;
		frequency = initialFrequency;
	}
	
	public String term() {
		return term;
	}
	
	public String doc() {
		return doc;
	}
	
	public int tf() {
		return frequency;
	}
	
	public void increment_tf() {
		frequency++;
	}
}