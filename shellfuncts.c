/*************************************************************************************
 * shellfuncts.c - Implementation of shell functions for myshell
 *
 *************************************************************************************/

#include <stdio.h> // For input/output functions: printf, fprintf, fopen
#include <stdlib.h> // For exit(), atoi (ASCII to Integer)
#include <string.h> // For string manipulation functions: strtok, strcmp, strlen
#include <unistd.h> // For POSIX calls: fork, execl, sleep, getpid
#include <sys/wait.h> // For waitpid
#include <ctype.h> // For character handling functions: isspace
#include "shellfuncts.h"


/**************************************************************************************
 * parse_line - Parses a command line into arguments and detects background execution
 *
 *    Params:  line - input command line string
 *             args - array to hold parsed argument pointers
 *             background - pointer to a boolean flag indicating background execution
 *
 *    Returns: Number of arguments parsed, or -1 on error
 *
 *************************************************************************************/
int parse_line(char *line, char **args, bool *background) {
	int argc = 0; // Token counter
	*background = false; // Initialize background flag
	char *p = line; // Pointer to current position in line

	// Loop until end of string
	while (*p) {
		// Skip leading whitespace
		while (*p && isspace((unsigned char)*p)) p++;

		// If we reach the end of the line, break
		if (*p == '\0') break;

		// Check for background execution indicator '&'
		if (*p == '&' && (isspace((unsigned char)*(p+1)) || *(p+1) == '\0')) {
			*background = true; // Set background flag
			printf("Running command in background\n"); // Print message indicating background execution
			p++; // Move past '&'
			continue; // Skip to next iteration
		}

		//If token contains double quotes, parse until next quote is found
		if (*p == '"') {
			p++; // Skip the opening quote
			char *token_start = p; // Token start position
			while (*p && *p != '"') p++; // Find the closing quote
			if (*p == '"') {
				*p++ = '\0'; // Null-terminate the argument
			} else {
				fprintf(stderr, "Unmatched double quotes in input\n"); // Print error for unmatched quotes
				return -1; // Return error if unmatched quotes
			}
			args[argc++] = token_start; // Save token’s starting address into args[], then increment argc
		} else {
			// If not quoted, find the next whitespace or end of line
			char *token_start = p; // Token start position
			// Find the end of the token (next whitespace or end of line)
			while (*p && !isspace((unsigned char)*p)) p++;
		// Null-terminate the argument
		if (*p) {
			*p++ = '\0'; // Replace space with null terminator and move past it
		}
		args[argc++] = token_start; // Save token’s starting address into args[], then increment argc
	}

		// To prevent overflow, only allow up to (MAX_ARGS - 1) arguments to reserve space for NULL character at the end
		if (argc >= MAX_ARGS - 1) {
			fprintf(stderr, "Too many arguments, max is %d\n", MAX_ARGS - 1); // Print error if too many arguments
			return -1; // Return error if too many arguments
		}
	}

	args[argc] = NULL; // Null-terminate the argument list
	return argc; // Return the number of tokens found
}

/*************************************************************************************
 * execute_command - executes a command with arguments, handling background execution
 *
 *    Params:  argc - number of arguments
 *             args - array of argument strings
 *             background - flag indicating if the command should run in the background
 *
 *************************************************************************************/
void execute_command(int argc, char **args, bool background) {
	bool valid = false; // Flag to check if command is valid

	// Check each supported command and its expected arg count
	if (strcmp(args[0], "create") == 0 && argc == 2) valid = true; // create <name>
	if (strcmp(args[0], "update") == 0 && argc == 4) valid = true; // update <name> <number> <text>
	if (strcmp(args[0], "list") == 0 && argc == 2) valid = true; // list <name>
	if (strcmp(args[0], "dir") == 0 && argc == 1) valid = true; // dir

	// If not valid, print error message and return
	if (!valid) {
		fprintf(stderr, "Invalid command or incorrect number of arguments.\n");
		return;
	}

	// Fork a new process to execute the command
	pid_t pid = fork();
	if (pid < 0) {
		perror("fork"); // Print error if fork fails
		exit(1); // Exit with error code
	}	
	else if (pid == 0) {
		// Child processes

		if (strcmp(args[0], "create") == 0) {
			// create: open a file using fopen() to create the file
			const char *filename = args[1]; // Begin by getting the filename from args

			// Check to see if filename already exists
			FILE *check_file = fopen(filename, "r"); // Try to open the file in read mode
			// If fopen returns NULL, the file does not exist
			// If fopen returns a valid pointer, the file exists
			// If the file exists, print an error message and exit
			if (check_file != NULL) {
				fprintf(stderr, "create: File '%s' already exists.\n", filename); // Print error if file exists
				fclose(check_file); // Close the file if it exists
				exit(1); // Exit child process with error
			}
			// If the file does not exist, create it
			FILE *file = fopen(filename, "w"); // open file in write mode
			if (file == NULL) {
				perror("create"); // If fopen fails, print error
				exit(1); // Exit child process with error
		}
		fclose(file);
		exit(0);
		} 
		
		else if (strcmp(args[0],"update") == 0) {
			// update: Append lines of text to the named file
			// In particular, the text string <text> is appended to the end of the file, <number> times
			// To simulate a (very) slow device, the update command sleeps for several seconds after appending each line
			const char *filename = args[1]; // Get the filename from args
			int number = atoi(args[2]); // Number of lines to write (atoi = ASCII to Integer)
			const char *text = args[3]; // Text to append at end
			FILE *file = fopen(filename, "a"); // Open file in append mode
			if (file == NULL) {
				perror("update"); // If fopen fails, print error
				exit(1); // Exit child process with error
			}
			// Loop to write the text to the file 'number' times
			for (int i = 0; i < number; i++) {
				fprintf(file, "%s\n", text); // Write the text to the file
				fflush(file); // Ensure the output is written immediately
				sleep(strlen(text) / 5); // use sleep(strlen(<text>) / 5) to simulate slow device
			}
			printf("Update has completed with pid %d\n", getpid()); // print a message on workstation saying the update has completed
			fclose(file); // Close the file
			exit(0); // Exit child process successfully
		} 
		
		else if (strcmp(args[0], "list") == 0) {
			// list: Display the contents of the named file on the workstation screen
			const char *filename = args[1]; // Get the filename from args
			FILE *file = fopen(filename, "r"); // Open file in read mode
			if (file == NULL) {
				// If the file does not exist, print an error message on the workstation screen and terminate the command
				fprintf(stderr, "list: File '%s' does not exist.\n", filename);
				exit(1); // Exit child process with error
			}
			fclose(file); // Close the file after checking existence
			// Use execl to execute the 'cat' command to display the file contents
			execl("/bin/cat", "cat", filename, (char *)NULL);
			// If execl fails, print an error message and exit
			perror("execl");
			exit(1); // Exit child process with error
		} 
		
		else if (strcmp(args[0], "dir") == 0) {
			// dir: Display the contents of the current directory on the workstation screen
			// Use execl to execute the 'ls' command to list directory contents
			execl("/bin/ls", "ls", (char *)NULL);
			// If execl fails, print an error message and exit
			perror("execl");
			exit(1); // Exit child process with error
		}
	} 
	else if (pid > 0) {
		// Parent process
		printf("child process for %s spawned with pid %d\n", args[0], pid); // Print the child process ID
		if (!background) {
			waitpid(pid, NULL, 0); // Wait for the child process to finish if not in background mode
		}
		// If in background mode, do not wait for the child process
	}
}