/* Jacqueline Justice
 * CSCI 4100
 * Programming Assignment 2
 * The Simple Management of Applications SHell (SMASH) prompts for and executes
 * commands. It supports redirection of I/O and piping between two commands.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

const int MAX_LINE = 256;
const int MAX_ARGS = 64;

int split_cmd(char *cmd, char *args[]);
void process_args(char *args[], int count);

void exec_cmd(char *args[]);
void exec_cmd_red_out(char *args[], char *filename);
void exec_cmd_red_in(char *args[], char *filename);
void exec_cmd_pipe(char *args1[], char *args2[]);

int main() {
  // The command line string
  char cmd[MAX_LINE];

  // The comand line broken into arguments
  char *args[MAX_ARGS];

  // Repeatedly prompt for a command and execute it
  while (1) {
    // Display prompt and read the command line
    printf("> ");
    fgets(cmd, MAX_LINE, stdin);

    // Process the command
    int count = split_cmd(cmd, args);
    process_args(args, count);
  }

  return 0;
}

// Break the command into command line arguments
int split_cmd(char *cmd, char *args[]) {
  int index = 0;

  // Extract each comand line argument and store it in args
  char *arg = strtok(cmd, " \t\v\r\n");
  while (arg != NULL && index < MAX_ARGS - 1) {
    args[index] = arg;
    arg = strtok(NULL, " \t\v\r\n");
    index++;
  }
  args[index] = NULL;
  return index;
}

// Takes an array of command line arguments and an argument count and processes
// the command line
void process_args(char *args[], int count) {

  // Find the location of the delimiter (|, <, or >)
  int i, delim = -1;
  for (i = 0; i < count; i++) {
    if (strcmp(args[i], "|") == 0 || strcmp(args[i], "<") == 0 ||
        strcmp(args[i], ">") == 0)
      delim = i;
  }

  // If user entered a blank line do nothing
  if (count == 0)
    return;

  // If user entered exit, then exit the shell
  else if (count == 1 && strcmp(args[0], "exit") == 0) {
    puts("[exiting smash]\n");
    exit(EXIT_SUCCESS);
  }

  // If there are no pipes or redirects, execute the command
  else if (delim < 0)
    exec_cmd(args);

  // Pipe or redirect appears as the first or last argument
  else if (delim == 0 || delim == count - 1)
    fprintf(stderr, "smash: sytnax error\n");

  // Redirect output
  else if (strcmp(args[delim], ">") == 0) {
    args[delim] = NULL;
    exec_cmd_red_out(args, args[delim + 1]);
  }

  // Redirect input
  else if (strcmp(args[delim], "<") == 0) {
    args[delim] = NULL;
    exec_cmd_red_in(args, args[delim + 1]);
  }

  // Pipe
  else if (strcmp(args[delim], "|") == 0) {
    args[delim] = NULL;
    exec_cmd_pipe(args, &args[delim + 1]);
  }

  // If this point is reached something went wrong with this code
  else {
    fprintf(stderr, "smash: internal error");
  }
}

// Execute the command
void exec_cmd(char *args[]) {

  // Fork a child process
  pid_t pid = fork();

  // Error with fork
  if (pid < 0) {
    fprintf(stderr, "smash: failed to fork process");
    exit(EXIT_FAILURE);
  }

  // Child process
  else if (pid == 0) {
    // Execute the command
    execvp(args[0], args);
    fprintf(stderr, "smash: %s: command not found\n", args[0]);
    exit(EXIT_FAILURE);
  }

  // Parent process
  else {
    // Wait for the child to complete
    wait(NULL);
  }
}

// Redirect output to write to the given file, then execute the command
void exec_cmd_red_out(char *args[], char *filename) {
  // fork a child process
  pid_t pid = fork();
  if (pid < 0) {
    fprintf(stderr, "smash: failed to fork process");
    exit(EXIT_FAILURE);
  } // there was an error creating the process
  else if (pid == 0) {
    // closing standard output
    close(1);
    // creating a new file
    int fd = creat(filename, 0644);
    // file returns negative, displays error, and exits child process
    if (fd < 0) {
      fprintf(stderr, "smash %s: error opening file", filename);
      perror(filename);
      exit(EXIT_FAILURE);
    }
    // run the command
    execvp(args[0], args);
    fprintf(stderr, "smash: %s: command not found\n", args[0]);
    exit(EXIT_FAILURE);

  } // the current process is the child
  else {
    wait(NULL);
  } // the current process is the parent
}

// Redirect input to read from the given file, then execute the command
void exec_cmd_red_in(char *args[], char *filename) {
  // fork a child process
  pid_t pid = fork();
  if (pid < 0) {
    fprintf(stderr, "smash: failed to fork process");
    exit(EXIT_FAILURE);

  } // there was an error creating the process
  else if (pid == 0) {
    // close standard input
    close(0);
    // open file for read only
    int file = open(filename, O_RDONLY);
    // error message if file returns negative
    if (file < 0) {
      fprintf(stderr, "smash %s: error opening file for reading", filename);
      perror(filename);
      exit(EXIT_FAILURE);
    }
    // run command
    execvp(args[0], args);
    fprintf(stderr, "smash: %s: command not found\n", args[0]);
    exit(EXIT_FAILURE);

  } // the current process is the child
  else {
    wait(NULL);

  } // the current process is the parent
}

// Create a pipline between the first command and the second command
void exec_cmd_pipe(char *args1[], char *args2[]) {
  // fork a child process
  pid_t pid = fork();
  //have to call wait if a child doesnt have a parent
  wait(NULL);
  
  if (pid < 0) {
    fprintf(stderr, "smash: failed to fork process");
    exit(EXIT_FAILURE);
    
  } // there was an error creating the process
  else if (pid == 0) {
    //pipe file descriptors
    int pipefds[2];
    
    int result = pipe(pipefds);
    
    if (result < 0) {
      fprintf(stderr, "smash: pipe error ");
      perror("pipe");
      exit(EXIT_FAILURE);
    }//there was an error with pipe
     
    //second child process
    pid_t pid2 = fork();
    
    if (pid2 < 0) {
      fprintf(stderr, "smash: failed to fork the second process");
      exit(EXIT_FAILURE);
    } // there was an error creating the second process
    else if (pid2 == 0) {
      close(pipefds[0]);
      dup2(pipefds[1], 1);
      close(pipefds[1]);
      // run command
      execvp(args1[0], args1);
      fprintf(stderr, "smash: %s: command not found\n", args1[0]);
      exit(EXIT_FAILURE);
    } //redirecting standard output to write to the pipes input
    close(pipefds[1]);
    dup2(pipefds[0], 0);
    close(pipefds[0]);
    //redirecting standard input to write to the pipes output 
    
    // run command
    execvp(args2[0], args2);
    fprintf(stderr, "smash: %s: command not found\n", args2[0]);
    exit(EXIT_FAILURE);
   
  } // the current process is the child
  else {
    wait(NULL);
  } // the current process is the parent
}
