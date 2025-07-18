# AFIT-CSCE489-PROJ1
Author: Capt Brandon Stetler, AFIT Master's student

This project created a simple Unix shell that executes a limited subset of commands, including "create", "update", "list", "dir", and "halt". The code can be run by first calling `make clean`, then `make`, and finally `./myshell`. 

## Commands
- `create <name>` - Create a new file in the current directory with the given name. If the file already exists, print an error message on the workstation screen.
- `update <name> <number> <text>` - Append lines of text to the named file. In particular, the text string <text> is appended to the end of the file, <number> times. To simulate a (very) slow device, the update command sleeps for several seconds after appending each line.
- `list <name>` - Display the contents of the named file on the workstation screen. If the file does not exist, print an error message on the workstation screen and terminate the command.
- `dir` - List the names of all files in the current directory. Note that since there is no command to change to a new directory, the current directory is the only directory.
- `halt` - Terminate the shell.

##
Like UNIX, the shell executes commands in the foreground by default. This means that it waits for the command to finish before prompting the user for another command. However, also like UNIX, the shell allows the user to specify that the command is to be executed in the background by placing an ampersand (i.e., &) at the end of the command line. In this case the shell immediately prompts the user for another command without waiting for the current command to complete execution.

## Sample Input
- `dir`
- `create file1`
- `dir`
- `update file1 3 “this is a line”`
- `list file1`
- `update file1 2 “this is another line”`
- `list file1`
- `create file2`
- `update file2 10 “this is the first 10 lines” &`
- `list file2`
- `create file3`
- `dir`
- `list file2`
- `update file3 10 “first update” &`
- `update file3 12 “second update”`
- `list file3`
- `list file4`
- `dir`
- `halt`