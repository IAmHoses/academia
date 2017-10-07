package preprocessing;

import java.util.Hashtable;
import containers.Document;

public class Preprocessor {
	DocumentPrepper prepper;
	DocumentNormalizer normalizer;
	
	public Preprocessor() {
		prepper = new DocumentPrepper();
		normalizer = new DocumentNormalizer();
	}
	
	public Document preprocess(String rawDoc) {
		Document preprocessedDoc;
		
		preprocessedDoc = prepper.prepare(rawDoc);
		normalizer.normalize(preprocessedDoc);
		
//		for (String key : preparedDocs.keySet()) {
//			System.out.println(preparedDocs.get(key).text());
//		}
		
//		for (String key : preparedDocs.keySet()) {
//			for (String token : preparedDocs.get(key).specialTokens().keySet()) {
//				System.out.println(preparedDocs.get(key).specialTokens().get(token).term());
//			}
//		}
		
		return preprocessedDoc;
	}

}
