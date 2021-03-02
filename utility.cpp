#include <stdio.h>
#include <string.h>
#include <sys/types.h>
// used to get home dir
// #include <unistd.h>
#include <pwd.h>
#include <unistd.h>
#include "utility.h"
#include <time.h>                       // have strftime

unsigned int hourtounixtime(int hour) {
  time_t nutid;
  struct tm *tid;
  time(&nutid);                                                            // get time
  tid=localtime(&nutid);                                                   // fillout struct
  tid->tm_min=0;
  tid->tm_hour=hour;
  return(mktime(tid));
}

// ***********************************************************
//
// write to log file
//
// ***********************************************************

void write_logfile(char *logentry) {
  char homedirpath[1024];
  char filename[2048];
  time_t nutid;
  struct tm *tid;
  time(&nutid);                                                            // get time
  tid=localtime(&nutid);                                                   // fillout struct
  FILE *logfile;
  getuserhomedir(homedirpath);
  strcpy(filename,homedirpath);
  strcat(filename,"/");
  strcat(filename,"mythtv-controller.log");
  logfile=fopen(filename,"a");
  if (logfile==NULL) logfile=fopen("mythtv-controller.log","w");
  if (logfile) {
    fprintf(logfile,"%02d/%02d/%02d %02d:%02d:%02d ",tid->tm_mday,tid->tm_mon+1,tid->tm_year+1900,tid->tm_hour,tid->tm_min,tid->tm_sec);
    fputs(logentry,logfile);
    fputs("\n",logfile);
    fclose(logfile);
  }
}


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
