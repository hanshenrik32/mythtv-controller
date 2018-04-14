#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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


// return string for filename

int get_webfilename(char *fname,char *webpath) {
  char *npointer=NULL;
  npointer=strrchr(webpath,'/');
  if (npointer) {
    strcpy(fname,npointer+1);
    return(1);
  }
  return(0);
}





// web downloader ver .1 by hans-henrik
//
// return hostname ,webpath from source link eks. www.dr.dk/images/image1.png
// return hname   = www.dr.dk
// return webpath = /images/image1.png
//

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


// downloader of files


int get_webfile(char *webpath,char *outfile) {

    struct sockaddr_in servaddr;
    struct hostent *hp;
    int sock_id;
    char message[2024] = {0};
    int msglen;
    char request[1000];
    char *hostpointer;
    char hostname[200];
    char wpath[200];
    char *lpos;
    int webobjlength;
    int i=0;
    bool loaderror=false;
    char tegn;
    FILE *fil;
    int error=0;
    strcpy(hostname,"");

    // return hostname
    //  wpath = text after hostbname (path to file - domainname)
    get_host(hostname,wpath,webpath);	// sample webpage http://www.dr.dk/image

    // http 1.0
//    strcpy(request,"GET ");
//    strcat(request,wpath);
//    strcat(request," HTTP/1.0\r\nFrom: wget!!!\r\nUser-Agent: HTTPTool/1.0\r\n\r\n");


    // http 1.1
    sprintf(request,"GET %s HTTP/1.1\r\nHost: %s\r\n\r\n",wpath,hostname);
    //Get a socket
    if((sock_id = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      fprintf(stderr,"Couldn't get a socket.\n");
      exit(EXIT_FAILURE);
    } else {
      //fprintf(stderr,"Got a socket.  ");
    }

    //book uses bzero which my man pages say is deprecated
    //the man page said to use memset instead. :-)
    memset(&servaddr,0,sizeof(servaddr));

    //get address for google.com
    if((hp = gethostbyname(hostname)) == NULL) {
        fprintf(stderr,"Couldn't get an address.\n");
        return(0);
    }
    else {
        //fprintf(stderr,"Got an address.  ");
    }

    //bcopy is deprecated also, using memcpy instead
    memcpy((char *)&servaddr.sin_addr.s_addr, (char *)hp->h_addr, hp->h_length);

    //fill int port number and type
    servaddr.sin_port = htons(80);
    servaddr.sin_family = AF_INET;

    //make the connection
    if(connect(sock_id, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        fprintf(stderr, "Couldn't connect.\n");
        error=1;
        loaderror=1;
    }
    else {
        //fprintf(stderr,"Got a connection!!!  ");
    }
    if (!(error)) {
      //send the request
      send(sock_id,request,strlen(request),0);
      //write(sock_id,request,strlen(request));
    }

    while((strcmp(message,"\r\n")) && (!(loaderror))) {
      for(i = 0;strcmp(message + i - 2, "\r\n"); i++) {
        read(sock_id,message+i,1);
        message[i+1]=0;
      }
      lpos=strstr(message, "Content-Length:");
      if (strstr(message, "Content-Length:")==message) {
        webobjlength = atoi(strchr(lpos, ' ') + 1);
      }

      if (strstr(message, "Invalid URL")) {
        webobjlength = 0;
        loaderror=true;
      }

      if (strstr(message,"HTTP/1.0 400 Bad Request")) {
        webobjlength = 0;
        loaderror=true;
      }

      if (strstr(message,"302 Found")) {
        webobjlength = 0;
        loaderror=true;
      }

      if (strstr(message,"404 Not Found")) {
        webobjlength = 0;
        loaderror=true;
      }

      if (strstr(message,"Moved Parmanently")) {
        webobjlength = 0;
        loaderror=true;

      }


//      printf("%s \n",message);

    }

    //lpos=strstr(message, "Content-Length:");
    //if (lpos) j = atoi(strchr(lpos, ' ') + 1);

//    printf("get %s length=%d \n",webpath, webobjlength);

    if (!(loaderror)) {
      fil=fopen(outfile,"w");
      if (!(fil)) {
        if (debugmode) fprintf(stderr," Open file for write error %s \n",outfile);
        loaderror=true;							// not posible to save file
//        exit(0);
      }
      if (fil) {
        for (i = 0;i < webobjlength; i++) {
          read(sock_id, &tegn, 1);
          fputc(tegn, fil);
          putchar('\r');
        }
        fclose(fil);
      }
    }
    close(sock_id);
    if (!(loaderror)) return(1); else return(0);
}



int get_webfile2(char *webpath,char *outfile) {
  char command[2048];
  strcpy(command,"wget ");
  strcat(command,webpath);
  strcat(command," -O ");
  strcat(command,outfile);
  system(command);
}
