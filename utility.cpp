#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <gtk/gtk.h>
// system_call_timeout.cpp
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
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


// ****************************************************************************************
//
// old do system call use do_system_call_with_timeout() now instead
//
// ****************************************************************************************

bool do_system_call(std::string cmd) {
  char filedata[1025];
  FILE *f = popen(cmd.c_str(), "r");
  fgets(filedata, 1024, f);
  if (!(f)) {
    return false;
  }
  fclose(f);
  return(true);
}


// ****************************************************************************************
//
// do system call with timeout
//
// ****************************************************************************************


bool do_system_call_with_timeout(const std::string& cmd, std::string& output, int timeout_seconds) {
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    return false;
  }
  pid_t pid = fork();
  if (pid == -1) {
    close(pipefd[0]);
  close(pipefd[1]);
  return false;
  }
  if (pid == 0) {
    dup2(pipefd[1], STDOUT_FILENO);
    dup2(pipefd[1], STDERR_FILENO);
    close(pipefd[0]);
    close(pipefd[1]);
    execl("/bin/sh", "sh", "-c", cmd.c_str(), nullptr);
    _exit(127);
  }
  close(pipefd[1]);
  fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
  output.clear();
  char buffer[1024];
  auto start = std::chrono::steady_clock::now();
  bool timed_out = false;
  while (true) {
    ssize_t count = read(pipefd[0], buffer, sizeof(buffer) - 1);
    if (count > 0) {
      buffer[count] = '\0';
      output += buffer;
    }
    int status = 0;
    pid_t result = waitpid(pid, &status, WNOHANG);
    if (result == pid) {
      close(pipefd[0]);
      return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
    if (elapsed.count() >= timeout_seconds) {
      timed_out = true;
      kill(pid, SIGKILL);
      waitpid(pid, nullptr, 0);
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  close(pipefd[0]);
  if (timed_out) {
    output += "\n[Process terminated due to timeout]";
  }
  return false;
}

// ****************************************************************************************
//
// get clipboard text
//
// ****************************************************************************************

std::string getClipboardText() {
  gtk_init(nullptr, nullptr);
  GtkClipboard* clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  gchar* text = gtk_clipboard_wait_for_text(clipboard);
  std::string result;
  if (text) {
    result = text;
    g_free(text);
  }
  return result;
}




// ****************************************************************************************
//
// hent antal elementer i directory dirname
//
// ****************************************************************************************

unsigned int countEntriesInDir(const char* dirname) {
    unsigned int n=0;
    dirent* d;
    DIR* dir = opendir(dirname);
    if (dir == NULL) return 0;
    while((d = readdir(dir))!=NULL) {
      if ((strcmp(d->d_name,".")!=0) && (strcmp(d->d_name,"..")!=0)) n++;
    }
    closedir(dir);
    return n;
}



// ***********************************************************
//
// write to log file in user home dir
//
// ***********************************************************

int write_logfile(FILE *fhandler,char *logentry) {
  char homedirpath[1024];
  char filename[2048];
  char temptxt[4096];
  time_t nutid;
  struct tm *tid;
  struct stat st;
  int filestatus;
  time(&nutid);                                                            // get time
  tid=localtime(&nutid);                                                   // fillout struct
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
  if (fhandler) {
	fprintf(fhandler,"%02d-%02d-%02d %02d:%02d:%02d ",tid->tm_mday,tid->tm_mon+1,tid->tm_year+1900,tid->tm_hour,tid->tm_min,tid->tm_sec);
   	fputs(logentry,fhandler);
   	fputs("\n",fhandler);
	fflush(fhandler);
	fileok=true;
  } else {
    printf("Error write to logfile... %s\n",filename);
  }  
  if (fileok) return(0); else return(1);
}



// get user homedir

int getuserhomedir(char *homedir) {    
  struct passwd *pw = getpwuid(getuid());
  if ((homedir) && (pw)) strcpy(homedir,pw->pw_dir); 
  else strcpy(homedir,""); 
  return(1);
}



bool file_exists(const char * filename) {
	if (strlen(filename)==0) return false;
    if (FILE * file = fopen(filename, "r")) {
      fclose(file);
      return true;
    }
    return false;
}

// check file size
// return size
int check_zerro_bytes_file(char *filename) {
  long size=0;
  FILE *f = fopen(filename, "rb");
  if (f) {
   fseek(f, 0 ,SEEK_END);
   size = ftell(f);
   fclose(f);
  }
  return(size);
}


// ****************************************************************************************
//
// clear screen text mode
//
// ****************************************************************************************

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
// 64bits encoder
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
