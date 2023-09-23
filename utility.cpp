#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>


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
// write to log file in user home dir
//
// ***********************************************************

int write_logfile(char *logentry) {
  char homedirpath[1024];
  char filename[2048];
  char temptxt[4096];
  time_t nutid;
  struct tm *tid;
  struct stat st;
  int filestatus;
  time(&nutid);                                                            // get time
  tid=localtime(&nutid);                                                   // fillout struct
  FILE *logfile;
  bool fileok=false;
  getuserhomedir(homedirpath);
  strcpy(filename,homedirpath);
  strcat(filename,"/");
  strcat(filename,"mythtv-controller.log");
  // check log file size
  // rotate old logfile if size > 200mb    
  filestatus=stat(filename, &st);
  // if file exist
  if ((filestatus==0) && (st.st_size>209715200)) {  
	strcpy(temptxt,"rm ");
	strcat(temptxt,homedirpath);
	strcat(temptxt,"/");
	strcat(temptxt,"mythtv-controller.log ");
	strcat(temptxt,homedirpath);
	strcat(temptxt,"/");
	strcat(temptxt,"mythtv-controller.log.old");
	system(temptxt);
  }
  logfile=fopen(filename,"r");
  if (logfile==NULL) {
    logfile=fopen(filename,"w");
    fputs("                 __  .__     __                                       __                .__  .__                    \n",logfile);
    fputs("  _____ ___.__._/  |_|  |___/  |____  __           ____  ____   _____/  |________  ____ |  | |  |   ___________     \n",logfile);
    fputs(" /     <   |  |\\   __\\  |  \\   __\\  \\/ /  ______ _/ ___\\/  _ \\ /    \\   __\\_  __ \\/  _ \\|  | |  | _/ __ \\_  __\\    \n",logfile);
    fputs("|  Y Y  \\___  | |  | |   Y  \\  |  \\   /  /_____/ \\  \\__(  <_> )   |  \\  |  |  | \\(  <_> )  |_|  |_\\  ___/|  | \\/   \n",logfile);
    fputs("|__|_|  / ____| |__| |___|  /__|   \\_/            \\___  >____/|___|  /__|  |__|   \\____/|____/____/\\___  >__|           \n",logfile);
    fputs("      \\/\\/                \\/                          \\/           \\/                                  \\/          \n",logfile);
    fputs("Ver 0.40.x \n",logfile);
	fileok=true;
	fclose(logfile);
  }
  if (logfile) {
	logfile=fopen(filename,"a");
    if ((tid) && (logfile)) {
		fprintf(logfile,"%02d-%02d-%02d %02d:%02d:%02d ",tid->tm_mday,tid->tm_mon+1,tid->tm_year+1900,tid->tm_hour,tid->tm_min,tid->tm_sec);
    	fputs(logentry,logfile);
    	fputs("\n",logfile);
    	fclose(logfile);
	}
	fileok=true;
  } else {
    printf("Error write to logfile... %s\n",filename);
  }
  if (fileok) return(0); else return(1); 
}


// get user homedir

int getuserhomedir(char *homedir) {  
  printf("**************************** Get users homedir ****************************** \n");
  struct passwd *pw = getpwuid(getuid());
  if (homedir) strcpy(homedir,pw->pw_dir);
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


//
// check if dir is empty
//

bool directory_empty(char *path) {
	unsigned int antal=0;
	struct dirent *dirname=NULL;
	DIR *dirp=NULL;
	dirp=opendir(path);
	if (dirp!=NULL) {
		while(dirname = readdir(dirp)) {
			antal++;
		}
	}
	if (antal>0) return(false); else return(true);
}



// ****************************************************************************************
//
// 64bits incoder
//
// ****************************************************************************************


size_t b64_encoded_size(size_t inlen) {
	size_t ret;
	ret = inlen;
	if (inlen % 3 != 0)
		ret += 3 - (inlen % 3);
	ret /= 3;
	ret *= 4;
	return ret;
}


// ****************************************************************************************
//
// 64bits incoder
//
// ****************************************************************************************


char *b64_encode(const unsigned char *in, size_t len) {
  const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	char   *out;
	size_t  elen;
	size_t  i;
	size_t  j;
	size_t  v;
	if (in == NULL || len == 0)
		return NULL;
	elen = b64_encoded_size(len);
	out  = (char *) malloc(elen+1);
	out[elen] = '\0';
	for (i=0, j=0; i<len; i+=3, j+=4) {
		v = in[i];
		v = i+1 < len ? v << 8 | in[i+1] : v << 8;
		v = i+2 < len ? v << 8 | in[i+2] : v << 8;
		out[j]   = b64chars[(v >> 18) & 0x3F];
		out[j+1] = b64chars[(v >> 12) & 0x3F];
		if (i+1 < len) {
			out[j+2] = b64chars[(v >> 6) & 0x3F];
		} else {
			out[j+2] = '=';
		}
		if (i+2 < len) {
			out[j+3] = b64chars[v & 0x3F];
		} else {
			out[j+3] = '=';
		}
	}
	return out;
}
