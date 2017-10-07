package tools;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class Statistics {
	
	
	public Statistics() {
		
	}
	
	public void indexStats(String config) {
		try {
			String filePath = "output/" + config.toLowerCase() + "Index.txt";
			RandomAccessFile file = new RandomAccessFile(filePath, "r");
			
			int lexiconSize = 0;
			
			String line = null;
			while ((line = file.readLine()) != null) {
				lexiconSize++;
			}
			
			long indexSize = file.length();
			
			int[] docFrequencies = new int[lexiconSize];
			
			file.seek(0);
			line = null;
			int i = 0;
			
			while ((line = file.readLine()) != null) {
				if (line.split(" --> ").length > 1) {
					docFrequencies[i] = line.split(" --> ")[1].split(" \\| ").length;
					i++;	
				}
			}
			
			Arrays.sort(docFrequencies);
			int max_df = docFrequencies[docFrequencies.length - 1];
			int min_df = docFrequencies[0];
			
			int mean_df = 0;
			for (int freq : docFrequencies) {
				mean_df += freq;
			}
			
			mean_df = mean_df / docFrequencies.length;
			int median_df = docFrequencies[docFrequencies.length / 2];
			
			System.out.println("Lexicon size (# of terms): " + lexiconSize);
			System.out.println("Index size (# of bytes): " + indexSize);
			System.out.println("Max df: " + max_df);
			System.out.println("Min df: " + min_df);
			System.out.println("Mean df: " + mean_df);
			System.out.println("Median df: " + median_df);
			
			file.close();
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
//		ArrayList<>
	}
}
