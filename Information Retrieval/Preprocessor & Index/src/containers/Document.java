package containers;

import java.util.Hashtable;

public class Document {
	private String DOCNO;
	private String TEXT;
	private Hashtable<String, Triple> specialTokens;
	
	public Document(String number, String text) {
		DOCNO = number;
		TEXT = text;
		specialTokens = new Hashtable<String, Triple>();
	}
	
	public String number() {
		return DOCNO;
	}
	
	public String text() {
		return TEXT;
	}
	
	public Hashtable<String, Triple> specialTokens() {
		return specialTokens;
	}
	
	public void update(String newText) {
		TEXT = newText;
	}
	
	public void saveToken(String specialToken) {
		specialTokens.put(specialToken, new Triple(specialToken, DOCNO, 1));
	}
	
	public void incrementTokenFreq(String specialToken) {
		Triple tokenTriple = specialTokens.get(specialToken);
		tokenTriple.increment_tf();
		specialTokens.replace(specialToken, tokenTriple);
	}
}
