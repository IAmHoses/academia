package containers;

import java.util.HashMap;

public class PositionalPostingList {
	private HashMap<String, PositionalTriple> map;	// K = DOCNO, V = PoTriple
	private double idf;
	
	public PositionalPostingList() {
		map = new HashMap<String, PositionalTriple>();
	}
	
	public HashMap<String, PositionalTriple> map() {
		return map;
	}
	
	public double idf() {
		return idf;
	}
	
	public void storeTriple(String DOCNO, PositionalTriple triple) {
		map.put(DOCNO, triple);
	}
	
	public void compute_vsm_idf(int n) {
		idf = Math.log10(n / map.size());
	}
	
	public void compute_probabilistic_model_idf(int n) {
		idf = Math.log10((n - map.size() + 0.5) / (map.size() + 0.5));
	}
}
