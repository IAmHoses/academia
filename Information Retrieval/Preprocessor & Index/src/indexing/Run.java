package indexing;

import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import containers.PositionalTriple;
import containers.Triple;

/*
 *  correct adding duplicate triples:
 *  	HashMap FOR STORAGE UNTIL MEMORY CAP --> HashMap.SIZE() == CAPACITY
 *  	USE SAME TECHNIQUE AS SPECIAL TOKENS :D
 *  
 *  when writing:
 *  	PUT ALL HashMap KEYS IN ARRAY
 *  	SORT ARRAY BY STRING
 *  	WRITE THE KEY,VALUE PAIRS BY ITERATING OVER SORTED ARRAY
 */

public class Run {
	private String config;
	private int lapNumber;
	private int memory;
	private HashMap<String, HashMap<String, Triple>> termSetsByDoc;
	private String[] sortedKeys;
	private File tempFile;
	private String tempFilePath;
	
	private HashMap<String, HashMap<String, PositionalTriple>> positionalTermSetsByDoc;
	
	public Run(String configuration, int lap, int memoryConstraint) {
		config = configuration;
		lapNumber = lap;
		memory = memoryConstraint;
		sortedKeys = new String[memoryConstraint];
		
		if (config == "POSITIONAL") {
			positionalTermSetsByDoc = new HashMap<String, HashMap<String, PositionalTriple>>();
		} else {
			termSetsByDoc = new HashMap<String, HashMap<String, Triple>>();
		}
	}
	
	public int lapNumber() {
		return lapNumber;
	}
	
	public int memory() {
		return memory;
	}
	
	public int consumed() {
		if (config == "POSITIONAL") {
			int numOfTriples = 0;
			for (String doc : positionalTermSetsByDoc.keySet()) {
				numOfTriples += positionalTermSetsByDoc.get(doc).size();
			}
			return numOfTriples;
		} else {
			int numOfTriples = 0;
			for (String doc : termSetsByDoc.keySet()) {
				numOfTriples += termSetsByDoc.get(doc).size();
			}
			return numOfTriples;	
		}
	}
	
	public boolean isFull() {
		return consumed() == memory();
	}
	
	public File tempFile() {
		return tempFile;
	}
	
	public String filePath() {
		return tempFilePath;
	}
	
	public void newDocTermSet(String docNo) {
		if (config == "POSITIONAL") {
			positionalTermSetsByDoc.put(docNo, new HashMap<String, PositionalTriple>());
		} else {
			termSetsByDoc.put(docNo, new HashMap<String, Triple>());	
		}
	}
	
	public void transferSpecialToken(Triple triple) {
		if (config == "POSITIONAL") {
			HashMap<String, PositionalTriple> docTermSet = positionalTermSetsByDoc.get(triple.doc());
			PositionalTriple castedTriple = new PositionalTriple(triple.term(), triple.doc(), 0);
			docTermSet.put(castedTriple.term(), castedTriple);
			positionalTermSetsByDoc.replace(castedTriple.doc(), docTermSet);	
		} else {
			HashMap<String, Triple> docTermSet = termSetsByDoc.get(triple.doc());
			docTermSet.put(triple.term(), triple);
			termSetsByDoc.replace(triple.doc(), docTermSet);	
		}
	}
	
	public void store(String docNo, String token) {
		HashMap<String, Triple> docTermSet = termSetsByDoc.get(docNo);
		
		if (docTermSet.containsKey(token)) {
			Triple tokenTriple = docTermSet.get(token);
			tokenTriple.increment_tf();
			docTermSet.replace(token, tokenTriple);
			termSetsByDoc.replace(docNo, docTermSet);
		} else {
			docTermSet.put(token, new Triple(token, docNo, 1));
			termSetsByDoc.replace(docNo, docTermSet);
		}
	}
	
	public void storePositionalToken(String docNo, String token, int position) {
		HashMap<String, PositionalTriple> docTermSet = positionalTermSetsByDoc.get(docNo);
		
		if (docTermSet.containsKey(token)) {
			PositionalTriple tokenTriple = docTermSet.get(token);
			tokenTriple.addPosition(position);
			docTermSet.replace(token, tokenTriple);
			positionalTermSetsByDoc.replace(docNo, docTermSet);
		} else {
			docTermSet.put(token, new PositionalTriple(token, docNo, position));
			positionalTermSetsByDoc.replace(docNo, docTermSet);
		}
	}
	
	public void storeTriple(String docNo, Triple existingTriple) {
		HashMap<String, Triple> docTermSet = termSetsByDoc.get(docNo);
		docTermSet.put(docNo, existingTriple);
	}
	
	public void sort() {
		Set<String> uniqueTerms = new HashSet<String>();
		
		if (config == "POSITIONAL") {
			for (String doc : positionalTermSetsByDoc.keySet()) {
				Set<String> elementsToAdd = new HashSet<String>();
				for (String element : positionalTermSetsByDoc.get(doc).keySet()) {
					elementsToAdd.add(element);
				}
				uniqueTerms.addAll(elementsToAdd);
			}
		} else {
			for (String doc : termSetsByDoc.keySet()) {
				Set<String> elementsToAdd = new HashSet<String>();
				for (String element : termSetsByDoc.get(doc).keySet()) {
					elementsToAdd.add(element);
				}
				uniqueTerms.addAll(elementsToAdd);
			}
		}

		int i = 0;
		for (String term : uniqueTerms) {
			sortedKeys[i] = term;
			i++;
		}
	    List<String> list = new ArrayList<String>(Arrays.asList(sortedKeys));
	    list.removeAll(Collections.singleton(null));
	    sortedKeys = list.toArray(new String[list.size()]);
	    
		Arrays.sort(sortedKeys);
	}
	
	public void write() {
		sort();
		try {
			tempFile = File.createTempFile("run" + lapNumber, null, new File("temp"));
			tempFile.deleteOnExit();
			String[] splitAbsolutePath = tempFile.getAbsolutePath().split("temp");
			tempFilePath = "tempFile" + splitAbsolutePath[1];

    	    BufferedWriter bw = Files.newBufferedWriter(tempFile.toPath(), StandardOpenOption.WRITE);
    	    
    	    if (config == "POSITIONAL") {
    			for (String key : sortedKeys) {
    				for (String doc : positionalTermSetsByDoc.keySet()) {
    					HashMap<String, PositionalTriple> docTermSet = positionalTermSetsByDoc.get(doc);
    					
    					if (docTermSet.containsKey(key)) {
    						bw.write(docTermSet.get(key).term() + " " + docTermSet.get(key).doc() + " " + 
    					docTermSet.get(key).tf() + " " + docTermSet.get(key).printPositions() + '\n');
    					}
    				}
    			}
    	    } else {
    			for (String key : sortedKeys) {
    				for (String doc : termSetsByDoc.keySet()) {
    					HashMap<String, Triple> docTermSet = termSetsByDoc.get(doc);
    					
    					if (docTermSet.containsKey(key)) {
    						bw.write(docTermSet.get(key).term() + " " + docTermSet.get(key).doc() + " " + 
    					docTermSet.get(key).tf() + '\n');
    					}
    				}
    			}
    	    }
			bw.close();
//			System.out.println("Run " + lapNumber + " written at " + tempFilePath);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
