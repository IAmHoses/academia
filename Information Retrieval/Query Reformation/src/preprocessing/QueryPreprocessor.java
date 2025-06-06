package preprocessing;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import containers.Query;
import containers.Triple;
import tools.Loader;

public class QueryPreprocessor {
	private String testKey = "265";
	private HashMap<String, Query> queries;
	
	public QueryPreprocessor(String queryPath, Boolean longQueries) {
		if (longQueries == false) {
			queries = new Loader().loadShortQueries(queryPath);
		}
		if (longQueries == true) {
			queries = new Loader().loadLongQueries(queryPath);
		}
	}
	
	public HashMap<String, Query> queries() {
		return queries;
	}
	
	public HashMap<String, Query> preprocess() {
		QueryPrepper queryPrepper = new QueryPrepper();
		QueryNormalizer queryNormalizer = new QueryNormalizer();
		
		for (String key : queries.keySet()) {
			Query query = queries.get(key);
			
			query = queryPrepper.prepare(query);
			query = queryNormalizer.normalize(query);
			
			queries.replace(key, query);
		}
		
		extractTokens();
			
		return queries;
	}
	
	public void extractTokens() {
		
		HashMap<String, String> stopWords = new Loader().buildDictionary("Resources/stopwords.txt", 1);
		
		PorterStemmer stemmer = new PorterStemmer();
		
		for (String key: queries.keySet()) {
			Query query = queries.get(key);
			
			String[] tokens = query.string().split("\\s+");
			
			for (String token: tokens) {
				String stemmedToken = stemmer.stripAffixes(token);
				
				if (stopWords.containsKey(stemmedToken)) {
					continue;
				} else {
					if (query.tokens().containsKey(stemmedToken)) {
						Triple tokenTriple = query.tokens().get(stemmedToken);
						tokenTriple.increment_tf();
						query.tokens().replace(stemmedToken, tokenTriple);
					} else {
						query.tokens().put(stemmedToken, new Triple(stemmedToken, query.number(), 1));
					}
				}
			}
			queries.replace(query.number(), query);
		}
	}
}
