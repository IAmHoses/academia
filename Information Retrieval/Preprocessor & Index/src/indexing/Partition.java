package indexing;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;

public class Partition {
	private File inputTempFile;
	private BufferedReader reader;
	private Boolean EoF_Reached;
	private String candidate;
	private Boolean candidateAccepted;
	
	public Partition(File file) {
		inputTempFile = file;
		
		Charset charset = Charset.forName("US-ASCII");
		try {
			reader = Files.newBufferedReader(inputTempFile.toPath(), charset);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		EoF_Reached = false;
		candidateAccepted = false;
	}
	
	public Boolean EoF_Reached() {
		return EoF_Reached;
	}
	
	public String candidate() {
		return candidate;
	}
	
	public String offerCandidate() throws IOException {
		if (EoF_Reached()) {
			return null;
		}
		
		String extractedTriple = null;
		extractedTriple = reader.readLine();
		
		if (extractedTriple != null) {
			candidate = extractedTriple;
		} else {	// reached EoF!
			EoF_Reached = true;
			reader.close();
//			inputTempFile.delete();
//			System.out.println(inputTempFile.getAbsolutePath() + " deleted");
			return null;
		}
		candidateAccepted = false;
		return extractedTriple;
	}
	
	public void acceptCandidate() {
		candidateAccepted = true;
	}
	
	public Boolean candidateAccepted() {
		return candidateAccepted;
	}
}
