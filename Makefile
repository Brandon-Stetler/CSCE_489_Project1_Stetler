# Makefile for building myshell program
# This Makefile compiles myshell and its dependencies
# It includes rules for compiling object files and linking them into the final executable
# It also includes a clean rule to remove generated files
# It uses gcc as the compiler with debugging and warning flags
CC=gcc
# Compiler flags for debugging and warnings
CFLAGS=-ggdb -Wall -Wextra -O0
# Define the source files and their dependencies
SRC = myshell.c shellfuncts.c
# Define the object files and dependencies
# DEPS is a list of header files that the source files depend on
# OBJ is a list of object files to be created from the source files
DEPS = shellfuncts.h
OBJ = myshell.o shellfuncts.o 
# Rule to build the myshell executable from object files
# The $@ is the target (myshell), $^ is the list of dependencies (myshell.o shellfuncts.o)
# The %.o: %.c rule compiles each .c file into an object file
# The $< is the first prerequisite (the .c file being compiled)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

myshell: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f *.o *~ myshell 
