/*************************************************************************************
 * shellfuncts.c - code definitions for your functions
 *
 *************************************************************************************/

#include <stdio.h> // For input/output functions: printf, fprintf, fopen
#include <stdlib.h> // For exit(), atoi (ASCII to Integer)
#include <string.h> // For string manipulation functions: strtok, strcmp, strlen
#include <unistd.h> // For POSIX xalls: fork, execl, sleep, getpid
#include <sys/wait.h> // For waitpid
#include <ctype.h> // For character handling functions: isspace
#include "shellfuncts.h"

/*************************************************************************************
 * hello - sends hello world to the user! I'm doing proper function commenting so future
 *         coders might not find my code as painful.
 *
 *		Params:	param1 - I explain my parameters, like this is 1 for American, 2 for
 *                      Australian
 *
 *		Returns: always returns 1, no matter what. Fairly useless.
 *
 *************************************************************************************/

// int hello(int param1) {
// 	// I'm commenting to explain that this checks param and changes the message
// 	if (param1 == 1)
// 		send_msg("Hello world!\n");
// 	else
// 		send_msg("G'day world!\n");

// 	// Return 1 because, why not.
// 	return 1;
// }

/*************************************************************************************
 * hello - sends hello world to the user! I'm doing proper function commenting so future
 *         coders might not find my code as painful.
 *
 *    Params:  param2 - not a very good parameter name - something better might be
 *                      say, msgstr or sendtext
 *
 *************************************************************************************/

// void send_msg(const char *param2) {
// 	printf("%s", param2);
// }

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
		if (*p == '&' && (isspace((unsigned char)*(p-1)) && *(p+1) == '\0')) {
			*background = true; // Set background flag
			p++; // Move past '&'
			continue; // Skip to next iteration
		}

		//If token contain double quotes, parse until next quote is found
		if (*p == '"') {
			p++; // Skip the opening quote
			args[argc++] = p; // Start of the argument
			while (*p && *p != '"') p++; // Find the closing quote
			if (*p) {
				*p = '\0'; // Null-terminate the argument
				p++; // Move past closing quote
			} else {
				fprintf(stderr, "Unmatched double quotes in input\n");
				return -1; // Return error if unmatched quotes
			}
		} else {
			// If not quoted, find the next whitespace or end of line
			while (*p && !isspace((unsigned char)*p)) p++;
		}

		// Store the start of the argument
		args[argc++] = p;

		// Find the end of the argument
		while (*p && !isspace((unsigned char)*p)) p++;

		// Null-terminate the argument
		if (*p) {
			*p = '\0'; // Replace space with null terminator
			p++; // Move past null terminator
		}
		
		// Prevent overflow: only allow up to MAX_ARGS - 1 arguments to reserve space for NULL character
		if (argc >= MAX_ARGS - 1) {
			fprintf(stderr, "Too many arguments, max is %d\n", MAX_ARGS - 1);
			return -1; // Return error if too many arguments
		}
	}

	args[argc] = NULL; // Null-terminate the argument list
	return argc; // Return the number of tokens found

	// char *tok = strtok(line, " \t\n");
	// while (tok != NULL && argc < MAX_ARGS - 1) {
	// 	args[argc++] = tok;
	// 	tok = strtok(NULL, " \t\n");
	// }
	// args[argc] = NULL;
	// return argc;
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
	if (strcmp(args[0], "create") == 0 && argc == 2) valid = true;
	if (strcmp(args[0], "update") == 0 && argc == 4) valid = true;
	if (strcmp(args[0], "list") == 0 && argc == 2) valid = true;
	if (strcmp(args[0], "dir") == 0 && argc == 1) valid = true;

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
		// Child process
		if (strcmp(args[0], "create") ==0) {
			// create: open a file using fopen() to create the file
			FILE *file = fopen(args[1], "w"); // open in write mode
			if (file == NULL) {
				perror("create"); // If fopen fails, print error
				exit(1); // Exit child process with error
		}
		fclose(file);
		exit(0);
		} else if (strcmp(args[0],"update") == 0) {
			// update: 	Append lines of text to the named file. 
			// In particular, the text string <text> is appended to the end of the file, <number> times. 
			// To simulate a (very) slow device, the update command sleeps for several seconds after appending each line.
			const char *filename = args[1]; // Get the filename from args
			int number = atoi(args[2]); // Number of lines to write
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
				sleep(strlen(text) / 5); // use sleep(strlen( <text>/5) to simulate slow device
			}
			printf("Update has completed with pid %d\n", getpid()); // print a message on workstation saying the update has completed
			fclose(file); // Close the file
			exit(0); // Exit child process successfully
		} else if (strcmp(args[0], "list") == 0) {
			// list: 	Display the contents of the named file on the workstation screen
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
		} else if (strcmp(args[0], "dir") == 0) {
			// dir: 	Display the contents of the current directory on the workstation screen
			// Use execl to execute the 'ls' command to list directory contents
			execl("/bin/ls", "ls", "-l", (char *)NULL);
			// If execl fails, print an error message and exit
			perror("execl");
			exit(1); // Exit child process with error
		}
	} else if (pid > 0) {
		// Parent process
		printf("spawned child pid %d\n", pid); // Print the child process ID
		if (!background) {
			waitpid(pid, NULL, 0); // Wait for the child process to finish if not in background mode
		}
		// If in background mode, do not wait for the child process
	}
}