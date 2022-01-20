#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include "myctrl_readwebfile.h"


using namespace std;

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
// web downloader ver .1 by hans-henrik
//
// return hostname ,webpath from source link eks. www.dr.dk/images/image1.png
// return hname   = www.dr.dk
// return webpath = /images/image1.png
//
// ****************************************************************************************

void get_host(char *hname,char *webpath,char *source) {
  int n=0;
  int nn=0;
  char tegn[5];
  char tmptxt[1000];
  char tmptxt2[1000];
  strcpy(tmptxt2,"");
  strcpy(tmptxt,source);
  if ((strlen(source)>7) && (strncmp(source,"http://",7)==0)) strcpy(tmptxt,source+7); // remove http(s)://
  else if ((strlen(source)>7) && (strncmp(source,"https://",8)==0)) strcpy(tmptxt,source+8); // remove http(s)://
  else strcpy(tmptxt,"");
  if (strchr(tmptxt,'/')>0) {
    while((tmptxt[n]!='/') && (n<strlen(source))) {
      n++;
    }
    tmptxt[n]=0;
    strcpy(hname,tmptxt);
    while(n<strlen(source)) {
      if (strncmp(source,"http://",7)==0) tmptxt2[nn]=source[n+7];
      else if (strncmp(source,"https://",8)==0) tmptxt2[nn]=source[n+8];
      else tmptxt2[nn]=source[n];
      nn++;
      n++;
    }
    tmptxt2[nn]=0;
    strcpy(webpath,tmptxt2);
  }
}



// ****************************************************************************************
//
// check if file is a image file of type jpg,png
//
// ****************************************************************************************

bool check_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if ((dot) && ((strcmp(dot,".jpg")==0) || (strcmp(dot,".png")))) return(1); else return(0);
}

// ****************************************************************************************
// Old not in use
// used to download images from web and convert by ImageMagick to 320x320
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



// ****************************************************************************************
//
// used by get_webfile3
//
// ****************************************************************************************


size_t file_write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}


// ****************************************************************************************
//
// Used to convert image size
//
// ****************************************************************************************


int convert_image(char *in_image,char *out_image) {
    int result;
    char command[2048];
    strcpy(command,"/usr/bin/convert -thumbnail 'x320^' ");
    strcat(command,in_image);
    strcat (command," -> ");
    strcat(command,out_image);
    result=system(command);
    return result;
}


// ****************************************************************************************
//
// Used to download images from web and convert by ImageMagick to 320x320
//
// download tempfile and convert to the real icon file saved in homedir/rss/images
//
// ****************************************************************************************

int get_webfile(char *webpath,char *outfile) {
  FILE *file;
  std::string response_string;
  CURLcode res;
  CURL *curl;
  char *base64_code;
  char tdestfile[4096];
  char tfilename[4096];
  char errbuf[CURL_ERROR_SIZE];
  strcpy(tdestfile,"");
  tmpnam(tfilename);
  strcat(tdestfile,tfilename);
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, webpath);
    // send data to curl_writeFunction_file
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write_data);
    curl_easy_setopt(curl, CURLOPT_VERBOSE,0L);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); // <-- ssl don't forget this
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0); // <-- ssl and this
    errbuf[0] = 0;
    try {
      file = fopen(tdestfile, "wb");
      if (file) {
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        // get file
        res = curl_easy_perform(curl);
        fclose(file);
      }
      if(res != CURLE_OK) {
        fprintf(stderr, "%s\n", curl_easy_strerror(res));
      }
    }
    catch (...) {
      printf("Error open file.\n");
    }
    curl_easy_cleanup(curl);
    convert_image(tdestfile,outfile);
    remove(tdestfile);                                                          // remove temp file again
  }
  return(1);
}
