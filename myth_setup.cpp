#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string.h>
#include <stdarg.h>

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <X11/Intrinsic.h>    /* Display, Window */
#include <GL/glx.h>           /* GLXContext */

#include <GL/glc.h>                     // glc true type font system

#include <libxml/parser.h>

// mysql support

#include <mysql.h>
#include "text3d.h"
#include "readjpg.h"
#include "myth_ttffont.h"
#include "myth_setup.h"
#include "checknet.h"

extern int configuvmeter;
extern int debugmode;

struct configkeytype {
    char cmdname[200];
    unsigned int scrnr;
};
extern int PRGLIST_ANTAL;
extern bool global_use_internal_music_loader_system;
extern bool full_screen;
extern char configmythsoundsystem[256];
extern fontctrl aktivfont;
extern char configmysqluser[256];               // mysql username
extern char configmysqlpass[256];               // mysql pass
extern char configmysqlhost[256];               // mysql host name
extern char configxbmcuser[256];                // xbmc access info
extern char configxbmcpass[256];                //
extern char configxbmchost[256];                //
extern char confighostname[256];                // host name
extern char configbackend[20];					        // backend system xbmc/mythtv
extern char confighostip[256];				        	// mysql host ip adress
extern char confighostwlanname[256];				    // wlan name id
extern char configpicturepath[256];             // path til pictures gallery source
extern char configmusicpath[256];               // path til music source (found in sqlbase/mythtvbackend)
extern char configmoviepath[256];				        // path til movie source (found in sqlbase/mythtvbackend)
extern char configrecordpath[256];	       			// path til recorded source (found in sqlbase/mythtvbackend)
extern char configdeviceid[256];		         		// default music player device
extern char configscreensavertimeout[256];			// screen saver time out i sekunder
extern char keybuffer[512];                     // keyboard buffer
extern char configclosemythtvfrontend[256];			// close mythtvfront end on startup
extern char configaktivescreensavername[256];		// screen saver name
extern char configsoundoutport[256];				    // sound output port (hdmi/spdif/analog)
extern char configfontname[200];				        // default ttf font name to load and use
extern long configtvguidelastupdate;            // last xmltv update
extern char configbackend_tvgraber[256];        // internal tv graber to use
extern char configbackend_tvgraber_path[256];   // internal tv graber to use
extern configkeytype configkeyslayout[12];			// functions keys startfunc
extern char configuse3deffect[20];
// extern char configvideoplayer[200];             // default video player
extern char configdefaultplayer[200];
extern int configdefaultplayer_screenmode;
extern int configland;
extern char *configlandsprog[];
extern int configxbmcver;
extern GLuint _texturesetupclose;
extern GLuint setuptexture;
extern GLuint _texturesoundsetup;
extern GLuint _texturesourcesetup;
extern GLuint _textureimagesetup;
extern GLuint _texturetemasetup;
extern GLuint _texturesetupfont;
extern GLuint _texturekeyssetup;
extern GLuint _texturelock;			                  // en hænge lås
extern GLuint setupkeysbar1;
extern GLuint setupkeysbar2;
extern GLuint setupsoundback;
extern GLuint setupsqlback;
extern GLuint setupnetworkback;
extern GLuint setupnetworkwlanback;
extern GLuint setupscreenback;
extern GLuint setuptemaback;                        //
extern GLuint setupfontback;                        //
extern GLuint setupkeysback;                        //
extern GLuint _texturevideoplayersetup;             // setup
extern GLuint _texturemythtvsql;
extern GLuint _textureclose;
extern GLuint _texturetvgrabersetup;            //
extern GLuint screenshot1,screenshot2,screenshot3,screenshot4,screenshot5,screenshot6,screenshot7,screenshot8,screenshot9,screenshot10;
extern unsigned int do_show_setup_select_linie;
extern int tema;
extern int screen_size;
extern int configmythtvver;
extern wifinetdef wifinets;
extern unsigned int setupwlanselectofset;             // valgte wlan nr i oversigt
extern int orgwinsizey;                               // screen size
extern int orgwinsizex;                               //
static bool fontselected=false;
//static int fontnr=0;


const GLfloat selectcolor[3]={1.0f,1.0f,0.0f};		// text select color

extern channel_list_struct channel_list[];                             // channel_list array used in setup graber

// Denne som bruges

void myglprint4(char *string)
{
    int len,i;
    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
    }
}

void myglprint5(char *string)
{
    int len,i;
    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
    }
}


void stroke_output2(GLfloat x, GLfloat y, char *format,...) {
    va_list args;
    char buffer[200], *p;

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glLineWidth(2.0);
    glScalef(0.003, 0.003, 0.003);
    for (p = buffer; *p; p++) glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
        glPopMatrix();
}



// draw cursor on screen at pos

void showcoursornow(int cxpos,int cypos,int txtlength) {
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  cxpos+=4+(txtlength*9);
  cypos+=6;
  struct timespec timer;
  long lasttimer=0;
  static bool showcursor=false;
  clock_gettime(CLOCK_REALTIME, &timer);
  if (timer.tv_nsec>lasttimer+243600692) showcursor=true; else showcursor=false;
  if (showcursor) {
    lasttimer=timer.tv_nsec;
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(cxpos+((orgwinsizex/2)-(1200/2))+6,cypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(cxpos+((orgwinsizex/2)-(1200/2))+6,cypos+((orgwinsizey/2)-(800/2))+20, 0.0);
    glTexCoord2f(1, 1); glVertex3f(cxpos+((orgwinsizex/2)-(1200/2))+14,cypos+((orgwinsizey/2)-(800/2))+20 , 0.0);
    glTexCoord2f(1, 0); glVertex3f(cxpos+((orgwinsizex/2)-(1200/2))+14,cypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
  }
  glPopMatrix();

}



// ************************************************************** setup video config *************************

void show_setup_screen() {
    char resl[1024];
    int winsizx=1200;
    int winsizy=800;
    int xpos=0;
    int ypos=0;

    // background
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupscreenback);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2)) , 0.0);
    glEnd();
    glPopMatrix();


    glPushMatrix();
    // close buttons
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D,_textureclose);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    winsizx=100;
    winsizy=100;
    xpos=550;
    ypos=50;
    glLoadName(40);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

    glPushMatrix();
    winsizx=300;
    winsizy=40;
    xpos=250;
    ypos=600;
    // here start input
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    glPushMatrix();
    winsizx=300;
    winsizy=40;
    xpos=250;
    ypos=550;
    // here start input
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

    // screen saver time out
    glPushMatrix();
    winsizx=50;
    winsizy=40;
    xpos=250;
    ypos=500;
    // here start input
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    //
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

    // use 3d
    glPushMatrix();
    winsizx=40;
    winsizy=40;
    xpos=250;
    ypos=450;
    // here start input
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    //
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

    glPushMatrix();
    winsizx=200;
    winsizy=40;
    xpos=250;
    ypos=400;
    // here start input
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    //
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    // full screen box
    glPushMatrix();
    winsizx=40;
    winsizy=40;
    xpos=250;
    ypos=350;
    // here start input
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    //
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();



    glPushMatrix();
    switch(screen_size) {
        case 1: sprintf(resl,"1024 x 768 (720p)  ");
                break;
        case 2: sprintf(resl,"1280 x 1024 (720p) ");
                break;
        case 3: sprintf(resl,"1920 x 1080 (1080p)");
                break;
        case 4: sprintf(resl,"1360 x 768         ");
                break;
        default:sprintf(resl,"1024 x 768 (720p)  ");
                break;
    }
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(380.0f, 750.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Video mode                ");
    if (do_show_setup_select_linie==0) {
        glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
        myglprint4((char *) resl);
    } else {
        myglprint4((char *) resl);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==0) showcoursornow(250,600,strlen(resl));

    glPushMatrix();
    // show close on exit type
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(380.0f, 700.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Screen saver             ");
    myglprint4((char *) " ");
    if (do_show_setup_select_linie==1) {

        strcpy(keybuffer,configaktivescreensavername);

        glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
        myglprint4((char *) keybuffer);

    } else {
        myglprint4((char *) configaktivescreensavername);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==1) showcoursornow(250,550,strlen(configaktivescreensavername));


    glPushMatrix();
    // show close on exit type
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(380.0f, 650.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Saver timeout            ");
    myglprint4((char *) " ");
    if (do_show_setup_select_linie==2) {
        strcpy(keybuffer,configscreensavertimeout);
        glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
//        glRasterPos2f(2.2f, 0.0f);
        myglprint4((char *) keybuffer);
    } else {
        myglprint4((char *) configscreensavertimeout);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==2) showcoursornow(250,500,strlen(configscreensavertimeout));

    glPushMatrix();
    // show close on exit type
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(380.0f, 600.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Use 3D                   ");
    myglprint4((char *) " ");
    if (do_show_setup_select_linie==3) {
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
//        glRasterPos2f(2.2f, 0.0f);
      if (strcmp(configuse3deffect,"yes")==0) myglprint4((char *) "yes"); else  myglprint4((char *) "no ");
    } else {
      if (strcmp(configuse3deffect,"yes")==0) myglprint4((char *) "yes"); else  myglprint4((char *) "no ");
    }

    if (do_show_setup_select_linie==3) showcoursornow(250,450,strlen("yes"));

    glPopMatrix();

    glPushMatrix();
    // show sprog valg
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(380.0f, 550.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Language                 ");
    myglprint4((char *) " ");
    if (do_show_setup_select_linie==4) {
      switch(configland) {
        case 0: strcpy(keybuffer,"English");
        break;
        case 1: strcpy(keybuffer,"Dansk");
        break;
        case 2: strcpy(keybuffer,"France");
        break;
        case 3: strcpy(keybuffer,"Tysk");
        break;
        case 4: strcpy(keybuffer,"Arabi");
        break;
        default: strcpy(keybuffer,"English");
        break;
      }
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      myglprint4((char *) keybuffer);
    } else {
      if ((configland>=0) && (configland<5)) {
	       switch(configland) {
	          case 0: strcpy(keybuffer,"English");
	             break;
	          case 1: strcpy(keybuffer,"Dansk");
	             break;
	          case 2: strcpy(keybuffer,"France");
	             break;
	          case 3: strcpy(keybuffer,"Tysk");
	             break;
	          case 4: strcpy(keybuffer,"Arabi");
	             break;
	          default: strcpy(keybuffer,"English");
	             break;
	       }
         myglprint4(keybuffer);
      }
    }
    glPopMatrix();

    if (do_show_setup_select_linie==4) showcoursornow(250,400,strlen(keybuffer));


    glPushMatrix();
    // show close on exit type
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(380.0f, 500.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Full screen mode          ");
    if (do_show_setup_select_linie==5) {
      if (full_screen) strcpy(keybuffer,"yes"); else strcpy(keybuffer,"no ");
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      myglprint4((char *) keybuffer);
    } else {
      if (full_screen) myglprint4((char *) "yes"); else myglprint4((char *) "no ");
      if (full_screen) strcpy(keybuffer,"yes"); else strcpy(keybuffer,"no ");
    }
    glPopMatrix();

    if (do_show_setup_select_linie==5) showcoursornow(250,350,strlen(keybuffer));

    glPushMatrix();
//    glScalef(25.0f, 25.0f, 1.00f);
    sprintf(resl,"Hardware %s",glGetString(GL_RENDERER));
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(380.0f, 450.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint5((char *) resl);
    glRasterPos2f(0.0f, -20.0f);
    sprintf(resl," Render   %s",glGetString(GL_VENDOR));
    myglprint5((char *) resl);
    glRasterPos2f(0.0f, -40.0f);
    sprintf(resl," Version  %s",glGetString(GL_VERSION));
    myglprint5((char *) resl);
    glPopMatrix();


}


//
// ************************************************************** setup video config *************************
// play options menu

void show_setup_video() {
    char resl[1024];
    int winsizx=1200;
    int winsizy=600;
    int xpos=0;
    int ypos=0;

    // background
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupscreenback);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2)) , 0.0);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(380.0f, 800.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Video play options.");   // keybuffer
    glPopMatrix();



    glPushMatrix();
    // close buttons
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D,_textureclose);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    winsizx=100;
    winsizy=100;
    xpos=550;
    ypos=150;
    glLoadName(40);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();



    glPushMatrix();
    winsizx=300;
    winsizy=40;
    xpos=250;
    ypos=600;
    // here start input
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    glPushMatrix();
    winsizx=300;
    winsizy=40;
    xpos=250;
    ypos=550;
    // here start input
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(380.0f, 750.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Videoplayer                ");
    if (do_show_setup_select_linie==0) {
        glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
        myglprint4((char *) configdefaultplayer);
    } else {
        myglprint4((char *) configdefaultplayer);
    }
    glPopMatrix();

    // show cursor
    if (do_show_setup_select_linie==0) showcoursornow(250,600,strlen(configdefaultplayer));

    glPushMatrix();
    // show close on exit type
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(380.0f, 700.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Player resolution         ");
    myglprint4((char *) " ");

    switch(configdefaultplayer_screenmode) {
        case 1: sprintf(resl,"1024 x 768 (720p)  ");
                break;
        case 2: sprintf(resl,"1280 x 1024 (720p) ");
                break;
        case 3: sprintf(resl,"1920 x 1080 (1080p)");
                break;
        case 4: sprintf(resl,"1360 x 768         ");
                break;
        default:sprintf(resl,"1024 x 768 (720p)  ");
                break;
    }

    if (do_show_setup_select_linie==1) {
        glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
        myglprint4((char *) resl);
    } else {
        myglprint4((char *) resl);
    }
    glPopMatrix();


    // debug mode
    glPushMatrix();
    winsizx=100;
    winsizy=40;
    xpos=250;
    ypos=500;
    // here start input
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    // show close on exit type
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(380.0f, 650.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Debug mode               ");
    myglprint4((char *) " ");

    if (debugmode==2) sprintf(resl,"Music");
    else if (debugmode==4) sprintf(resl,"radio");
    else if (debugmode==8) sprintf(resl,"Keyboard/mouse");
    else if (debugmode==16) sprintf(resl,"Movie");
    else if (debugmode==32) sprintf(resl,"Not def");
    else if (debugmode==64) sprintf(resl,"Not def");
    else if (debugmode==128) sprintf(resl,"Stream");
    else if (debugmode==256) sprintf(resl,"TV guide stuf");
    else if (debugmode==512) sprintf(resl,"media importer");
    else sprintf(resl,"%d",debugmode);

    if (do_show_setup_select_linie==2) {
        glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
        myglprint4((char *) resl);
    } else {
        myglprint4((char *) resl);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==1) showcoursornow(250,550,strlen(resl));

    glPushMatrix();
    winsizx=300;
    winsizy=40;
    xpos=250;
    ypos=450;
    // here start input
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    // show uv mode
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(380.0f, 600.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "UV Meter mode             ");

    if (configuvmeter==0) strcpy(resl,"None");
    if (configuvmeter==1) strcpy(resl,"Simple");
    if (configuvmeter==2) strcpy(resl,"Dual");
    if (configuvmeter>2) strcpy(resl,"None");

    sprintf(resl,"%d",configuvmeter);
    if (do_show_setup_select_linie==3) {
        glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
        myglprint4((char *) resl);
    } else {
        myglprint4((char *) resl);
    }
    glPopMatrix();

    // show cursor
    if (do_show_setup_select_linie==2) showcoursornow(250,500,strlen(resl));

//    if (do_show_setup_select_linie==1) showcoursornow(250,550,strlen(configaktivescreensavername));


}







// ************************************************************** setup tema config *************************

void show_setup_tema() {
//    int aktivtema=1;
    // mask
    char temptxt[80];
    int winsizx=100;
    int winsizy=200;
    int xpos=0;
    int ypos=0;

    // background
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupscreenback);                   //setuptemaback);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),200 , 0.0);
    glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
    glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,200 , 0.0);
    glEnd();
    glPopMatrix();


    glPushMatrix();
    // show tema
    winsizx=550;
    winsizy=350;
    xpos=250;
    ypos=220;
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    switch(tema) {
        case 1:	glBindTexture(GL_TEXTURE_2D,screenshot1);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        case 2:	glBindTexture(GL_TEXTURE_2D,screenshot2);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        case 3:	glBindTexture(GL_TEXTURE_2D,screenshot3);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        case 4:	glBindTexture(GL_TEXTURE_2D,screenshot4);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        case 5:	glBindTexture(GL_TEXTURE_2D,screenshot5);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        case 6:	glBindTexture(GL_TEXTURE_2D,screenshot6);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        case 7:	glBindTexture(GL_TEXTURE_2D,screenshot7);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        case 8:	glBindTexture(GL_TEXTURE_2D,screenshot8);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        case 9:	glBindTexture(GL_TEXTURE_2D,screenshot9);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        case 10:glBindTexture(GL_TEXTURE_2D,screenshot10);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        default:
                glBindTexture(GL_TEXTURE_2D,screenshot10);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glLoadName(41);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();



//    glLoadIdentity();
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(840 , 740 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glScalef(25.0f, 25.0f, 1.00f);
    sprintf(temptxt,"Tema:%d ",tema);
    glcRenderString(temptxt);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();


    glPushMatrix();
    // close buttons
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D,_textureclose);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    winsizx=100;
    winsizy=100;
    xpos=450;
    ypos=100;
    glLoadName(40);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();



}



// ************************************************ setup font ****************************

void show_setup_font(int startofset) {
    int i;
    // mask
    char temptxt[200];
    int winsizx=100;
    int winsizy=200;
    int xpos=0;
    int ypos=0;


    // background
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupfontback);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),200 , 0.0);
    glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
    glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,200 , 0.0);
    glEnd();
    glPopMatrix();


    for (i=0;i<8;i++) {
      if (i==4) {
          winsizx=600;
          winsizy=20;
          xpos=180;
          ypos=360;
          glDisable(GL_BLEND);
          glBlendFunc(GL_ONE, GL_ONE);
          glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
          //glBindTexture(GL_TEXTURE_2D,setupfontback);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glBegin(GL_QUADS); //Begin quadrilateral coordinates
          glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
          glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
          glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
          glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
          glEnd(); //End quadrilateral coordinates
      }
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(540 , 600-(i*20) , 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(25.0f, 25.0f, 1.00f);
      if ((i+startofset)-3>=0) strcpy(temptxt,(char *) aktivfont.typeinfo[(i+startofset)-3].fontname);
      else sprintf(temptxt,"                                        ");
      //sprintf(temptxt,"Tema:%d ",tema);
      glcRenderString(temptxt);
        glEnable(GL_TEXTURE_2D);
      glPopMatrix();
    }
/*

    // select selected font
    if ((startofset>0) && (fontselected==false)) {
        fontselected=true;
        printf("Select font %s \n",aktivfont.typeinfo[startofset].fontname);
        fontnr=startofset;
        aktivfont.selectfont(aktivfont.typeinfo[startofset].fontname);

//        glLoadIdentity();
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(400.0f, 200.f, 0.0f);
        glRasterPos2f(0.0f, 0.0f);

        glScalef(25.0f, 25.0f, 1.00f);
        glcRenderString("Sample");
        //myglprint4((char *) "This is a demo of the font");
         // restore select font
        //    aktivfont.selectfont(configfontname);

    } else {

//        glLoadIdentity();
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(200.0f, 200.0f, 0.0f);
        glRasterPos2f(0.0f, 0.0f);
        glScalef(25.0f, 25.0f, 1.00f);
        glcRenderString("Sample");

    }

    if ((strcmp(aktivfont.typeinfo[startofset].fontname,configfontname)!=0) && (fontnr!=startofset)) {
        fontselected=false;
    }


*/
    glPushMatrix();
    // close buttons
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D,_textureclose);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    winsizx=100;
    winsizy=100;
    xpos=490;
    ypos=70;
    glLoadName(40);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

}






// ****************************************************** network *************************

void show_wlan_networks(int valgtnr) {
    int i;
    int si;
    char tmptxt[80];

    int winsizx=100;
    int winsizy=200;
    int xpos=0;
    int ypos=0;

    // background
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupnetworkwlanback);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),200 , 0.0);
    glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
    glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,200 , 0.0);
    glEnd();
    glPopMatrix();


    winsizx=200;
    winsizy=30;
    xpos=300;
    ypos=400;
    i=0;
    while ((i<wifinets.antal()) && (i<12)) {
        if (i==valgtnr) {
            glPushMatrix();
            //glBindTexture(GL_TEXTURE_2D,0);
            //glTranslatef(-1.4f, 2.4f-(valgtnr*0.4f),-14.8f);
            glTranslatef(600.0f, 400.0f, 0.0f);
            glRasterPos2f(0.0f, 0.0f+(i*20.0f));
            glColor3f(0.0f, 0.0f, 1.0f);
            glBegin(GL_QUADS); //Begin quadrilateral coordinates
            glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
            glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
            glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
            glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
            glEnd(); //End quadrilateral coordinates
            glPopMatrix();
        }

        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(600.0f, 400.0f, 0.0f);
        glRasterPos2f(0.0f, 0.0f+(i*20.0f));
        //glTranslatef(-6.5f, 1.5f,-14.8f);
        //glRasterPos2f(2.2f, 0.8f-(i*0.4f));
        wifinets.get_networkid(i,tmptxt);        	// get wlan name
        myglprint4((char *) tmptxt);
        glPopMatrix();

        glPushMatrix();
        wifinets.get_networksign(i,&si);        	// get wlan signal styrke
        sprintf(tmptxt,"%d db",si);
        //glRasterPos2f(6.0f, 0.8-(i*0.4f));
        glTranslatef(600.0f, 400.0f, 0.0f);
        glRasterPos2f(250.0f, 0.0f+(i*20.0f));
        myglprint4((char *) tmptxt);
        glPopMatrix();

        winsizx=20;
        winsizy=20;
        xpos=0;
        ypos=0;
        if (wifinets.get_encryptstatus(i)==true) {
            //glLoadIdentity();
            glPushMatrix();
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,_texturelock);
            //glTranslatef(1.2f, 2.4f-(i*0.4f),-14.8f);
            glTranslatef(550.0f, 260.0f+(i*20), 0.0f);
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_QUADS); //Begin quadrilateral coordinates
            glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
            glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
            glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
            glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
            glEnd(); //End quadrilateral coordinates
            glPopMatrix();
        }
        i++;
    }
    if (wifinets.antal()==0) {
      glDisable(GL_TEXTURE_2D);
      myglprint4((char *) "No wifi networks avable.");
    }


    glPushMatrix();
    // close buttons
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);
//    glTranslatef(0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D,_textureclose);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    winsizx=100;
    winsizy=100;
    xpos=450;
    ypos=80;
    glLoadName(40);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

}





// ************** network setup window *****************************************************

void show_setup_network() {
    char tmptxt[1024];
    // mask
    char id[80];			// wlan id
    int si;				// wlan signal
    int winsizx=100;
    int winsizy=200;
    int xpos=0;
    int ypos=0;


    // background
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupnetworkback);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),200 , 0.0);
    glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
    glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,200 , 0.0);
    glEnd();
    glPopMatrix();


    glPushMatrix();
    // close buttons
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D,_textureclose);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    winsizx=100;
    winsizy=100;
    xpos=450;
    ypos=80;
    glLoadName(40);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    // text input background
    glPushMatrix();
    winsizx=200;
    winsizy=40;
    xpos=450;
    ypos=500;
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
//    glTranslatef(-3.6f, 2.1f-0.5f,-14.8f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    // text input background
    glPushMatrix();
    winsizx=200;
    winsizy=40;
    xpos=450;
    ypos=450;
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f,1.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

    glPushMatrix();
    winsizx=200;
    winsizy=40;
    xpos=450;
    ypos=400;
    // text input background
//    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f,1.0f, 1.0f);
//    glTranslatef(-3.6f, 2.1f-1.5f,-14.8f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    glPushMatrix();
    winsizx=200;
    winsizy=40;
    xpos=450;
    ypos=350;    // text input background wlan essid's password
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f,1.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();



    glPushMatrix();
    // Buttons
//    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f,1.0f, 1.0f);
//    glTranslatef(-3.6f+3.0f, 2.1f-1.5f,-14.8f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glLoadName(41);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.6, -0.2f, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.6,  0.2f, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f( 0.8,  0.2f, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f( 0.8, -0.2f, 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glTranslatef(800,650, 0.0f);
    glRasterPos2f(20.0f, 0.0f);
    if (do_show_setup_select_linie==3) glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) confighostname);
    glPopMatrix();

    glPushMatrix();
    // here start input
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(700,650, 0.0f);
    glRasterPos2f(0.0f, 0.0f);

    myglprint4((char *) "Hostname ");
    glRasterPos2f(120.0f, 0.0f);
    if (do_show_setup_select_linie==0) {
//        glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
        glColor3f(0.5f,0.5f,0.5f);
        myglprint4((char *) keybuffer);
    } else {
        myglprint4((char *) confighostname);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==0) showcoursornow(450,500,strlen(confighostname));


//    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(700,600, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "IP adress");
    glRasterPos2f(120.0f, 0.0f);
    if (do_show_setup_select_linie==1) {
      strcpy(keybuffer,confighostip);
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      myglprint4((char *) keybuffer);
    } else {
        myglprint4((char *) confighostip);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==1) showcoursornow(450,450,strlen(confighostip));

    glPushMatrix();
    //  show wlan network id
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(700,550, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "WLAN");
    glRasterPos2f(120.0f, 0.0f);
    wifinets.get_networkid(setupwlanselectofset,id);
    if (do_show_setup_select_linie==2) {
      strcpy(keybuffer,confighostwlanname);
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      myglprint4((char *) keybuffer);
    } else {
      myglprint4((char *) confighostwlanname);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==2) showcoursornow(450,400,strlen(confighostwlanname));

    // get wifi signal db
    glPushMatrix();

    wifinets.get_networksign(setupwlanselectofset,&si);
    sprintf(tmptxt,"%d db",si);
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    glTranslatef(1020,550, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Signal");
    glRasterPos2f(60.0f, 0.0f);
    myglprint4((char *) tmptxt);
    glPopMatrix();



    glPushMatrix();
    //  show wlan network password
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(700,500, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Password");

    wifinets.get_wifipass(setupwlanselectofset,tmptxt);			// get password for aktive wlan
    if (wifinets.antal()>0) strcpy(tmptxt,""); else {
      // no wifi network avable
      strcpy(tmptxt,"No wifi");
      if (do_show_setup_select_linie==3) strcpy(keybuffer,"No wifi");
    }
    glRasterPos2f(120.0f, 0.0f);
    if (do_show_setup_select_linie==3) {
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      myglprint4((char *) keybuffer);
    } else {
      myglprint4((char *) tmptxt);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==3) showcoursornow(450,350,strlen(tmptxt));

}





// ******************************************* setup sound config **********************************************

void show_setup_sound() {
    // mask
    int winsizx=100;
    int winsizy=200;
    int xpos=0;
    int ypos=0;

    // background
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupsoundback);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),200 , 0.0);
    glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
    glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,200 , 0.0);
    glEnd();
    glPopMatrix();


    glPushMatrix();
    // close buttons
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D,_textureclose);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    winsizx=100;
    winsizy=100;
    xpos=450;
    ypos=150;
    glLoadName(40);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    glPushMatrix();
    // text input background
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
//    glTranslatef(-2.0f, 2.1f+0.0f,-14.8f);
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=500;

    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    glPushMatrix();
    // text input background
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
//    glTranslatef(-2.0f, 2.1f-0.5f,-14.8f);
    winsizx=200;
    winsizy=30;
    xpos=300;
    ypos=450;
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    glPushMatrix();
    // text input background
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    //    glTranslatef(-2.0f, 2.1f-0.5f,-14.8f);
    winsizx=200;
    winsizy=30;
    xpos=300;
    ypos=400;
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    glPushMatrix();
    // show sound system in use
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(500.0f, 650.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Output device");
    glRasterPos2f(40.0f, 0.0f);
    myglprint4((char *) ":");
    if (do_show_setup_select_linie==0) {
      strcpy(keybuffer,configmythsoundsystem);
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      glRasterPos2f(160.0f, 0.0f);
      myglprint4((char *) keybuffer);
    } else {
      glRasterPos2f(160.0f, 0.0f);
      myglprint4((char *) configmythsoundsystem);
    }
    glPopMatrix();


    glPushMatrix();
    // show sound output device
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(500.0f, 600.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Output");
    glRasterPos2f(30.0f, 0.0f);
    myglprint4((char *) ":");
    glRasterPos2f(160.0f, 0.0f);
    if (do_show_setup_select_linie==1) {
      strcpy(keybuffer,configsoundoutport);
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      glRasterPos2f(160.0f, 0.0f);
      myglprint4((char *) keybuffer);
    } else {
      myglprint4((char *) configsoundoutport);
    }
    glPopMatrix();

    glPushMatrix();
    // show sound system
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(500.0f, 550.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Sound system");
    glRasterPos2f(30.0f, 0.0f);
    myglprint4((char *) ":");
    glRasterPos2f(160.0f, 0.0f);
    if (do_show_setup_select_linie==2) {
      strcpy(keybuffer,"FMOD");
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      glRasterPos2f(160.0f, 0.0f);
      myglprint4((char *) keybuffer);
    } else {
      myglprint4((char *) "FMOD");
    }
    glPopMatrix();

}



//
// *********************************** setup sql database info ***********************************************
// db mythtv or xbmc ok
//

void show_setup_sql() {
    char text[200];
    float mythver;
    // mask
    int winsizx=100;
    int winsizy=200;
    int xpos=0;
    int ypos=0;

    // background
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,setupsqlback);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),100 , 0.0);
    glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
    glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,100 , 0.0);
    glEnd();
    glPopMatrix();


    glPushMatrix();
    // close buttons
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D,_textureclose);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    winsizx=100;
    winsizy=100;
    xpos=450;
    ypos=0;
    glLoadName(40);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    // text input background
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    winsizx=200;
    winsizy=30;
    xpos=300;
    ypos=400;
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates


    // text input background
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    winsizx=200;
    winsizy=30;
    xpos=300;
    ypos=350;
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates

    // text input background
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    winsizx=200;
    winsizy=30;
    xpos=300;
    ypos=300;
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates



    // text input background
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=250;
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates

    // text input background
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=200;
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates

    // text input background
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=150;
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates

    // text input background
//    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
//    glTranslatef(-2.0f, 2.1f-2.5f,-14.8f);
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=100;
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    glBegin(GL_QUADS); //Begin quadrilateral coordinates

    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates

    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    if (strcmp(configbackend,"mythtv")==0) {
      if (configmythtvver==1264) mythver=0.24f;
      if (configmythtvver) {
        sprintf(text,"Mythtv version   : %2.2f - Database ver : %d",mythver,configmythtvver);
      } else {
        if (global_use_internal_music_loader_system) sprintf(text,"No mythtv backend found. Internal db in use."); else sprintf(text,"Mythtv/Database config error no connection.");
      }
      glTranslatef(500.0f, 650.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      myglprint4((char *) text);
    } else if (strcmp(configbackend,"xbmc")==0) {
      if (configxbmcver) {
        if (configxbmcver==75) sprintf(text,"XBMC version     : 12 found"); else sprintf(text,"XBMC version     : %d found",configxbmcver);
      } else sprintf(text,"NO XBMC version found");
      glTranslatef(500.0f, 650.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      myglprint4((char *) text);
    }


    // here start input
    glPushMatrix();
    glTranslatef(10.0f, -50.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "mythtv/xbmc");
    glRasterPos2f(140.0f, 0.0f);
    myglprint4((char *) ":");
    if (do_show_setup_select_linie==0) {
        strcpy(keybuffer,configbackend);
        glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
        glRasterPos2f(160.0f, 0.0f);
        myglprint4((char *) keybuffer);
    } else {
        glColor3f(1.0f,1.0f,1.0f);
        glRasterPos2f(160.0f, 0.0f);
        myglprint4((char *) configbackend);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==0) showcoursornow(-200,-200,strlen(configbackend));
    // here start input
    glPushMatrix();
    glColor3f(1.0f,1.0f,1.0f);
    glTranslatef(10.0f, -100.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    if (strcmp(configbackend,"mythtv")==0) myglprint4((char *) "Mythtv server");
    else myglprint4((char *) "XBMC server");
    glRasterPos2f(140.0f, 0.0f);
    myglprint4((char *) ":");
    if (do_show_setup_select_linie==1) {
      if (strcmp(configbackend,"mythtv")==0) strcpy(keybuffer,configmysqlhost);
      else strcpy(keybuffer,configxbmchost);
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      glRasterPos2f(160.0f, 0.0f);
      myglprint4((char *) keybuffer);
    } else {
      glColor3f(1.0f,1.0f,1.0f);
      glRasterPos2f(160.0f, 0.0f);
      if (strcmp(configbackend,"mythtv")==0) myglprint4((char *) configmysqlhost);
      else myglprint4((char *) configxbmchost);
    }
    glPopMatrix();
    if (do_show_setup_select_linie==1) showcoursornow(-200,-250,strlen(configmysqlhost));

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(10.0f, -150.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    if (strcmp(configbackend,"mythtv")==0) myglprint4((char *) "Mythtv user");
    else myglprint4((char *) "XBMC user");
    glRasterPos2f(140.0f, 0.0f);
    myglprint4((char *) ":");
    if (do_show_setup_select_linie==2) {
      if (strcmp(configbackend,"mythtv")==0) strcpy(keybuffer,configmysqluser);
      else strcpy(keybuffer,configxbmcuser);
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      glRasterPos2f(160.0f, 0.0f);
      myglprint4((char *) keybuffer);
    } else {
      glColor3f(1.0f,1.0f,1.0f);
      glRasterPos2f(160.0f, 0.0f);
      if (strcmp(configbackend,"mythtv")==0) myglprint4((char *) configmysqluser);
      else myglprint4((char *) configxbmcuser);
    }
    glPopMatrix();
    if (do_show_setup_select_linie==2) showcoursornow(-200,-300,strlen(configmysqluser));

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(10.0f, -200.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    if (strcmp(configbackend,"mythtv")==0) myglprint4((char *) "Password");
    else myglprint4((char *) "Password");
    glRasterPos2f(140.0f, 0.0f);
    myglprint4((char *) ":");
    if (do_show_setup_select_linie==3) {
      if (strcmp(configbackend,"mythtv")==0) strcpy(keybuffer,configmysqlpass);
      else strcpy(keybuffer,configxbmcpass);
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      glRasterPos2f(160.0f, 0.0f);
      myglprint4((char *) keybuffer);
    } else {
      glColor3f(1.0f,1.0f,1.0f);
      glRasterPos2f(160.0f, 0.0f);
      if (strcmp(configbackend,"mythtv")==0) myglprint4((char *) configmysqlpass);
      else myglprint4((char *) configxbmcpass);
    }
    glPopMatrix();
    if (do_show_setup_select_linie==3) showcoursornow(-200,-350,strlen(configmysqlpass));

    // show config music path
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(10.0f, -250.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Music path");
    glRasterPos2f(140.0f, 0.0f);
    myglprint4((char *) ":");
    glRasterPos2f(160.0f, 0.0f);
    if (do_show_setup_select_linie==4) {
      strcpy(keybuffer,configmusicpath);
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      myglprint4((char *) keybuffer);
    } else {
      glColor3f(1.0f,1.0f,1.0f);
      myglprint4((char *) configmusicpath);
    }
    glPopMatrix();
    if (do_show_setup_select_linie==4) showcoursornow(-200,-400,strlen(configmusicpath));

    // show config picture pth
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(10.0f, -300.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Picture path");
    glRasterPos2f(140.0f, 0.0f);
    myglprint4((char *) ":");
    glRasterPos2f(160.0f, 0.0f);
    if (do_show_setup_select_linie==5) {
      strcpy(keybuffer,configpicturepath);
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      myglprint4((char *) keybuffer);
    } else {
      glColor3f(1.0f,1.0f,1.0f);
      myglprint4((char *) configpicturepath);
    }
    glPopMatrix();
    if (do_show_setup_select_linie==5) showcoursornow(-200,-450,strlen(configpicturepath));

    // show config movie path
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(10.0f, -350.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Movie path");
    glRasterPos2f(140.0f, 0.0f);
    myglprint4((char *) ":");
    glRasterPos2f(160.0f, 0.0f);
    if (do_show_setup_select_linie==6) {
      strcpy(keybuffer,configmoviepath);
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      glRasterPos2f(160.0f, 0.0f);
      myglprint4((char *) keybuffer);
    } else {
      myglprint4((char *) configmoviepath);
    }
    glPopMatrix();
    if (do_show_setup_select_linie==6) showcoursornow(-200,-500,strlen(configmoviepath));



    glPushMatrix();
    glColor3f(1.0f,1.0f,1.0f);
    glTranslatef(10.0f, -400.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Recorded path");
    glRasterPos2f(140.0f, 0.0f);
    myglprint4((char *) ":");
    glRasterPos2f(160.0f, 0.0f);
    if (do_show_setup_select_linie==7) {
      strcpy(keybuffer,configrecordpath);
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      myglprint4((char *) keybuffer);
    } else {
      myglprint4(configrecordpath);
    }
    glPopMatrix();
    if (do_show_setup_select_linie==7) showcoursornow(-200,-550,strlen(configrecordpath));
}





//
// ********************* setuo keys ************************************************************************
//

void show_setup_keys() {
    //const char *keysarray[10]={" F3"," F4"," F5"," F6"," F7"," F8"," F9","F10","F11","F12"};

    int winsizx=100;
    int winsizy=300;
    int xpos=0;
    int ypos=0;
    char text[200];

    // background
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,setupkeysback);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),100 , 0.0);
    glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
    glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,100 , 0.0);
    glEnd();
    glPopMatrix();


    glPushMatrix();
    // close buttons
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D,_textureclose);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    winsizx=100;
    winsizy=100;
    xpos=475;
    ypos=-10;
    glLoadName(40);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

    glPushMatrix();
    // overskrift
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(680, 680, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Command to execute.                                                              ScrNr");
    glPopMatrix();

    glPushMatrix();
    // F3
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(610, 650, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "F3");
    glPopMatrix();

    glPushMatrix();
    // F4
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(610, 600, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "F4");
    glPopMatrix();


    glPushMatrix();
    // F5
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(610, 550, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "F5");
    glPopMatrix();

    glPushMatrix();
    // F6
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(610, 500, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "F6");
    glPopMatrix();

    glPushMatrix();
    // F7
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(610, 450, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "F7");
    glPopMatrix();

    glPushMatrix();
    // F8
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(610, 400, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "F8");
    glPopMatrix();

    glPushMatrix();
    // F9
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(610, 350, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "F9");
    glPopMatrix();

    glPushMatrix();
    // F10
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(610, 300, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "F10");
    glPopMatrix();

    glPushMatrix();
    // F11
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(610, 250, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "F11");
    glPopMatrix();


    // start af input felter
    // text input background F3
    glPushMatrix();
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=500;

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates


    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=450;

    // text input background F4
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates


    // text input background F5
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=400;

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates

    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates



    // text input background F6
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=350;
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates

    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates



    // text input background F7
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=300;
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates

    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates

    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=250;
    // text input background F8
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates

    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates

    // text input background F9
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=200;
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates

    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates


    // text input background F10
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=150;
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates

    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates


    // text input background F11
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=100;
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates

    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates


    // text input background F12
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=150;
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates

    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    glPushMatrix();
    xpos=810;
    ypos=100;
    winsizx=50;
    winsizy=30;

    for(int i=0;i<9;i++) {
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindTexture(GL_TEXTURE_2D,setupkeysbar2);			// setupkeysbar1
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS); //Begin quadrilateral coordinates
        glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+(i*50)+((orgwinsizey/2)-(800/2)) , 0.0);
        glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+(i*50)+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
        glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+(i*50)+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
        glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+(i*50)+((orgwinsizey/2)-(800/2)) , 0.0);
        glEnd(); //End quadrilateral coordinates
    }
    glPopMatrix();

    /*
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    for(int i=0;i<10;i++) {
        // start input here
        glColor3f(1.0f,1.0f,1.0f);
        glTranslatef(680-(i*50) , 650-(i*50), 0.0f);
        glRasterPos2f(0.0f, 0.0f);
        myglprint4((char *) keysarray[i]);
        glRasterPos2f(20.0, 0.0f);
        myglprint4((char *) ":");
    }
    glPopMatrix();
    */
    //printf("linie %d \n",do_show_setup_select_linie);


    // line 0
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(680 , 650 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==0) {
        strcpy(keybuffer,configkeyslayout[0].cmdname);
        myglprint4((char *) keybuffer);   // keybuffer
    } else {
        myglprint4((char *) configkeyslayout[0].cmdname);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==0) showcoursornow(311,500,strlen(keybuffer));

    // line 0
    glPushMatrix();
    glTranslatef(1180 , 650 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==1) {
        sprintf(keybuffer,"%d",configkeyslayout[0].scrnr);
        myglprint4((char *) keybuffer);   // keybuffer
    } else {
        sprintf(text,"%d",configkeyslayout[0].scrnr);
        myglprint4((char *) text);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==1) showcoursornow(812,500,strlen(keybuffer));


    // line 1
    glPushMatrix();
    glTranslatef(680 , 600 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==2) {
        strcpy(keybuffer,configkeyslayout[1].cmdname);
        myglprint4((char *) keybuffer);
    } else {
        sprintf(text,"%s",configkeyslayout[1].cmdname);
        myglprint4((char *) text);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==2) showcoursornow(311,450,strlen(keybuffer));

    // line 1
    glPushMatrix();
    glTranslatef(1180 , 600 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==3) {
      sprintf(keybuffer,"%d",configkeyslayout[1].scrnr);
      myglprint4((char *) keybuffer);   // keybuffer
    } else {
      sprintf(text,"%d",configkeyslayout[1].scrnr);
      myglprint4((char *) text);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==3) showcoursornow(812,450,strlen(keybuffer));


    // line 2
    glPushMatrix();
    glTranslatef(680 , 550 , 0.0f);
    glRasterPos2f(0.8f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==4) {
        strcpy(keybuffer,configkeyslayout[2].cmdname);
        myglprint4((char *) keybuffer);
    } else {
        glColor3f(1.0f,1.0f,1.0f);
        myglprint4((char *) configkeyslayout[2].cmdname);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==4) showcoursornow(311,400,strlen(keybuffer));

    // line 2
    glPushMatrix();
    glTranslatef(1180 , 550 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==5) {
      sprintf(keybuffer,"%d",configkeyslayout[2].scrnr);
      myglprint4((char *) keybuffer);   // keybuffer
    } else {
      sprintf(text,"%d",configkeyslayout[2].scrnr);
      myglprint4((char *) text);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==5) showcoursornow(812,400,strlen(keybuffer));



    // line 3
    glPushMatrix();
    glTranslatef(680 , 500 , 0.0f);
    glRasterPos2f(0.8f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==6) {
        strcpy(keybuffer,configkeyslayout[3].cmdname);
        myglprint4((char *) keybuffer);
    } else {
        glColor3f(1.0f,1.0f,1.0f);
        myglprint4((char *) configkeyslayout[3].cmdname);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==6) showcoursornow(311,350,strlen(keybuffer));

    // line 3
    glPushMatrix();
    glTranslatef(1180 , 500 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==7) {
      sprintf(keybuffer,"%d",configkeyslayout[3].scrnr);
      myglprint4((char *) keybuffer);   // keybuffer
    } else {
      sprintf(text,"%d",configkeyslayout[3].scrnr);
      myglprint4((char *) text);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==7) showcoursornow(812,350,strlen(keybuffer));


    // line 4
    glPushMatrix();
    glTranslatef(680 , 450 , 0.0f);
    glRasterPos2f(0.8f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==8) {
      strcpy(keybuffer,configkeyslayout[4].cmdname);
      myglprint4((char *) keybuffer);
    } else {
        glColor3f(1.0f,1.0f,1.0f);
        myglprint4((char *) configkeyslayout[4].cmdname);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==8) showcoursornow(311,300,strlen(keybuffer));


    // line 4
    glPushMatrix();
    glTranslatef(1180 , 450 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==9) {
      sprintf(keybuffer,"%d",configkeyslayout[4].scrnr);
      myglprint4((char *) keybuffer);   // keybuffer
    } else {
      sprintf(text,"%d",configkeyslayout[4].scrnr);
      myglprint4((char *) text);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==9) showcoursornow(812,300,strlen(keybuffer));

    // line 5
    glPushMatrix();
    glTranslatef(680 , 400 , 0.0f);
    glRasterPos2f(0.8f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==10) {
      strcpy(keybuffer,configkeyslayout[5].cmdname);
      myglprint4((char *) keybuffer);
    } else {
      glColor3f(1.0f,1.0f,1.0f);
      myglprint4((char *) configkeyslayout[5].cmdname);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==10) showcoursornow(311,250,strlen(keybuffer));

    // line 5
    glPushMatrix();
    glTranslatef(1180 , 400 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==11) {
      sprintf(keybuffer,"%d",configkeyslayout[5].scrnr);
      myglprint4((char *) keybuffer);   // keybuffer
    } else {
      sprintf(text,"%d",configkeyslayout[5].scrnr);
      myglprint4((char *) text);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==11) showcoursornow(812,250,strlen(keybuffer));


    // line 6
    glPushMatrix();
    glTranslatef(680 , 350 , 0.0f);
    glRasterPos2f(0.8f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==12) {
      strcpy(keybuffer,configkeyslayout[6].cmdname);
      myglprint4((char *) keybuffer);
    } else {
      glColor3f(1.0f,1.0f,1.0f);
      myglprint4((char *) configkeyslayout[6].cmdname);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==12) showcoursornow(311,200,strlen(keybuffer));

    // line 6
    glPushMatrix();
    glTranslatef(1180 , 350 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==13) {
      sprintf(keybuffer,"%d",configkeyslayout[6].scrnr);
      myglprint4((char *) keybuffer);   // keybuffer
    } else {
      sprintf(text,"%d",configkeyslayout[6].scrnr);
      myglprint4((char *) text);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==13) showcoursornow(812,200,strlen(keybuffer));

    // line 7
    glPushMatrix();
    glTranslatef(680 , 300 , 0.0f);
    glRasterPos2f(0.8f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==14) {
      strcpy(keybuffer,configkeyslayout[7].cmdname);
      myglprint4((char *) keybuffer);
    } else {
        glColor3f(1.0f,1.0f,1.0f);
        myglprint4((char *) configkeyslayout[7].cmdname);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==14) showcoursornow(311,150,strlen(keybuffer));

    // line 7
    glPushMatrix();
    glTranslatef(1180 , 300 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==15) {
      sprintf(keybuffer,"%d",configkeyslayout[7].scrnr);
      myglprint4((char *) keybuffer);   // keybuffer
    } else {
      sprintf(text,"%d",configkeyslayout[7].scrnr);
      myglprint4((char *) text);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==15) showcoursornow(812,150,strlen(keybuffer));

    // line 8
    glPushMatrix();
    glTranslatef(680 , 250 , 0.0f);
    glRasterPos2f(0.8f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==16) {
      strcpy(keybuffer,configkeyslayout[8].cmdname);
      myglprint4((char *) keybuffer);
    } else {
        glColor3f(1.0f,1.0f,1.0f);
        myglprint4((char *) configkeyslayout[8].cmdname);
    }
    glPopMatrix();

    if (do_show_setup_select_linie==16) showcoursornow(311,100,strlen(keybuffer));

    // line 9
    glPushMatrix();
    glTranslatef(1180 , 250 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==17) {
      sprintf(keybuffer,"%d",configkeyslayout[8].scrnr);
      myglprint4((char *) keybuffer);   // keybuffer
    } else {
      sprintf(text,"%d",configkeyslayout[8].scrnr);
      myglprint4((char *) text);
    }
    glPopMatrix();
    if (do_show_setup_select_linie==17) {
        sprintf(text,"%d",configkeyslayout[8].scrnr);
        showcoursornow(812,100,strlen(text));
    }
}


//
// parse channel info from xmltvguide reader channel overview xmlfile
//

void load_channel_list_from_graber() {
  FILE *fil;
  char buffer[1024];
  unsigned int cnr=0;
  xmlChar *tmpdat;
  xmlDoc *document;
  xmlNode *root, *first_child, *node, *node1 ,*subnode;
  xmlChar *xmltvid;
  xmlChar *content;
  int sysresult;
  int channelnr=0;
  char result[1024];
  char exestring[2048];
  if (debugmode) printf("Get channel list file from web.\n");
  strcpy(exestring,configbackend_tvgraber);
  strcat(exestring," --list-channels | grep '<display-name lang=' | cut -c29-300 | cut -f1 -d'<' > ~/tvguide_channels.txt");
  sysresult=system(exestring);
  fil=fopen("/home/hans/tvguide_channels.txt","r");
  if (fil) {
    while(!(feof(fil))) {
      fgets(buffer,512,fil);
      if (cnr<MAXPRGLIST_ANTAL) {
        strcpy(channel_list[cnr].id,"");
        strcpy(channel_list[cnr].name,buffer);
        channel_list[cnr].selected=true;
        cnr++;
        PRGLIST_ANTAL++;
      }
    }
    fclose(fil);
    // remove temp file again
    sysresult=system("rm ~/tvguide_channels.txt");
    if (debugmode) printf("Done channel list file from web. found %2d \n",cnr);
  }
}

//
// save tvguide channel info
//

void save_channel_list() {
  FILE *fil;
  unsigned int cnr=0;
  fil=fopen("/home/hans/tvguide_channels.dat","w");
  if (fil) {
    while(cnr<200) {
      fwrite(&channel_list[cnr],sizeof(channel_list_struct),1,fil);
      cnr++;
    }
    fclose(fil);
  }
}


//
// load tvguide channel info
//

void load_channel_list() {
  FILE *fil;
  unsigned int cnr=0;
  fil=fopen("/home/hans/tvguide_channels.dat","w");
  if (fil) {
    while(cnr<200) {
      fread(&channel_list[cnr],sizeof(channel_list_struct),1,fil);
      cnr++;
    }
    fclose(fil);
  }
}


//
// ********************* show setuo tv graber ************************************************************************
//

void show_setup_tv_graber(int startofset) {
    const char *weekdays[10]={"Monday","tuesday","Wednesday","thorsdag","Fredag","lørdag","søndag"};
    struct tm *xmlupdatelasttime;
    int winsizx=100;
    int winsizy=300;
    int xpos=0;
    int ypos=0;
    char text[200];
    static bool hent_tv_channels=false;
    // update channel list before show it
    if (hent_tv_channels==false) {
      hent_tv_channels=true;
      load_channel_list_from_graber();
      //load_channel_list();
    }
    // background
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,setupnetworkwlanback);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),100 , 0.0);
    glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
    glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,100 , 0.0);
    glEnd();
    glPopMatrix();


    glPushMatrix();
    // close buttons
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D,_textureclose);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    winsizx=100;
    winsizy=100;
    xpos=475;
    ypos=-10;
    glLoadName(40);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

    glPushMatrix();
    // overskrift
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(780, 740, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "TV Graber config");
    glPopMatrix();

    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(520, 650, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Use TV graber ");
    glPopMatrix();

    if (strcmp(configbackend_tvgraber,"Other")==0) {
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(520, 600, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      myglprint4((char *) "tvg command");
      glPopMatrix();
    }

    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(540, 600, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Last update");
    glPopMatrix();



    // start af input felter

    glPushMatrix();
    winsizx=500;
    winsizy=30;
    xpos=300;
    ypos=500;

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates

    if (strcmp(configbackend_tvgraber,"Other")==0) {
      // line 2
      winsizx=500;
      winsizy=30;
      xpos=300;
      ypos=450;
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glDisable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);
      glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS); //Begin quadrilateral coordinates
      glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glEnd(); //End quadrilateral coordinates
      do_show_setup_select_linie=1;
    }

    // line 0 input
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(680 , 650 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==0) {
        strcpy(keybuffer,configbackend_tvgraber);
        myglprint4((char *) keybuffer);   // keybuffer
    } else {
        myglprint4((char *) configbackend_tvgraber);
    }
    glPopMatrix();
    if (strcmp(configbackend_tvgraber,"Other")!=0) {
        // line 1 input
        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        glTranslatef(680 , 600 , 0.0f);
        glRasterPos2f(0.0f, 0.0f);
        glColor3f(1.0f,1.0f,1.0f);
        if (do_show_setup_select_linie==0) {
          xmlupdatelasttime=localtime(&configtvguidelastupdate);
          sprintf(keybuffer,"%s %d %d %d %d:%d",weekdays[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
          myglprint4((char *) keybuffer);   // keybuffer
        } else {
          xmlupdatelasttime=localtime(&configtvguidelastupdate);
          sprintf(keybuffer,"%s %d %d %d %d:%d",weekdays[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
          myglprint4((char *) keybuffer);   // keybuffer
        }
        glPopMatrix();
    }


    glPushMatrix();
    glTranslatef(680 , 600 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==1) {
      strcpy(keybuffer,configbackend_tvgraber_path);
      myglprint4((char *) configbackend_tvgraber_path);
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(680 , 600 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    if (do_show_setup_select_linie==2) {
      //strcp   y(keybuffer,configbackend_tvgraber_path);
      myglprint4((char *) "  ");
    }
    glPopMatrix();


    //
    // show channel names to select active channel and order
    //
    glPushMatrix();
    winsizx=450;
    winsizy=280;
    xpos=300;
    ypos=150;
    glEnable(GL_TEXTURE_2D);
    glColor3f(0.2f, 0.2f, 0.2f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,0);			// setupkeysbar1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

    for (int n=0;n<14;n++) {
      glPushMatrix();
      glTranslatef(672 , 560-(n*20) , 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      if ((do_show_setup_select_linie-3)==n) glColor3f(1.0f,1.0f,1.0f); else glColor3f(.7f,0.7f,0.7f);
      if (channel_list[(n-1)+startofset].selected) myglprint4((char *) "[x] "); else myglprint4((char *) "[ ] ");
      myglprint4((char *) channel_list[(n-1)+startofset].name);
      glPopMatrix();
    }

    if ((do_show_setup_select_linie==0) && (strcmp(configbackend_tvgraber,"Other")!=0)) showcoursornow(311,500,strlen(keybuffer));
    if ((do_show_setup_select_linie==1) && (strcmp(configbackend_tvgraber,"Other")==0)) showcoursornow(311,450,strlen(keybuffer));
    else if ((do_show_setup_select_linie==1) && (strcmp(configbackend_tvgraber,"Other")!=0)) showcoursornow(311,500,strlen(configbackend_tvgraber));
    else if ((do_show_setup_select_linie==2) && (strcmp(configbackend_tvgraber,"Other")!=0)) showcoursornow(311,450,strlen(configbackend_tvgraber));
    else if ((do_show_setup_select_linie>=2) && (strcmp(configbackend_tvgraber,"Other")!=0)) {
      showcoursornow(311,388-((do_show_setup_select_linie-3)*20),0);
    }
}




//
// ********************* setup start interface ********************************************** select sub menu
//

void show_setup_interface() {
    int ii=0;
    int winsizx=1000;
    int winsizy=800;
    int xpos=0;
    int ypos=0;
    int tabelofset=0;


    // background
//    glLoadIdentity();
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    //glBlendFunc(GL_ONE, GL_ONE);

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,setuptexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2)) , 0.0);
    glEnd();
    glPopMatrix();



//***********************************************************************************************
// sound setup
    // buttons
    winsizx=200;
    winsizy=200;
    xpos=200;
    ypos=500;
    // glTranslatef(-1.0f+sintable[tabelofset-ii]*3, 0.0f+costable[tabelofset-ii]*3,-15.0f);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D,_texturesoundsetup);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glLoadName(30);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates


//***********************************************************************************************

    // buttons
    //glTranslatef(-1.0f+sintable[tabelofset+(ii+14)]*3, 0.0f+costable[tabelofset+(ii+14)]*3,-15.0f);
    xpos=400;
    ypos=500;
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_texturesourcesetup);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(31);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates

//***********************************************************************************************



    // buttons
//    glTranslatef(-1.0f+sintable[tabelofset+(ii+28)]*3, 0.0f+costable[tabelofset+(ii+28)]*3,-15.0f);
    xpos=600;
    ypos=500;
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_textureimagesetup);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(32);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates

//***********************************************************************************************

    // buttons
//    glTranslatef(-1.0f+sintable[tabelofset+(ii+42)]*3, 0.0f+costable[tabelofset+(ii+42)]*3,-15.0f);
    xpos=800;
    ypos=500;
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_texturetemasetup);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(33);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates

//***********************************************************************************************

    // buttons
//    glTranslatef(-1.0f+sintable[tabelofset+(ii+56)]*3, 0.0f+costable[tabelofset+(ii+56)]*3,-15.0f);
    xpos=200;
    ypos=300;
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_texturemythtvsql);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(34);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates


//***********************************************************************************************


    // buttons
//    glTranslatef(-1.0f+sintable[tabelofset+(ii+70)]*3, 0.0f+costable[tabelofset+(ii+70)]*3,-15.0f);
    xpos=400;
    ypos=300;
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_texturesetupfont);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(35);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates


// *************************************************************************************************

//    glTranslatef(-1.0f+sintable[tabelofset+(ii+84)]*3, 0.0f+costable[tabelofset+(ii+84)]*3,-15.0f);
    xpos=600;
    ypos=300;
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_texturekeyssetup);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(36);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates


    // setup video player button
    xpos=800;
    ypos=300;
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_texturevideoplayersetup);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(38);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates


    // setup tv graber to use
    xpos=400;
    ypos=100;
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_texturetvgrabersetup);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(39);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates



    // close button
    xpos=200;
    ypos=100;
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_texturesetupclose);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(37);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd(); //End quadrilateral coordinates

}
