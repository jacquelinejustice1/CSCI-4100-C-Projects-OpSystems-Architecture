/*
Jacqueline Justice
Assignment 5 - PWDED
This assignement implements a simple C version of UNIX
pwd program called pwded.
*/

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Function Prototypes
void inum_to_filename(ino_t inode, char *buffer, int size);
ino_t filename_to_inum(char *filename);
void display_path(ino_t inode);

int main() {

  ino_t inode = filename_to_inum(".");
  display_path(inode);
  printf("\n");
  return 0;
}
// function to convert inode number to filename
void inum_to_filename(ino_t inode, char *buffer, int size) {
  DIR *dir_ptr = opendir(".");
  if (dir_ptr == NULL) {
    perror(buffer);
    exit(EXIT_FAILURE);
  }

  struct dirent *dirent_ptr = readdir(dir_ptr);
  if (dirent_ptr == NULL) {
    fprintf(stderr, "error looking for inum %d\n", (int)inode);
    exit(EXIT_FAILURE);
  }

  while ((dirent_ptr = readdir(dir_ptr)) != NULL) {

    ino_t curr = filename_to_inum(dirent_ptr->d_name);

    if (curr == inode) {
      strncpy(buffer, dirent_ptr->d_name, size);
      buffer[BUFSIZ - 1] = '\0';
      closedir(dir_ptr);
      return;
    }
  }
}
// function to convert filename to inode number
ino_t filename_to_inum(char *filename) {

  struct stat info;
  int result = lstat(filename, &info);

  if (result == -1) {
    fprintf(stderr, "Cannot stat ");
    perror(filename);
  }
  return info.st_ino;
}
// recursively displays the path of the current directory
void display_path(ino_t inode) {

  char buffer[BUFSIZ];

  ino_t parent = filename_to_inum("..");

  if (parent == inode) {
    return;
  }
  int change = chdir("..");
  if (change < 0) {
    perror("..");
    exit(EXIT_FAILURE);
  }
  inum_to_filename(inode, buffer, BUFSIZ);
  display_path(parent);
  printf("/%s", buffer);
}
