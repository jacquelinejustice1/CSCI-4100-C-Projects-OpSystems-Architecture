/*
Jacqueline Justice
CSCI 4100
Assignment 1
A simple C version of the UNIX cat program called kittycat that
allows you to display the contents of one or more text files, If the user enters the file name, it allows the user to display standard input.
*/

//A preprocessor statement to include a header file containing
//definitions of I/O functions
#include <stdio.h>
//For closing the file
#include <stdlib.h>

// prototype function for display_stream
void display_stream(FILE *stream);

/*
Main function that takes stardard user input if the user enters the kittycat command and displays what the user enters
otherwise, opens the file that the user enters, and outputs the file
If there is no file, displays a file not found error
*/
int main(int argc, char *argv[]) {
  int i;
  if (argc < 2) {
    
    display_stream(stdin);

  } else {
    
    for (i = 1; i < argc; i++) {
      
      FILE *stream = fopen(argv[i], "r");

      if (stream == NULL) {
        perror("error: file not found");
        exit(0);
      }
      display_stream(stream);
      printf("\n");
      fclose(stream);
    }
  }
}
// function that takes a file stream, opens and displays
//it character by character until the end of the file
//if the file is not found, displays a print error message
void display_stream(FILE *stream) {
  
  if (stream == NULL) {
    perror("error: file not found");
    exit(0);
  }

  int character;

  while (character != EOF) {
    character = fgetc(stream);
    putchar(character);
  }
}
