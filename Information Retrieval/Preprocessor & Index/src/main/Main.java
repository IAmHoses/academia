//package main;
//import java.io.IOException;
//
//import exceptions.MemoryConstraintException;
//import indexing.Indexer;
//import tools.Statistics;
//
//public class Master {
//	public static void main(String[] args) {
//		String configuration = "PHRASE";
//		int memoryConstraint = 1000;
//		
//		long t0 = System.nanoTime();
//		System.out.println("System started!");
//		
//		Indexer indexer = new Indexer(configuration, memoryConstraint);
////		indexer.build();
//		
//		System.out.println("Generating runs...");
//		indexer.generateRuns();
//		System.out.println("done!");
//		
//		long t1 = System.nanoTime();
//
//		System.out.println("Merging runs...");
//		try {
//			indexer.mergeRuns();
//		} catch (IOException | MemoryConstraintException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//		System.out.println("done!");
//
//		long t2 = System.nanoTime();
//		
//		long delta_t0 = (long) ((t1 - t0) * Math.pow(10, -6));
//		long delta_t1 = (long) ((t2 - t1) * Math.pow(10, -6));
//		long delta_t2 = (long) ((t2 - t0) * Math.pow(10, -6));
//		
//		System.out.println("Time taken to create temporary files: " + String.valueOf(delta_t0) + " milliseconds");
//		System.out.println("Time taken to merge temporary files: " + String.valueOf(delta_t1) + " milliseconds");
//		System.out.println("Time taken to build inverted index: " + String.valueOf(delta_t2) + " milliseconds\n");
//		
//		Statistics measurer = new Statistics();
//		measurer.indexStats(configuration);
//		
//		System.out.println("System finished!");
//	}
//}
