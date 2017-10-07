package containers;

public class Triple {
	private String term;
	private String number;
	private int frequency;
	private double n_idf;
	
	public Triple(String token, String identifier, int freq) {
		term = token;
		number = identifier;
		frequency = freq;
	}
	
	public String term() {
		return term;
	}
	
	public String number() {
		return number;
	}
	
	public int tf() {
		return frequency;
	}
	
	public void increment_tf() {
		frequency++;
	}
	
	public void set_n_idf(double computed_n_idf) {
		n_idf = computed_n_idf;
	}
	
	public double n_idf() {
		return n_idf;
	}
}