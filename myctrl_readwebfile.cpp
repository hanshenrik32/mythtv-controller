#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include <iostream>
#include "myctrl_readwebfile.h"


                                                                // 1  = wifi net
                                                                // 2  = music
                                                                // 4  = stream
                                                                // 8  = keyboard/mouse move
                                                                // 16 = movie
                                                                // 32 = searcg
extern int debugmode;                                           // 64 = radio station land icon loader
                                                                // 128= stream search
                                                                // 256 = tv program stuf
                                                                // 512 = media importer
                                                                // 1024 = flag loader



// ****************************************************************************************
//
// return string for filename long = last path + name as filename
//
// ****************************************************************************************
//

int get_webfilenamelong(char *fname,char *webpath) {
  char *npointer=NULL;
  char *filename;                       // to save filename
  char tmp[20000];
  int firstslashpointer=0;
  strcpy(tmp,webpath);
  npointer=strrchr(tmp,'/');
  if (npointer) {
    firstslashpointer=npointer-tmp;             // husk sted
    filename=new char[strlen(npointer)+1];
    if (filename) {
      strcpy(filename,npointer+1);              // save filename
      tmp[firstslashpointer-1]='\0';
      npointer=strrchr(tmp,'/');
      if (npointer) {
        //strcpy(fname,filename);
        /* old have errors
        strcpy(fname,npointer+1);
        //fname[11]='\0';
        strcat(fname,filename);
        */
        strcpy(fname,filename);
      }
      delete [] filename;
    }
    return(1);
  }
  return(0);
}


// ****************************************************************************************
//
// return string for filename or null
//
// ****************************************************************************************


int get_webfilename(char *fname,char *webpath) {
  char *npointer=NULL;
  npointer=strrchr(webpath,'/');
  if (npointer) {
    strcpy(fname,npointer+1);
    return(1);
  }
  return(0);
}


// ****************************************************************************************
//
// Check if file is a image file of type jpg,png
//
// ****************************************************************************************

bool check_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if ((dot) && ((strcmp(dot,".jpg")==0) || (strcmp(dot,".png")))) return(1); else return(0);
}

// ****************************************************************************************
//
// used to download images from web
// more secure now use wget
//
// ****************************************************************************************

int get_webfile2(char *webpath,char *outfile) {
  char command[2048];
  // check file ext is image yes download
  if ((check_filename_ext(webpath)) && (strlen(webpath)<300)) {
    strcpy(command,"wget \"");
    strcat(command,webpath);
    strcat(command,"\" -O- | convert -thumbnail 'x320^' - - > ");
    strcat(command,outfile);
    //strcat(command," 2>%1 ");
    strcat(command," 2>> wget.log ");
    //if (debugmode & 4) printf(" do COMMAND *%s* \n",command);
    return (system(command));
  }
}


// ***************************************************************************************
//
// download file function for libcurl
//
// ***************************************************************************************


static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}


// ***************************************************************************************
//
// download file
//
// ***************************************************************************************

int convert_file_to_icons(char *outfile) {
    char command[2048];
    char tempname[2048];
    char resultname[2048];
    strcpy(command,"convert -thumbnail 'x320^' ");
    strcat(command,outfile);                                                  //
    strcat(command," > ");
    strcat(command,tmpnam(tempname));
    strcat(command,".jpg");
    system(command);
    try {
      rename(tempname, outfile);
    } catch (...) {
      printf("Error move file %s \n",outfile);
    }
    //return (system(command));
}


// ***************************************************************************************
//
// download file
// call get_webfile(char *webpath,char *outfile)
//
// ***************************************************************************************

int get_webfile(char *webpath,char *outfile) {
    CURL *curl_handle;
    static const char *filename = outfile;
    FILE *file;
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, webpath);
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
    try {
      file = fopen(filename, "wb");
      if (file) {
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, file);
        curl_easy_perform(curl_handle);
        fclose(file);
      }
      curl_easy_cleanup(curl_handle);
      curl_global_cleanup();
    } catch (...) {
      printf("Error downloading file %s \n",outfile);
    }
    convert_file_to_icons(outfile);
    return 0;
}
