package preprocessing;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;

import containers.Score;
import containers.Triple;
import tools.Loader;

public class PhraseBuilder {
	private HashMap<String, String> stopWords; 
	
	public PhraseBuilder () {
		stopWords = new Loader().buildDictionary("resources/stopwords.txt", 1);
	}
	
	public LinkedList<Triple> identifyPhrases(String[] tokens, String queryNumber, int twoGramThreshold, int threeGramThreshold) {
		LinkedList<Triple> phrases = new LinkedList<Triple>();
		
		phrases.addAll(identifyTwoGrams(tokens, queryNumber, twoGramThreshold));
		phrases.addAll(identifyThreeGrams(tokens, queryNumber, threeGramThreshold));
		
		return phrases;
	}
	
	public LinkedList<Triple> identifyTwoGrams(String[] tokens, String queryNumber, int threshold) {
		String[] zipper1 = new String[tokens.length + 1];
		String[] zipper2 = new String[tokens.length + 1];
		
		for (int i = 0; i < tokens.length; i++) {
			zipper1[i + 1] = tokens[i];
			zipper2[i] = tokens[i];
		}

		String[] raw_twoGrams = new String[tokens.length + 1];
		
		for (int j = 0; j < raw_twoGrams.length; j++) {
			if (zipper1[j] == null || zipper2[j] == null) {
				continue;
			}
			raw_twoGrams[j] = zipper1[j] + "_" + zipper2[j];
		}
		
	    List<String> list = new ArrayList<String>(Arrays.asList(raw_twoGrams));
	    list.removeAll(Collections.singleton(null));
	    raw_twoGrams = list.toArray(new String[list.size()]);
		
		HashMap<String, Integer> frequencies = new HashMap<String, Integer>();
		
		for (String twoGram : raw_twoGrams) {
			String[] splitGram = twoGram.trim().split("\\_");
			
			if (twoGram == null || splitGram.length < 2) {
				continue;
			}
			if (stopWords.containsKey(splitGram[0]) || stopWords.containsKey(splitGram[1])) {
				continue;
			}
			
			if (frequencies.containsKey(twoGram)) {
				frequencies.replace(twoGram, frequencies.get(twoGram) + 1);
			} else {
				frequencies.put(twoGram, 1);
			}
		}
		
		LinkedList<Triple> twoGrams = new LinkedList<Triple>();
		
		for (String key : frequencies.keySet()) {
			if (frequencies.get(key) >= threshold) {
				twoGrams.add(new Triple(key, queryNumber, frequencies.get(key)));
			}
		}
		return twoGrams;
	}
	
	public LinkedList<Triple> identifyThreeGrams(String[] tokens, String queryNumber, int threshold) {
		String[] zipper1 = new String[tokens.length + 2];
		String[] zipper2 = new String[tokens.length + 2];
		String[] zipper3 = new String[tokens.length + 2];
		
		for (int i = 0; i < tokens.length; i++) {
			zipper1[i + 2] = tokens[i];
			zipper2[i + 1] = tokens[i];
			zipper3[i] = tokens[i];
		}

		String[] raw_threeGrams = new String[tokens.length + 1];
		
		for (int j = 0; j < raw_threeGrams.length; j++) {
			if (zipper1[j] == null || zipper2[j] == null || zipper3[j] == null) {
				continue;
			}
			raw_threeGrams[j] = zipper1[j] + "_" + zipper2[j] + "_" + zipper3[j];
		}
		
	    List<String> list = new ArrayList<String>(Arrays.asList(raw_threeGrams));
	    list.removeAll(Collections.singleton(null));
	    raw_threeGrams = list.toArray(new String[list.size()]);
		
		HashMap<String, Integer> frequencies = new HashMap<String, Integer>();
		
		for (String threeGram : raw_threeGrams) {
			String[] splitGram = threeGram.trim().split("\\_");
			
			if (threeGram == null || splitGram.length < 3) {
				continue;
			}
			if (stopWords.containsKey(splitGram[0]) || stopWords.containsKey(splitGram[1]) || stopWords.containsKey(splitGram[2])) {
				continue;
			}
			
			if (frequencies.containsKey(threeGram)) {
				frequencies.replace(threeGram, frequencies.get(threeGram) + 1);
			} else {
				frequencies.put(threeGram, 1);
			}
		}
		
		LinkedList<Triple> threeGrams = new LinkedList<Triple>();
		
		for (String key : frequencies.keySet()) {
			if (frequencies.get(key) >= threshold) {
				threeGrams.add(new Triple(key, queryNumber, frequencies.get(key)));
			}
		}
		return threeGrams;
	}
}
