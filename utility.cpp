#include <stdio.h>
#include <string.h>
#include <sys/types.h>
// used to get home dir
// #include <unistd.h>
#include <pwd.h>
#include <unistd.h>
#include "utility.h"


// get user homedir

int getuserhomedir(char *homedir) {
  struct passwd *pw = getpwuid(getuid());
  strcpy(homedir,pw->pw_dir);
  return(1);
}



bool file_exists(const char * filename)
{
    if (FILE * file = fopen(filename, "r")) {
      fclose(file);
      return true;
    }
    return false;
}

// check file size
// return size
int check_zerro_bytes_file(char *filename) {
  long size;
  FILE *f = fopen(filename, "rb");
  if (f) {
   fseek(f, 0 ,SEEK_END);
   size = ftell(f);
   fclose(f);
  }
  return(size);
}



int clearscreen() {
    gotoxy(1,1);
    for(int i=0;i<100;i++) {
        printf("                                                                                                                                                                             ");
    }
    return(1);
}

int gotoxy(int x, int y) {
	char essq[100];		// String variable to hold the escape sequence
	char xstr[100];		// Strings to hold the x and y coordinates
	char ystr[100];		// Escape sequences must be built with characters

	/*
	** Convert the screen coordinates to strings
	*/
	sprintf(xstr, "%d", x);
	sprintf(ystr, "%d", y);

	/*
	** Build the escape sequence (vertical move)
	*/
	essq[0] = '\0';
	strcat(essq, "\033[");
	strcat(essq, ystr);

	/*
	** Described in man terminfo as vpa=\E[%p1%dd
	** Vertical position absolute
	*/
	strcat(essq, "d");

	/*
	** Horizontal move
	** Horizontal position absolute
	*/
	strcat(essq, "\033[");
	strcat(essq, xstr);
	// Described in man terminfo as hpa=\E[%p1%dG
	strcat(essq, "G");

	/*
	** Execute the escape sequence
	** This will move the cursor to x, y
	*/
	printf("%s", essq);

	return 0;
}
