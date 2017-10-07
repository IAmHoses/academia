package tools;
/*
 * Takes a file of text terms and generates 
 */

import java.io.*;
import java.nio.file.*;
import java.nio.charset.Charset;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import containers.Document;
import containers.PostingList;
import containers.Query;
import containers.Triple;

public class Loader {
	
	public String[] populateArray(String[] array, String textFile) {
		Path path = Paths.get(textFile);
		Charset charset = Charset.forName("US-ASCII");
		
		try (BufferedReader reader = Files.newBufferedReader(path, charset)) {
			String line = null;
			
			for (int i = 0; i < array.length; i++) {
				line = reader.readLine();
				array[i] = line;
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		return array;
	}
	
	public HashMap<String, String> buildDictionary(String textFile, int fields) {
		HashMap<String, String> dictionary = new HashMap<String, String>();
		
		Path path = Paths.get(textFile);
		Charset charset = Charset.forName("US-ASCII");
		
		try (BufferedReader reader = Files.newBufferedReader(path, charset)) {
			String line = null;
			
			while ((line = reader.readLine()) != null) {
				if (fields == 1) {
					dictionary.put(line, line);
				}
				if (fields == 2) {
					String[] keyValuePair = line.split(" : ");
					dictionary.put(keyValuePair[0], keyValuePair[1]);	
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		return dictionary;
	}
	
	public HashMap<String, PostingList> loadIndex(String indexPath) {
		HashMap<String, PostingList> index = new HashMap<String, PostingList>();
		
		
		Path path = Paths.get(indexPath);
		Charset charset = Charset.forName("US-ASCII");
		
		try (BufferedReader reader = Files.newBufferedReader(path, charset)) {
			String line;
			
			while ((line = reader.readLine()) != null) {
				PostingList postingList = new PostingList();
				
				String[] splitLine = line.split(" --> ");
				String term = splitLine[0];
			
				for (String rawTriple : splitLine[1].split(" \\| ")) {
//					System.out.println(configuration + ": " + term);
					String[] tripleComponents = rawTriple.split(" ");
					Triple newTriple = new Triple(tripleComponents[0], tripleComponents[1], 
							Integer.parseInt(tripleComponents[2]));
					postingList.storeTriple(tripleComponents[1], newTriple);
				}
				index.put(term, postingList);
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return index;
	}
	
	public HashMap<String, Query> loadShortQueries(String textFile) {
		HashMap<String, Query> queries = new HashMap<String, Query>();
		
		Path path = Paths.get(textFile);
		Charset charset = Charset.forName("US-ASCII");
		
		try (BufferedReader reader = Files.newBufferedReader(path, charset)) {
			String queryNumber = null;
			String queryString = null;
			
			Pattern numberPattern = Pattern.compile("(<num> Number: )([0-9]{3})( )?");
			Pattern topicPattern = Pattern.compile("(<title> Topic: +)(.+)");
			
			String line = null;
			
			while ((line = reader.readLine()) != null) {
				Matcher numberMatcher = numberPattern.matcher(line);
				Matcher topicMatcher = topicPattern.matcher(line);
				
				if (numberMatcher.matches()) {
					queryNumber = numberMatcher.group(2);
					continue;
				}
				if (topicMatcher.matches()) {
					queryString = topicMatcher.group(2);
				}
				if (line.equals("</top>")) {
					queries.put(queryNumber, new Query(queryNumber, queryString));
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		return queries;
	}
	
	public HashMap<String, Query> loadLongQueries(String textFile) {
		HashMap<String, Query> queries = new HashMap<String, Query>();
		
		Path path = Paths.get(textFile);
		Charset charset = Charset.forName("US-ASCII");
		
		try (BufferedReader reader = Files.newBufferedReader(path, charset)) {
			String queryNumber = null;
			String queryString = "";
			
			Pattern numberPattern = Pattern.compile("(<num> Number: )([0-9]{3})( )?");
			Pattern narrativePattern = Pattern.compile("(<narr> Narrative: )( )?");
			
			String line = null;
			
			while ((line = reader.readLine()) != null) {
				Matcher numberMatcher = numberPattern.matcher(line);
				Matcher narrativeMatcher = narrativePattern.matcher(line);
				
				if (numberMatcher.matches()) {
					queryNumber = numberMatcher.group(2);
					continue;
				}
				if (narrativeMatcher.matches()) {
					while (!(line = reader.readLine()).equals("</top>")) {
						queryString += line;
					}
					
					if (line.equals("</top>")) {
						queries.put(queryNumber, new Query(queryNumber, queryString));
						queryString = "";
					}
				}

			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		return queries;
	}
	
	public HashMap<String, Document> loadRawDocuments() {
		String[] documentFiles = new String[11];
		documentFiles = populateArray(documentFiles, "Resources/documents.txt");
		
		HashMap<String, Document> rawDocuments = new HashMap<String, Document>();
		
		for (String file : documentFiles) {
//			System.out.println(file);
			
			Path path = Paths.get(file);
			Charset charset = Charset.forName("US-ASCII");
			
			try (BufferedReader reader = Files.newBufferedReader(path, charset)) {
				String docNumber = null;
				String docText = "";
				
				Pattern docNumberPattern = Pattern.compile("(<DOCNO> )(.+)( </DOCNO>)");
				Pattern docParentPattern = Pattern.compile("(<PARENT> )(.+)( </PARENT>)");
				Pattern commentPattern = Pattern.compile("<!--.*-->");
				
				String line = null;
				
				while ((line = reader.readLine()) != null) {
					if (line.equals("<DOC>")) {
						while (!(line = reader.readLine()).equals("</DOC>")) {
							Matcher docNumberMatcher = docNumberPattern.matcher(line);
							Matcher docParentMatcher = docParentPattern.matcher(line);
							Matcher commentMatcher = commentPattern.matcher(line);
							
							if (docNumberMatcher.matches()) {
								docNumber = docNumberMatcher.group(2);
							}
							else if (docParentMatcher.matches() || commentMatcher.matches()) {
								continue;
							}
							else if (line.length() == 0 || line.equals("<TEXT>") || line.equals("</TEXT>")) {
								continue;
							}
							else {
								docText += line + '\n';
							}
						}
						
						rawDocuments.put(docNumber, new Document(docNumber, docText));
						docText = "";
					}
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		return rawDocuments;
	}
}
