/*************************************************************************************
 * shellfuncts.h - header file for your shell functions
 *
 *************************************************************************************/
#ifndef SHELLFUNCTS_H // include guard
#define SHELLFUNCTS_H // include guard

#include <stdbool.h> // for bool type

#define MAX_LINE 1024 // Maximum length of command line input
#define MAX_ARGS   16 // Maximum number of arguments supported in this project

int parse_line(char *line, char **args, bool *background); // Function to parse a line into arguments

void execute_command(int argc, char **args, bool background); // Function to execute a command

#endif // End guard

