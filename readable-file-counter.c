// READABLE FUNCTION  //

/* A Recursive Readable File Counter Program Written in C */
// NAME: Linnea P. Castro
// DATE: 13 SEP 2023
// COURSE: CS 360
// ASSIGNMENT: 3

// LIBRARIES TO INCLUDE //
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define DEBUG 0

// readable FUNCTION TEMPLATE
int readable(char *inputPath){

// VARIABLES
int accesscheck;
int numreadablefiles;
char *buffer;
buffer = NULL; // Must initialize this!
long max;
numreadablefiles = 0; // Init number of readable files to 0
struct stat area, *s = &area;
DIR *dir;
struct dirent *entry;
int changingdir;
int result;

// IF NULL WAS PASSED IN
if (inputPath == NULL){ // If inputPath passed in is NULL
  max = pathconf(".", _PC_NAME_MAX); // Find length of path
  buffer = malloc(max); // Malloc enough space for size of buffer
                        // Must malloc because inputPath = NULL and we have no space!
                        // Malloc is for memory during execution(run time), not compilation
  inputPath = getcwd(buffer, max); // currentworkingdir = current working directory
  if (DEBUG) printf("Current working directory is: %s\n", inputPath);
}

// DO WE HAVE READ ACCESS?
accesscheck = access(inputPath, R_OK); // Check access for argument that was passed in

if (accesscheck != 0){ // If I did not pass the accesscheck, ie, no read/execute access
  if (DEBUG) printf("Is this failing?\n");
  return (errno * (-1)); // Return negative errno if it fails
}

if (lstat(inputPath, s) == 0){ // Use lstat to check what argument the readable function passed in
                               // ie, regular, directory, etc.
  // REGULAR FILE CHECK
  if (S_ISREG (s->st_mode)){ // If what was passed in to readable function was a reg. file
    accesscheck = access(inputPath, R_OK); // Check access
    if (accesscheck == 0){ // We DO have read access
      if (DEBUG) printf("file name is: %s\n", inputPath);
      numreadablefiles++; // Increment numreadablefiles
      return numreadablefiles; // Return numreadablefiles
    }
    return numreadablefiles; // No access to file, return
  }
  
  // DIRECTORY CHECK
  else if (S_ISDIR (s->st_mode)){ // If it is a directory
    if (DEBUG) printf("We are in a directory!\n");
    accesscheck = access(inputPath, R_OK | X_OK);// Check that we have read and write access to this directory
    if (accesscheck == 0){ // If we DO indeed have access:
      dir = opendir(inputPath); // Opens a directory stream and returns a pointer to that directory stream
      changingdir = chdir(inputPath); // Change directories so we are INSIDE that directory we were passed
      if (dir == NULL){ // There was an error opening the directory stream
        return 0; // Don't add anything to numreadablefiles
      }
 
      // BEGIN EXAMINING DIRECTORY STREAM
      while((entry = readdir(dir)) != NULL){ // As long as there are entries in the directory stream to examine
        result = strcmp(entry->d_name, "."); // Check if entry->d_name is "." (pwd)
        if (result == 0){
          continue; // If it is pwd, continue
        }
        result = strcmp(entry->d_name, ".."); // Check if entry->d_name is ".." (parent)
        if (result == 0){
          continue; // If it is parent, continue
        }
        
        // WHAT ARE WE LOOKING AT?
        if (lstat(entry->d_name, s) == 0){ // Use lstat to examine what we're looking at

          // REGULAR FILE
          if (S_ISREG (s->st_mode)){ // If it is a regular file
            accesscheck = access(entry->d_name, R_OK); 
            if (accesscheck == 0){ // We do have access
              if (DEBUG) printf("file name is: %s\n", entry->d_name);
              numreadablefiles++; // Add to numreadablefiles 
            }
          }
          
          // DIRECTORY
          if (S_ISDIR (s->st_mode)){
            if (DEBUG) printf("Is the program getting here?  It is getting here.\n");
            accesscheck = access(entry->d_name, R_OK | X_OK); // Check access
            if (accesscheck == 0){ // We DO have read and execute privilege in this directory
              if (DEBUG) printf("Am I changing directories?\n");
              result = readable(entry->d_name); // Recursively call readable, assign it to result
              if (result >= 0){ // Check to see if result was a negative number 
                numreadablefiles += result; // Only add to numreadablefiles if we received positive answers
              }
              changingdir = chdir(".."); // Go back to parent directory
            }
          }
        }
      }
    }
  }

}

// ONLY FREE BUFFER IF BUFFER WAS NULL
if (buffer == NULL){
  free(buffer);
}

// RETURN FINAL NUMREADABLEFILES
return numreadablefiles;
};

