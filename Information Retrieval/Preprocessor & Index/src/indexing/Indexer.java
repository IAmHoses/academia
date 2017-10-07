package indexing;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.PriorityQueue;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import containers.Document;
import containers.Triple;
import exceptions.EndOfFileException;
import exceptions.MemoryConstraintException;
import preprocessing.PhraseBuilder;
import preprocessing.PorterStemmer;
import preprocessing.Preprocessor;
import tools.Loader;

public class Indexer {
	private String configuration;
	private int memoryConstraint;
	
	private String[] files;
	private long offset;
	private int lapNumber;
	private PriorityQueue<Run> runs;
	
	private Run runCache;
	private Document documentCache;
	private LinkedList<Triple> phraseCache;
	private int phrasesToRemove;
	private LinkedList<String> specialTokensToDelete;
	private int usedMemory;
	Boolean runWritten;

	private Preprocessor preprocessor;
	private HashMap<String, String> stopWords;
	
	private LinkedList<Partition> partitions;
	private PriorityQueue<String> mergeCandidates;
	
	public Indexer(String config, int memory) {
		configuration = config;
		memoryConstraint = memory;
		
		files = new String[11];
		files = new Loader().populateArray(files, "files/input.txt");
		Comparator<Run> comparator = new RunLapComparator();
		runs = new PriorityQueue<Run>(comparator);
		specialTokensToDelete = new LinkedList<String>();
		preprocessor = new Preprocessor();
	}
	
	public void build() {
		generateRuns();
		try {
			mergeRuns();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (MemoryConstraintException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void generateRuns() {
		for (int i = 0; i < files.length; i++) {
			try {
				RandomAccessFile inputFile = new RandomAccessFile(files[i], "r");
				System.out.println("File " + (i + 1) + " read from input files");
				
				while (offset < inputFile.length()) {
					Run newRun = generateSingleRun(inputFile, lapNumber);
					/*
					 *  If you had to cache a run, then the run isn't full and the lap isn't completed.
					 *  Just chill and let generateRun do its thing until it's full (continue while loop)
					 */
					if (newRun.isFull()) {
						runs.add(newRun);
						lapNumber++;
					}
				}
				// reset offset for next file
				offset = 0;
				inputFile.close();
			} catch (FileNotFoundException e) {
				//TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		// Reached end of input files, write up that last bit and add it to priority queue
		if (runCache != null) {
			runCache.write();
			runs.add(runCache);
			runCache = null;
		}
		System.out.println(runs.size() + " runs written and added to heap");
	}
	
	// add usedMemory > memory exception
	
	public Run generateSingleRun(RandomAccessFile file, int lap) throws IOException {
		Run currentRun;
		
		if (runCache != null) {
			currentRun = runCache;
//			System.out.println("Pulled run from cache!");
			runCache = null;
		} else {
			currentRun = new Run(configuration, lap, memoryConstraint);
//			System.out.println("New run instantiated!");
		}
		
		runWritten = false;
		while (runWritten == false) {
			Document currentDoc;
			
			// if document was cached, retrieve it and reset the cache
			if (documentCache != null) {
				currentDoc = documentCache;
				documentCache = null;
				currentRun.newDocTermSet(currentDoc.number());
			} else {
				// completed parsing last document, on to the next one (if not at EOF)
				
				if (offset == file.length()) {	// attempting to read from EOF
					runCache = currentRun;
					return currentRun;
				} else {
					String rawDoc = null;
					try {
						rawDoc = extractDocument(file);
					} catch (EndOfFileException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}					
					currentDoc = preprocessor.preprocess(rawDoc);
					currentRun.newDocTermSet(currentDoc.number());
				}
			}
			
			if (!specialTokensToDelete.isEmpty()) {
				for (String token : specialTokensToDelete) {
					currentDoc.specialTokens().remove(token);
				}
				specialTokensToDelete.clear();
			}
			
			if (!currentDoc.specialTokens().isEmpty() && configuration != "PHRASE") {
				for (String key : currentDoc.specialTokens().keySet()) {
//					if (usedMemory > memory) {
//						System.err.println("ERROR: MEMORY OVERFLOW");
//						// exception handling?
//					}
					// if interuped mid-doc, cache it before writing run and resetting memory
					if (usedMemory == memoryConstraint) {
						documentCache = currentDoc;
						currentRun.write();
						runWritten = true;
						resetMemory();
						break;
					}
					currentRun.transferSpecialToken(currentDoc.specialTokens().get(key));
					usedMemory = currentRun.consumed();
					specialTokensToDelete.add(key);
				}
				specialTokensToDelete.clear();	// made it through transfer without interruption, reset for next document
			}
			if (runWritten == true) {
//				System.out.println("runWritten flag tripped after token transfer, continung to next iteration in while loop");
				continue;
			}
			
			switch (configuration) {
			case "SINGLE_TERM": parseSingleTerms(currentDoc, currentRun);
			break;
			case "POSITIONAL": parsePositionals(currentDoc, currentRun);
			break;
			case "PHRASE": parsePhrases(currentDoc, currentRun);
			break;
			case "STEM": parseStems(currentDoc, currentRun);
			break;
			
			default: System.out.println("Invalid configuration for index builder");
			}
		}
//		System.out.println("Exiting from generateRun()...");
		return currentRun;
	}
	
	public String extractDocument(RandomAccessFile file) throws EndOfFileException {		
		try {
			if (offset == file.length()) {
				throw new EndOfFileException("ERROR in extractDocument(): attempt to read from EOF");
			}
			file.seek(offset);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
		
		String rawDocument = "";
		String line = null;
		
		try {
			while ((line = file.readLine()) != null) {
				if (line.equals("") || line.equals("\n") || line.equals("<DOC>")) {
					continue;
				} else if (line.equals("</DOC>")) {
					file.readLine();	// progress to next line
					offset = file.getFilePointer();	// save position
//					System.out.println("New offset: " + offset + " | Length of file: " + file.length());
					break;
				} else {
					rawDocument = rawDocument.concat(line + "\n");
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return rawDocument;
	}
	
	public void resetMemory() {
		usedMemory = 0;
	}
	
	public void parseSingleTerms(Document doc, Run run) {
		stopWords = new Loader().buildDictionary("files/stopwords.txt", 1);
		
		String[] tokens = doc.text().split("\\s+");
		
		for (String token : tokens) {
			if (usedMemory == memoryConstraint) {
				/*
				 * All keys to be deleted were removed under the assumption that we wouldn't be interrupted
				 * during text parsing. However, that is now not the case and we must make sure to delete 
				 * all special tokens after we retrieve the cached document. This way they will not be 
				 * double-counted
				 */
				
				for (String specialToken : doc.specialTokens().keySet()) {
					specialTokensToDelete.add(specialToken);
				}
				documentCache = doc;
				run.write();
				runWritten = true;
				resetMemory();
				break;
			}
			if (token.length() == 0) {
				continue;
			}
			if (stopWords.containsKey(token)) {
				doc.update(doc.text().replaceFirst(token, ""));
				continue;
			}
			run.store(doc.number(), token);
			usedMemory = run.consumed();
			doc.update(doc.text().replaceFirst(token, ""));
		}
	}
	
	public void parsePositionals(Document doc, Run run) {
		String[] tokens = doc.text().split("\\s+");
		
		for (int i = 0; i < tokens.length; i++) {
			if (usedMemory == memoryConstraint) {
				/*
				 * All keys to be deleted were removed under the assumption that we wouldn't be interrupted
				 * during text parsing. However, that is now not the case and we must make sure to delete 
				 * all special tokens after we retrieve the cached document. This way they will not be 
				 * double-counted
				 */
				for (String specialToken : doc.specialTokens().keySet()) {
					specialTokensToDelete.add(specialToken);
				}
				documentCache = doc;
				run.write();
				runWritten = true;
				resetMemory();
				break;
			}
			String token = tokens[i];
			if (token.length() == 0) {
				continue;
			}
			run.storePositionalToken(doc.number(), token, i + 1);;
			usedMemory = run.consumed();
			doc.update(doc.text().replaceFirst(token, ""));
		}
	}
	
	public void parsePhrases(Document doc, Run run) {
		LinkedList<Triple> phrases;
		
		if (phraseCache != null) {
			phrases = phraseCache;
			
			for (int i = 0; i < phrasesToRemove; i++) {
				phrases.removeFirst();
			}
			phraseCache = null;
			phrasesToRemove = 0;
		} else {
			String[] tokens = doc.text().split("\\s+");
			phrases = new PhraseBuilder().identifyPhrases(tokens, doc.number(), 3, 2);
		}
		
		for (int i = 0; i < phrases.size(); i++) {
			if (usedMemory == memoryConstraint) {
				/*
				 * All keys to be deleted were removed under the assumption that we wouldn't be interrupted
				 * during text parsing. However, that is now not the case and we must make sure to delete 
				 * all special tokens after we retrieve the cached document. This way they will not be 
				 * double-counted
				 */
				for (String specialToken : doc.specialTokens().keySet()) {
					specialTokensToDelete.add(specialToken);
				}
				documentCache = doc;
				phraseCache = phrases;
				phrasesToRemove = i;
				run.write();
				runWritten = true;
				resetMemory();
				break;
			}
			run.storeTriple(doc.number(), phrases.get(i));
			usedMemory = run.consumed();
		}
	}
	
	public void parseStems(Document doc, Run run) {
		stopWords = new Loader().buildDictionary("files/stopwords.txt", 1);
		
		PorterStemmer stemmer = new PorterStemmer();
		
		String[] tokens = doc.text().split("\\s+");
		
		for (String token : tokens) {
			if (usedMemory == memoryConstraint) {
				/*
				 * All keys to be deleted were removed under the assumption that we wouldn't be interrupted
				 * during text parsing. However, that is now not the case and we must make sure to delete 
				 * all special tokens after we retrieve the cached document. This way they will not be 
				 * double-counted
				 */
				
				for (String specialToken : doc.specialTokens().keySet()) {
					specialTokensToDelete.add(specialToken);
				}
				documentCache = doc;
				run.write();
				runWritten = true;
				resetMemory();
				break;
			}
			if (token.length() == 0) {
				continue;
			}
			if (stopWords.containsKey(token)) {
				doc.update(doc.text().replaceFirst(token, ""));
				continue;
			}
			run.store(doc.number(), stemmer.stripAffixes(token));
			usedMemory = run.consumed();
			doc.update(doc.text().replaceFirst(token, ""));
		}
	}
	
	public void mergeRuns() throws IOException, MemoryConstraintException {
		/*
		 * FINAL BOSS:
		 * 	1. keep track of last triples within each partition
		 *  2. before a partition is deleted, save its last word to a class member
		 *  3. when partitions.size() == 0, go through each saved last word to make sure its there
		 *  4. justify how this method respects memory constraints in design doc
		 */
		
		File tempFile = File.createTempFile("sortedRuns", ".tmp", new File("temp"));
		tempFile.deleteOnExit();
		RandomAccessFile newIndex = new RandomAccessFile(tempFile, "rw");
		
		partitions = new LinkedList<Partition>();
		LinkedList<Partition> partitionsToDelete = new LinkedList<Partition>();
		
		Comparator<String> comparator = new ExtractedTripleComparator();
		PriorityQueue<String> leftoverTriples = new PriorityQueue<String>(comparator);
		
		for (Run run: runs) {
			partitions.add(new Partition(run.tempFile()));
		}
		
		System.out.println("Number of partitions to be merged: " + partitions.size());
		
		mergeCandidates = new PriorityQueue<String>(comparator);
		
		// we need to kick off the process with a first round of candidates
		
		for (Partition partition : partitions) {
			mergeCandidates.add(partition.offerCandidate());
		}
		
		String firstFinalist = mergeCandidates.poll().trim();
		
		for (Partition partition : partitions) {
			if (firstFinalist.compareTo(partition.candidate()) == 0) {
				partition.acceptCandidate();
			}
		}
		
		newIndex.writeBytes(firstFinalist + "\n");
//		System.out.println("Assigned first finalist: " + firstFinalist);
		
		// now go through automated merging
		
		int looping = 0;
		while (partitions.size() > 0) {
			if (partitions.size() == 1 && looping > 100000) {
				break;
			}
			if (!partitionsToDelete.isEmpty()) {
				for (Partition partitionToDelete : partitionsToDelete) {
					partitions.remove(partitionToDelete);
//					System.out.println("Partitions remaining: " + partitions.size()); 
				}
				partitionsToDelete.clear();
//				System.out.println("Cleared empty partitions.");
			}
			
			String newCandidate;
			for (Partition partition : partitions) {
				if (partition.candidateAccepted()) {
					newCandidate = partition.offerCandidate();
//					System.out.println("New Candidate " + newCandidate);
					
					if (newCandidate == null) {	// then partition has exhausted all candidates
						partitionsToDelete.add(partition);
						continue;
					} else {
						mergeCandidates.add(newCandidate);
//						System.out.println("Size of merge candidates heap: " + mergeCandidates.size());
						
						if (mergeCandidates.size() > memoryConstraint) {
							throw new MemoryConstraintException("Memory constraint exceeded on merge candidates heap");
						}
					}
				}
			}
			
			if (!mergeCandidates.isEmpty()) {
				String mergeFinalist = mergeCandidates.poll().trim();				
//				System.out.println(mergeFinalist + " is the next finalist for merging");
				
				for (Partition partition : partitions) {
					if (mergeFinalist.compareTo(partition.candidate().trim()) == 0) {
						partition.acceptCandidate();
					}
				}
				
				newIndex.writeBytes(mergeFinalist + "\n");
//				System.out.println("New finalist written: " + mergeFinalist);
				
//				Boolean preventDoubleCount = false;
//				newIndex.seek(0);
//				String line = null;
//				
//				while ((line = newIndex.readLine()) != null) {
//					if (line.trim().compareTo(mergeFinalist) == 0) {
//						System.out.println("Avoided double counting " + mergeFinalist);
//						preventDoubleCount = true;
//						break;
//					}
//				}
//				if (preventDoubleCount != true) {
//					newIndex.writeBytes(mergeFinalist + "\n");
//				}
			}
			if (partitions.size() == 1) {
				looping++;
//				System.out.println("Looping value: " + looping);
			}
		}
		
		// construct posting lists!
		
		newIndex.seek(0);
		
		Charset charset = Charset.forName("US-ASCII");
		Path path = Paths.get("output/" + configuration.toLowerCase() + "Index.txt");

		try (BufferedWriter writer = Files.newBufferedWriter(path, charset, StandardOpenOption.CREATE)) {
			System.out.println("Writing new index...");
			String postingList = null;
			
			String line = null;
		    while ((line = newIndex.readLine()) != null) {
		    	if (postingList == null) {
		    		postingList = line.trim();
		    		continue;
		    	}
		    	if (line.trim().split(" ")[0].compareTo(postingList.split(" ")[0]) == 0) {		    		
		    		Boolean replacementMade = false;
		    		
		    		String[] triple = line.trim().split(" ");
		    		
//			    		for (String component: triple) {
//			    			System.out.println(component);
//			    		}
		    		
		    		String regex = "(" + triple[0] + " " + triple[1] + " )([0-9]+)( \\{.*\\})?";
		    		Pattern pattern = Pattern.compile(regex);
		    		Matcher matcher = pattern.matcher(postingList);
		    		
		    		if (matcher.find()) {
		    			if (Integer.parseInt(triple[2]) > Integer.parseInt(matcher.group(2))) {
//		    				System.out.println("Before replacement: " + postingList);
		    				postingList = postingList.replace(matcher.group(0), line);
//		    				System.out.println("After replacement: " + postingList);
//		    				System.out.println("Replacement made: " + matcher.group(0) + " --> " + line + "\n");
		    				replacementMade = true;
//		    				System.out.println("Replacement made");
		    			} else {
		    				// irrelevant triple ( < ) or duplicate triple ( = )
		    				continue;
		    			}
		    		}

		    		if (replacementMade == false) {	// avoid double counting
		    			postingList += " | " + line;
//		    			System.out.println("Appended " + line + " to an existing posting list");
		    		}		    		
		    	} else {
		    		writer.write(postingList.split(" ")[0] + " --> " + postingList + "\n");
//		    		System.out.println("New posting list: " + postingList.split(" ")[0] + " --> " + postingList + "\n");
		    		postingList = line;
		    	}
		    }
		    
		} catch (IOException x) {
		    System.err.format("IOException: %s%n", x);
		}
		
//		tempFile.delete();
		newIndex.close();
		
		System.out.println("New index built and stored at output/" + configuration.toLowerCase() + "Index.txt");
	}
}
