# Makefile for building myshell program

# This Makefile compiles myshell and its dependencies
# It includes rules for compiling object files and linking them into the final executable
# It also includes a clean rule to remove generated files
# It uses gcc as the compiler with debugging and warning flags
CC = gcc

# Compiler flags for debugging and warnings
CFLAGS = -ggdb -Wall -Wextra -O0

# Define the object files and dependencies
# DEPS is a list of header files that the source files depend on
# OBJ is a list of object files to be created from the source files
DEPS = shellfuncts.h
OBJ = myshell.o shellfuncts.o 

# The .PHONY target is used to declare that 'all' and 'clean' are not actual files
.PHONY: all clean
# The 'all' target is the default target that will be built when 'make' is run without arguments
all: myshell

# Rule to compile each .c file into a .o file
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Rule to link the object files into the final executable
myshell: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# The 'clean' target removes all object files and the executable
# This is useful for cleaning up the directory before a new build
clean:
	rm -f *.o *~ myshell 
