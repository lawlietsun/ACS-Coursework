#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "common.h"

void usage(char *prog_name, char *filename) {
   printf("Usage: %s <data to add to %s>\n", prog_name, filename);
   exit(0);
}

int main(int argc, char *argv[]) {
  uid_t userid = getuid();
  char *buffer = (char *) ec_malloc(100);
  char *datafile = (char *) ec_malloc(strlen(NOTE_FILE) + 1);
  strcpy(datafile, NOTE_FILE);
  
  if (argc < 2) {
    // No arguments implies no note data was provided
    usage(argv[0], datafile);
  }
  
  strcpy(buffer, argv[1]); // Copy note into buffer
  printf("[DEBUG] user id: \'%d\'\n", userid);
  printf("[DEBUG] buffer   @ %p: \'%s\'\n", buffer, buffer);
  printf("[DEBUG] datafile @ %p: \'%s\'\n", datafile, datafile);

  // Open our data file to write
  int fd = open(datafile, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);
  if (fd == -1) {
    fatal("Problem opening file in main()");
  }
  printf("[DEBUG] file descriptor is %d\n");


  // Write data to file
  // userid
  int header_bytes = write(fd, &userid, sizeof(uid_t));
  // newline
  header_bytes += write(fd, "\n", 1);
  if (header_bytes != 1 + sizeof(uid_t)) {
    fatal("In main() while writing note neader\n");
  }
  // Note data
  if (write(fd, buffer, strlen(buffer)) == -1  //Note data
     || write(fd, "\n", 1) == -1) {              // Terminating newline
    fatal("In main() while writing note to file\n");
  }

  printf("Clearing up resources:\n");
  printf("buffer...\n");
  free(buffer);
  printf(datafile);
  if (close(fd) == -1) {
    fatal("In main() while closing file");
  }
  free(datafile);
  printf("\nDONE!\n");
  return 0;
}


