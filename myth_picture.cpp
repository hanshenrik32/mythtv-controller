#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <math.h>
// time
#include <ctime>
#include <sys/timeb.h>


#include "readjpg.h"
#include "myth_picture.h"


extern char configpicturepath[256];
extern int screen_size;
extern GLuint screensaverbox_mask;
extern int orgwinsizex,orgwinsizey;

// ****************************************************************************************
//
// Loadin pictures from folder in homedir
//
// ****************************************************************************************


int picture_saver::dir_loader(char *dirpath) {
    int leng;
    static int picnr=0;
    DIR *dp;
    struct dirent *dirp;
    //char newcheck[1000];
    char dirfile[1000];				// name of file found in dir
    char fil_path[1000];
    printf("Loading pictures ");
    dp  = opendir(dirpath);			// read dirpath
    if (dp) {
      while (((dirp = readdir(dp))!=NULL) && (this->picture_antal<MAX_PICTURES-1)) {
        if ((strcmp(dirp->d_name,".")!=0) && ((dirp->d_name[0]!='.')!=0)  && ((strcmp(dirp->d_name,"..")!=0))) {
          if (dirp->d_type==DT_DIR) {
            strcpy(fil_path,dirpath);
            strcat(fil_path,"/");
            strcat(fil_path,dirp->d_name);
            picnr=picnr+dir_loader(fil_path);
          }
          if (picnr<MAX_PICTURES-1) {
            strcpy(dirfile,dirp->d_name);
            leng=strlen(dirfile);
            if (leng>3) {
             if ((strcmp(&dirfile[leng-4],".jpg")==0) || (strcmp(&dirfile[leng-5],".jpeg")==0) || (strcmp(&dirfile[leng-4],".png")==0)) {
                strcpy(fil_path,dirpath);
                strcat(fil_path,"/");
                strcat(fil_path,dirfile);
                printf(".");
                screensaverpicturepaths[picture_antal]=(char *) malloc(200 * sizeof(char));
                if ((screensaverpicturepaths[picture_antal]) && (this->picture_antal<199)) {
                  strcpy(screensaverpicturepaths[picture_antal],fil_path);		// save path + file to load later
                  picnr++;
                  this->picture_antal++;
                }
              }
            }
          }
        }
      }
      closedir(dp);
    }
    printf(" %d Pictures loaded \n",picture_antal);
    return(picnr);
}

// constructor

picture_saver::picture_saver(void)

{
    this->picture_antal=0;			//
    screensaver_pic=0;
    loaded=false;
}


// ****************************************************************************************
//
// destructor
//
// ****************************************************************************************


picture_saver::~picture_saver()

{
    //free screensaverpicturepaths[0];
    if (screensaver_pic) glDeleteTextures( 1, &screensaver_pic);
}



// ****************************************************************************************
//
// show pictures as screen saver
//
// ****************************************************************************************


void picture_saver::show_pictures()

{
    static time_t now=0;
    static time_t lasttime=0;
    char *homedir;
    char startdir[1000];
    //char startdir[1000];
    static int show_picnr=0;
    static unsigned int timeout=0;
    static float pi=0.0f;
    float xof;
    int winsizx=1920;
    int winsizy=1080;

    if (loaded==false) {
      //homedir = getenv ("HOME");
      //strcpy(startdir,homedir);
      dir_loader("/home/hans/Photos");		// load dir
      loaded=true;
    }
    switch(screen_size) {
        case 1: xof=1024/768;
                break;
        case 2: xof=1280/1024;
                break;
        case 3: xof=1920/1080;
                break;
        case 4: xof=1360/768;
                break;
        default: xof=0.0f;
    }
    //glRotatef(0, 0.0f, 0.0f, 1.0f);
    //glRotatef(0, 0.0f, 1.0f, 0.0f);
    // 10 time load new pic
    if ((picture_antal>0) && (screensaverpicturepaths) && (screensaverpicturepaths[0])) {
        if (!(screensaver_pic)) screensaver_pic=loadTexture(screensaverpicturepaths[0]);
        now=time(NULL);
        if (lasttime+4<now) {
            lasttime=now;
            if (show_picnr>=this->picture_antal) show_picnr=0;
            if (screensaver_pic) glDeleteTextures( 1, &screensaver_pic);				// del last picture loaded
            screensaver_pic=loadTexture(screensaverpicturepaths[show_picnr]);	// load next picture
            show_picnr++;
        }
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glTranslatef(0.0f, 0.0f, 0.0f);
        /*
        glTranslatef(0, 0, -7+sin(pi));
        pi=pi+0.01f;
        if (pi>=(3.14159265f)) pi=0.0f;
        */
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D,screensaver_pic);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2)) , 0.0);
        glTexCoord2f(0, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
        glTexCoord2f(1, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
        glTexCoord2f(1, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2)) , 0.0);
        glEnd();
        glPopMatrix();
    } else printf("No pictures in homedir/Photos directory.\n");
}
