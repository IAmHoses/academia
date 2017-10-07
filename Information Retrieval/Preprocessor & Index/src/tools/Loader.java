package tools;
/*
 * Takes a file of text terms and generates 
 */

import java.io.*;
import java.nio.file.*;
import java.nio.charset.Charset;
import java.util.*;

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
}
