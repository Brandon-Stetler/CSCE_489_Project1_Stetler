/*************************************************************************************
 * myshell - Project 1 of CSCE 489 
 * Author: Capt Brandon Stetler, AFIT Master's Student
 * Due date: July 18, 2025
 *************************************************************************************/

#include <stdio.h> // For input/output functions
#include <stdlib.h> // For exit()
#include <string.h> // For string manipulation functions
#include <stdbool.h> // For bool type
#include <unistd.h> // For getpid()
#include "shellfuncts.h" // For parse_line() and execute_command()


int main(void){
	char line[MAX_LINE]; // Buffer to hold the command line input
	char *args[MAX_ARGS]; // Array to hold parsed token pointers
	bool background; // Flag to indicate if the command should run in the background

	// Print a welcome message	
	printf("Welcome to myshell! PID = %d. Type 'halt' to exit.\n", getpid());

	// Main REPL loop; read a line, parse it, and execute the command
	while (1){
		printf("shell> "); // Prompt for input
		if (!fgets(line, MAX_LINE, stdin)) 
			// Read in line, and if EOF is reached or an error occurs, break the loop
			break;

		// Tokenize input line into arguments, detect '&'
		int argc = parse_line(line, args, &background);
		if (argc == 0) {
			continue; // If no arguments, prompt again
		}

		// Logic for 'halt' command: no fork needed
		if (strcmp(args[0], "halt") == 0 && argc == 1) {
			printf("Exiting myshell. Goodbye!\n");
			break; // Exit the shell
		}

		// Send other commands to execute_command
		execute_command(argc, args, background);

	}

	return 0; // Exit the shell
}
