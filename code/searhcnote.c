#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "common.h"

int find_user_note(int fd, uid_t user_uid) {
  uid_t note_uid = -1;
  unsigned char byte;
  int length;

  while(note_uid != user_uid) { // loop until a note is found
    if (read(fd, &note_uid, sizeof(uid_t)) != sizeof(uid_t)
      ||read(fd, &byte, sizeof(unsigned char)) != sizeof(unsigned char)) {
      // We failed to read the next header so return end of file code
      return -1;
    }
    byte = '\0';
    length = 0;
    // count bytes in current note
    while (byte != '\n') {
      if(read(fd, &byte, 1) != 1) {
        return -1;
      } else {
        ++length;
      }
    }
  }
  off_t rewind = -length;
  if (lseek(fd, rewind, SEEK_CUR) == -1) {
    fatal("Unable to rewind file\n");
  } //Rewind file by length bytes
  printf("[DEBUG] found a %d byte note for user id %d\n", length, note_uid);
  return length;
}

// A function to search a note for a given keyword
// returns 1 if a match is found, 0 if there is no match
int search_note(char *note, char *keyword) {
	int i, keyword_length, match=0;

	keyword_length = strlen(keyword);
	if(keyword_length == 0)  // if there is no search string
		return 1;              // always "match"
	
	for(i=0; i < strlen(note); i++) { // iterate over bytes in note
		if(note[i] == keyword[match])  // if byte matches keyword
			match++;   // get ready to check the next byte
		else {        //   otherwise
			if(note[i] == keyword[0]) // if that byte matches first keyword byte
				match = 1;  // start the match count at 1
			else
				match = 0;  // otherwise it is zero
		}
		if(match == keyword_length) // if there is a full match
			return 1;   // return matched
	}
	return 0;  // return not matched
}


int print_notes(int fd, uid_t user_uid, char *search_str) {
  int note_length;
  char byte=0;
  char note_buffer[100];
  note_length = find_user_note(fd, user_uid);
  if (note_length == -1) {  // EOF signal. No more notes to read.
    return 0;
  }
  int rl;
  read(fd, note_buffer, note_length);
  note_buffer[note_length] = '\0';
  // if searchstring is within note_buffer, print matching note
 	if(search_note(note_buffer, search_str)) {
    printf("[NOTE] ");
		printf(note_buffer);     
  }
  // continue reading 
  return 1;
}


int main(int argc, char *argv[]) {
  uid_t userid = getuid();
  int printing = 1;
  int fd;
  char search_buffer[100];

  if (argc > 1) {
    strcpy(search_buffer, argv[1]);
  } else {
    search_buffer[0] = '\0'; // Null byte - empty string
  }

  fd = open(NOTE_FILE, O_RDONLY);
  if (fd == -1) {
    fatal("Problem opening file in main()");
  }
  while(printing) {
    printing = print_notes(fd, userid, search_buffer);
  }
  printf("--------[ END OF DATA ]--------\n");
  close(fd);

  return 0;
}
