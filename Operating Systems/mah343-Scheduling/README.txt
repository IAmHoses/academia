Compilation Instructions:
	After unzipping mah343-Scheduling.tar.gz, all source code can be found in the directory /home/mah343/mah343-Scheduling/Scheduling\ Algorithms/src/scheduling
	Simply compile all the java code with your preferred method; I used Eclipse .jar exportation, but you can achieve compilation via the command line interface.

Run Instructions
	Using the .jar file I generated, you can run my code via the command line interface with the following command:
		java -jar mah343-Scheduling.jar <scheduling algorithm> [optional algorithm parameter] [verbose] <process time file n>*
	Note that the files pointer argument elements must be in the same directory as the jar file if they are named explicitly without pathing. That is, if you supply the program with files pointer argument elements of the form "process-1.txt" "process-2.txt", said files will *need* to be in the same directory as the jar file. Else, you will need to put a path before each barebones file name. That is, each file argument element will need to be a full path of a form similar to "/home/mah343/mah343-Scheduling/process-1.txt" "/home/mah343/mah343-Scheduling/process-2.txt"