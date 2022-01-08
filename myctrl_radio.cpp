#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>
#include <mysql.h>
#include <GL/glc.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#include "myctrl_radio.h"
#include "utility.h"
#include "myth_ttffont.h"
#include "utility.h"
#include "readjpg.h"
#include "loadpng.h"


extern char debuglogdata[1024];                                  // used by log system

extern float configdefaultradiofontsize;
extern const char *dbname;                                    // db name in mysql
extern char configmysqluser[256];                             //
extern char configmysqlpass[256];                             //
extern char configmysqlhost[256];                             //
extern char configmusicpath[256];
extern int configmythtvver;
extern int screen_size;                                       //
extern int screensizey;                                       //
extern int screeny;                                           //
extern unsigned int musicoversigt_antal;                      //
extern int radio_key_selected;                                //
extern int music_select_iconnr;                               //
extern int do_music_icon_anim_icon_ofset;                     //
extern GLuint _textureIdback1;                                //
extern GLuint onlineradio;					                          //
extern GLuint onlineradio_empty;				                      //
extern GLuint onlineradio192;					                        //
extern GLuint onlineradio320;					                        //
extern GLuint radiooptions;                                   //
extern int fonttype;
extern fontctrl aktivfont;
extern GLuint _textureIdloading;
extern GLuint gfxlande[45];
extern GLuint gfxlandemask;
extern radiostation_class radiooversigt;
extern GLint cur_avail_mem_kb;
extern bool radio_oversigt_loaded;
extern bool radio_oversigt_loaded_done;
extern bool radio_oversigt_loaded_begin;

extern int radio_oversigt_loaded_nr;
extern int radio_oversigt_antal;


// ****************************************************************************************
//
// constructor
//
// ****************************************************************************************

radiostation_class::radiostation_class() : antal(0) {
    int i;
    for(i=0;i<maxantal;i++) stack[i]=0;
    for(i=0;i<radiooptionsmax;i++) {
      strcpy(radiosortopt[i].radiosortopt,"");
      radiosortopt[i].antal=0;
    }
    radiooptionsselect=0;							// selected line in radio options
}


// ****************************************************************************************
//
// destructor
//
// ****************************************************************************************

radiostation_class::~radiostation_class() {
    clean_radio_oversigt();
}


// ****************************************************************************************
//
// clean up number of created
//
// ****************************************************************************************

void radiostation_class::clean_radio_oversigt() {
    startup_loaded=false;				// set radio station loaded in
    for(int i=0;i<antal;i++) {
      if (stack[i]->textureId) glDeleteTextures(1, &stack[i]->textureId);	// delete radio texture
      delete stack[i];							// delete radio station
    }
    antal=0;
}

// *******************************************************************************************

// select next in radio sort option menu
void radiostation_class::nextradiooptselect() {
    if ((radiooptionsselect<40) && (strcmp(radiosortopt[radiooptionsselect+1].radiosortopt,"")!=0)) radiooptionsselect++;
}

// select last in radio sort option menu
void radiostation_class::lastradiooptselect() {
    if (radiooptionsselect>0) radiooptionsselect--;
}


// set en radio icon image

void radiostation_class::set_texture(int nr,GLuint idtexture) {
    stack[nr]->textureId=idtexture;
}

// ****************************************************************************************
//
// update radio stations gfx link
//
// ****************************************************************************************

int radiostation_class::opdatere_radiostation_gfx(int nr,char *gfxpath) {
    char sqlselect[512];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    sprintf(sqlselect,"update radio_stations set gfx_link='%s' where intnr=%d",gfxpath,nr);
    conn=mysql_init(NULL);
    // Connect to database
    try {
      if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, dbname, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
      }
      if (conn) mysql_close(conn);
    }
    catch (...) {
      fprintf(stdout,"Error update radio station db.\n");
    }
    return(1);
}

// ****************************************************************************************
//
// load all radio stations gfx
//
// ****************************************************************************************

int radiostation_class::load_radio_stations_gfx() {
    int i=0;
    GLuint texture;
    char tmpfilename[200];
    char gfxfilename[80];
    char resl[200];
    FILE *filhandle;
    try {
      filhandle=fopen("radio_gfx.log","w");
      if (startup_loaded) return(0);
      startup_loaded=true;
      while(i<radiooversigt.radioantal()) {
        strcpy(tmpfilename,"/opt/mythtv-controller/images/radiostations/");	// hent path
        strcpy(gfxfilename,radiooversigt.get_station_gfxfile(i));			// hent radio icon gfx filename
        strcat(tmpfilename,gfxfilename);        					// add filename to path
        if ((strcmp(gfxfilename,"")!=0) && (file_exists(tmpfilename))) {		// den har et navn samt gfx filen findes.
          texture=loadTexture ((char *) tmpfilename);					// load texture
          set_texture(i,texture);							// save it in radio station struct to show
        } else if (strcmp(gfxfilename,"")==0) {
          // check hvis ikke noget navn om der findes en fil med radio station navnet *.png/jpg
          // hvis der gør load denne fil.
          strcpy(tmpfilename,"/opt/mythtv-controller/images/radiostations/");
          strcat(tmpfilename,radiooversigt.get_station_name(i));
          strcat(tmpfilename,".png");
          if (file_exists(tmpfilename)) {
            texture=loadTexture ((char *) tmpfilename);                                 // load texture
            set_texture(i,texture);                         		            // save it in radio station struct
            strncpy(stack[i]->gfxfilename,get_station_name(i),stationamelength-1);      // update station gfxfilename to station name
            strcat(stack[i]->gfxfilename,".png");
            opdatere_radiostation_gfx(stack[i]->intnr,stack[i]->gfxfilename);           // and update db filename
          } else {
            strcpy(tmpfilename,"/opt/mythtv-controller/images/radiostations/");
            strcat(tmpfilename,radiooversigt.get_station_name(i));
            strcat(tmpfilename,".jpg");
            if (file_exists(tmpfilename)) {
              texture=loadTexture ((char *) tmpfilename);                                 // load texture
              set_texture(i,texture);     		                                            // save it in radio station struct
              strncpy(stack[i]->gfxfilename,get_station_name(i),stationamelength-1);      // update station gfxfilename to station name
              strcat(stack[i]->gfxfilename,".png");
              opdatere_radiostation_gfx(stack[i]->intnr,stack[i]->gfxfilename);           // and update db filename
            }
          }
        } else {
          sprintf(resl,"Radio station gfx file %s for %s is missing.\n",get_station_gfxfile(i),get_station_name(i));
          fputs(resl,filhandle);
        }
        i++;
      }
      if (filhandle) fclose(filhandle);
    }
    catch (...) {
      fprintf(stdout,"Error loading radio station graphic icons.\nWriting to radio_gfx.log fault.\n");
    }
    return(1);
}






// ****************************************************************************************
//
// TEMP USE TEMP USE TEMP USE TEMP USE
// Shopw all radio stations working or not
// You have to(set online to enable it on db)
// OVERLOAD opdatere_radio_oversigt
//
// ****************************************************************************************

int radiostation_class::opdatere_radio_oversigt() {
  char sqlselect[512];
  // mysql vars
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  int art,intnr,kbps;
  int land;
  bool online;
  //gotoxy(10,13);
  //printf("Opdatere radio oversigt fra database. type %d \n",radiosortorder);
  strcpy(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 order by popular desc,name");
  try {
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, dbname, 0, NULL, 0)) {
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (antal<maxantal)) {
          // printf("Hent info om radio station nr %s %-20s\n",row[6],row[0]);
          art=atoi(row[3]);
          intnr=atoi(row[6]);
          kbps=atoi(row[7]);
          online=atoi(row[8]);
          land=atoi(row[9]);
          if (antal<maxantal) {
            stack[antal]=new (struct radio_oversigt_type);
            if (stack[antal]) {
              strncpy(stack[antal]->station_name,row[0],stationamelength);
              strncpy(stack[antal]->desc,row[4],statiodesclength);
              strncpy(stack[antal]->streamurl,row[1],statiourl_homepage);
              strncpy(stack[antal]->homepage,row[2],statiourl_homepage);
              strncpy(stack[antal]->gfxfilename,row[5],stationamelength);
              stack[antal]->art=art;
              stack[antal]->kbps=kbps;
              stack[antal]->online=online;
              stack[antal]->land=land;
              stack[antal]->textureId=0;
              stack[antal]->intnr=intnr;
              antal++;
            }
          }
        }
        if (antal==0) printf("No Radio station loaded");
      } else {
        fprintf(stderr,"\nFailed to update radiodb, can not connect to database: mythtvcontroller Error: %s\n",mysql_error(conn));
      }
      //load_radio_stations_gfx();
      return(antal-1);
    } else {
      fprintf(stderr,"\nFailed to update radiodb, can not connect to database: mythtvcontroller Error: %s\n",mysql_error(conn));
    //  exit(0);
    }
    if (conn) mysql_close(conn);
  }
  catch (...) {
    fprintf(stdout,"Error connect to radio station db.\n");
  }
  return(0);
}




// ****************************************************************************************
//
// search radio station in db after searchtxt
// OVERLOAD
//
// ****************************************************************************************

int radiostation_class::opdatere_radio_oversigt(char *searchtxt) {
    char sqlselect[512];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    int art,intnr,kbps;
    int land;
//    char tmptxt1[80];
    bool online;
    strcpy(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and name like '%");
    strcat(sqlselect,searchtxt);
    strcat(sqlselect,"%'");
    try {
      conn=mysql_init(NULL);
      // Connect to database
      if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, dbname, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        if (res) {
          while (((row = mysql_fetch_row(res)) != NULL) && (antal<maxantal)) {
            art=atoi(row[3]);
            intnr=atoi(row[6]);
            kbps=atoi(row[7]);
            online=atoi(row[8]);
            land=atoi(row[9]);
            if (antal<maxantal) {
              stack[antal]=new (struct radio_oversigt_type);
              if (stack[antal]) {
                strncpy(stack[antal]->station_name,row[0],stationamelength);
                strncpy(stack[antal]->desc,row[4],statiodesclength);
                strncpy(stack[antal]->streamurl,row[1],statiourl_homepage);
                strncpy(stack[antal]->homepage,row[2],statiourl_homepage);
                strncpy(stack[antal]->gfxfilename,row[5],stationamelength);
                stack[antal]->art=art;
                stack[antal]->kbps=kbps;
                stack[antal]->online=online;
                stack[antal]->land=land;
                stack[antal]->textureId=0;
                stack[antal]->intnr=intnr;
                antal++;
              }
            }
          }
        }
        if (antal>0) return(antal-1); else return(0);
      } else fprintf(stderr,"Failed to connect to database: Error: %s\n",mysql_error(conn));
      if (conn) mysql_close(conn);
    }
    catch (...) {
      fprintf(stdout,"Error connect to mysql radio station db.\n");
    }
    return(0);
}



// ****************************************************************************************
//
// Opdatere liste efter sort order (radiosortorder)
// OVERLOAD
//
// ****************************************************************************************

int radiostation_class::opdatere_radio_oversigt(int radiosortorder) {
    char sqlselect[512];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    int art,intnr,kbps;
    int land;
    bool online;
    //gotoxy(10,13);
    //printf("Opdatere radio oversigt fra database. type %d \n",radiosortorder);
    if (radiosortorder==0)			// start order default by hørst mest
      strcpy(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by intnr");
      //strcpy(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 order by popular desc,name");
    else if (radiosortorder==28)		// bit rate
      sprintf(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by bitrate desc,popular desc,name");
    else if (radiosortorder==27)		// land kode
      sprintf(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by landekode desc,popular desc,name");
    else if (radiosortorder==19)		// mest hørt
      sprintf(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by lastplayed desc,popular desc,name");
    else 					// ellers efter art
      sprintf(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 and art=%d order by popular desc,name",radiosortorder);
    try {
      conn=mysql_init(NULL);
      // Connect to database
      if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, dbname, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        if (res) {
          while (((row = mysql_fetch_row(res)) != NULL) && (antal<maxantal)) {
            // printf("Hent info om radio station nr %s %-20s\n",row[6],row[0]);
            art=atoi(row[3]);
            intnr=atoi(row[6]);
            kbps=atoi(row[7]);
            online=atoi(row[8]);
            land=atoi(row[9]);
            if (antal<maxantal) {
              stack[antal]=new (struct radio_oversigt_type);
              if (stack[antal]) {
                strncpy(stack[antal]->station_name,row[0],stationamelength);
                strncpy(stack[antal]->desc,row[4],statiodesclength);
                strncpy(stack[antal]->streamurl,row[1],statiourl_homepage);
                strncpy(stack[antal]->homepage,row[2],statiourl_homepage);
                strncpy(stack[antal]->gfxfilename,row[5],stationamelength);
                stack[antal]->art=art;
                stack[antal]->kbps=kbps;
                stack[antal]->online=online;
                stack[antal]->land=land;
                stack[antal]->textureId=0;
                stack[antal]->intnr=intnr;
                antal++;
              }
            }
          }
          if (antal==0) printf("No Radio station loaded");
        } else {
          fprintf(stderr,"Failed to update radiodb, can not connect to database: mythtvcontroller Error: %s\n",mysql_error(conn));
        }
        if (conn) mysql_close(conn);
        //load_radio_stations_gfx();
        return(antal-1);
      } else {
        fprintf(stderr,"Failed to update radiodb, can not connect to database: mythtvcontroller Error: %s\n",mysql_error(conn));
      }
      if (conn) mysql_close(conn);
    }
    catch (...) {
      fprintf(stdout,"Error connect to mysql radio station db.\n");
    }
    return(0);
}


// ****************************************************************************************
//
// show radio stations overview
//
// ****************************************************************************************
extern int orgwinsizey;                                                         // screen size
extern int orgwinsizex;

bool radiostation_class::show_radio_oversigt1(GLuint normal_icon,GLuint normal_icon_mask,GLuint back_icon,GLuint dirplaylist_icon,int _mangley) {
    static bool show_all_kode_errors=false;
    int buttonsize=200;
    int buttonsizey=180;
    int xof=5;
    int yof=orgwinsizey-(buttonsizey);
    int boffset=154;
    int bonline=8;                        // numbers of icons pr line
    int i=0;
    int sofset=0;
    int lradiooversigt_antal=(bonline*5);
    int buttonzoom;
    char tmpfilename[200];
    char gfxfilename[200];
    char temptxt[200];
    char *lastslash;
    bool radio_pictureloaded=true;
    const char *radiostation_iconsgfx="/opt/mythtv-controller/images/radiostations/";
    char *base,*right_margin;
    int length,width;
    int pline=0;
    sofset=(_mangley/40)*8;
    GLuint texture;
    if (screen_size==2) {
      bonline=6;
    }
    if (screen_size==4) {
      bonline=7;
      buttonsize=160;
      buttonsizey=140;
    }
    // er gfx loaded
    // if no load 1 at eatch run
    // loader
    if ((radio_oversigt_loaded==false) && (radio_oversigt_loaded_nr<radiooversigt.radioantal())) {
        radio_oversigt_loaded_begin=true;
        radio_pictureloaded=false;
        strcpy(tmpfilename,radiostation_iconsgfx);      		                                    // hent path
        strcpy(gfxfilename,radiooversigt.get_station_gfxfile(radio_oversigt_loaded_nr));        // hent radio icon gfx filename
        strcat(tmpfilename,gfxfilename);        						// add filename to path
        if ((strcmp(gfxfilename,"")!=0) && (file_exists(tmpfilename))) {			// den har et navn samt gfx filen findes.
          texture=loadTexture ((char *) tmpfilename);						// load texture
          set_texture(radio_oversigt_loaded_nr,texture);					// save it in radio station struct to show
        } else if (strcmp(gfxfilename,"")==0) {
          // check hvis ikke noget navn om der findes en fil med radio station navnet *.png/jpg
          // hvis der gør load denne fil.
          strcpy(tmpfilename,radiostation_iconsgfx);
          strcat(tmpfilename,radiooversigt.get_station_name(radio_oversigt_loaded_nr));
          strcat(tmpfilename,".png");
          if (file_exists(tmpfilename)) {
            texture=loadTexture ((char *) tmpfilename);                      						           // load texture
            set_texture(radio_oversigt_loaded_nr,texture);                  				       		            // save it in radio station struct
            strncpy(stack[radio_oversigt_loaded_nr]->gfxfilename,get_station_name(radio_oversigt_loaded_nr),stationamelength-1);      // update station gfxfilename to station name
            strcat(stack[radio_oversigt_loaded_nr]->gfxfilename,".png");
            opdatere_radiostation_gfx(stack[radio_oversigt_loaded_nr]->intnr,stack[radio_oversigt_loaded_nr]->gfxfilename);           // and update db filename
          } else {
            strcpy(tmpfilename,radiostation_iconsgfx);
            strcat(tmpfilename,radiooversigt.get_station_name(radio_oversigt_loaded_nr));
            strcat(tmpfilename,".jpg");
            if (file_exists(tmpfilename)) {
              texture=loadTexture ((char *) tmpfilename);                                 // load texture
              set_texture(radio_oversigt_loaded_nr,texture);     		                                // save it in radio station struct
              strncpy(stack[radio_oversigt_loaded_nr]->gfxfilename,get_station_name(radio_oversigt_loaded_nr),stationamelength-1);      // update station gfxfilename to station name
              strcat(stack[radio_oversigt_loaded_nr]->gfxfilename,".png");
              opdatere_radiostation_gfx(stack[radio_oversigt_loaded_nr]->intnr,stack[radio_oversigt_loaded_nr]->gfxfilename);           // and update db filename
            }
          }
        }
        if (radio_oversigt_loaded_nr>=radiooversigt.radioantal()-1) {
          radio_oversigt_loaded=true;
          radio_oversigt_loaded_done=true;
        } else radio_oversigt_loaded_nr++;
    }
    glPushMatrix();
    while((i<lradiooversigt_antal) && ((int) i+(int) sofset<(int) antal) && (stack[i+sofset]!=NULL)) {
        if (((i % bonline)==0) && (i>0)) {
          xof=5;
          yof=yof-(buttonsizey+46);
        }
        if (i+1==(int) radio_key_selected) buttonsizey=180.0f; else buttonsizey=150.0f;
        if (stack[i+sofset]->textureId) {
            // radio default icon
            glPushMatrix();
            // is the radio station online
            // if NOT show faded colors
            if (stack[i+sofset]->online) glColor4f(1.0f, 1.0f, 1.0f,1.0f); else glColor4f(.3f, .3f, .3f, 1.0f);
            glEnable(GL_TEXTURE_2D);
            glBlendFunc(GL_ONE, GL_ONE);
            //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glBindTexture(GL_TEXTURE_2D,onlineradio_empty);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
            glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsizey, 0.0);
            glTexCoord2f(1, 1); glVertex3f( xof+buttonsize, yof+buttonsizey , 0.0);
            glTexCoord2f(1, 0); glVertex3f( xof+buttonsize,yof , 0.0);
            glEnd();
            glPopMatrix();
            glPushMatrix();
            // is the radio station online
            // if NOT show faded colors
            if (stack[i+sofset]->online) glColor4f(1.0f, 1.0f, 1.0f,1.0f); else glColor4f(.3f, .3f, .3f, 1.0f);
            // indside draw radio station icon
            glEnable(GL_TEXTURE_2D);
            glBlendFunc(GL_ONE, GL_ONE);
            //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glLoadName(100+i+sofset);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
            glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
            glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-10, yof+buttonsizey-20 , 0.0);
            glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-10, yof+10 , 0.0);
            glEnd();
            glPopMatrix();
        } else {
          // default icon
          glPushMatrix();
          if (stack[i+sofset]->online) glColor4f(1.0f, 1.0f, 1.0f,1.0f); else glColor4f(.3f, .3f, .3f, 1.0f);
          glEnable(GL_TEXTURE_2D);
          glBlendFunc(GL_ONE, GL_ONE);
          //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
          glBindTexture(GL_TEXTURE_2D,onlineradio);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glLoadName(100+i+sofset);
          glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsizey, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsize, yof+buttonsizey , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsize,yof , 0.0);
          glEnd();
          glPopMatrix();
        }
        // draw radio station contry flags
        if (stack[i+sofset]->land>0) {
          // gfxlandemask mask
          if (gfxlande[stack[i+sofset]->land]) {
            glBindTexture(GL_TEXTURE_2D,gfxlande[stack[i+sofset]->land]);       //

            //glBindTexture(GL_TEXTURE_2D,gfxlande[i+sofset]);

            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f(10+ xof, yof+10 , 0.0);
            glTexCoord2f(0, 1); glVertex3f(10+ xof,yof+30+10, 0.0);
            glTexCoord2f(1, 1); glVertex3f(10+ xof+40, yof+30+10 , 0.0);
            glTexCoord2f(1, 0); glVertex3f(10+ xof+40,yof +10, 0.0);
            glEnd();
          } else {
            //if (debugmode & 1024) printf("Contry kode %d missing flag, File name %s\n",stack[i+sofset]->land,gfxlande[stack[i+sofset]->land]);
            // write debug log
            sprintf(debuglogdata,"Contry code %d missing flag, File name %s",stack[i+sofset]->land,gfxlande[stack[i+sofset]->land]);
            if (gfxlande[stack[i+sofset]->land]==0) {
              if (show_all_kode_errors==false) write_logfile((char *) debuglogdata);
            } else if (show_all_kode_errors==false) write_logfile((char *) debuglogdata);
          }
        }
        // print radios station name
        glPushMatrix();
        pline=0;
        glTranslatef(xof,yof-18,0);
        glDisable(GL_TEXTURE_2D);
        glScalef(configdefaultradiofontsize, configdefaultradiofontsize, 1.0);
        glColor4f(1.0f, 1.0f, 1.0f,1.0f);
        glRasterPos2f(0.0f, 0.0f);
        glDisable(GL_TEXTURE_2D);
        bool stop=false;
        strcpy(temptxt,stack[i+sofset]->station_name);        // radio station navn
        base=temptxt;
        length=strlen(temptxt);
        width = 22;                                                             // normal 22 18 point font
        while(*base) {
          if(length <= width) {
            glTranslatef((width/5)-(strlen(base)/4),0.0f,0.0f);
            glcRenderString(base);
            pline++;
            break;
          }
          right_margin = base+width;
          while((!isspace(*right_margin)) && (stop==false)) {
            right_margin--;
            if (right_margin == base) {
              right_margin += width;
              while(!isspace(*right_margin)) {
                if (*right_margin == '\0') break;
                else stop=true;
                right_margin++;
              }
            }
          }
          if (stop) *(base+width)='\0';
          *right_margin = '\0';
          glcRenderString(base);
          pline++;
          glTranslatef(xof,(yof-18)-pline*1.2f,0);
          length -= right_margin-base+1;                         // +1 for the space
          base = right_margin+1;
          if (pline>=2) break;
        }

/*
        float fontsiz=15.0f;
        glDisable(GL_TEXTURE_2D);
        glTranslatef(xof,yof-18,0);
        glScalef(fontsiz, fontsiz, 1.0);
        strcpy(temptxt,stack[i+sofset]->station_name);        // radio station navn
        lastslash=strrchr(temptxt,'/');
        if (lastslash) strcpy(temptxt,lastslash+1);
        //float ytextofset=0.0f;
        int ii,j,k,pos,ofs;
        ii=pos=0;
        char word[16000];
        ofs=(strlen(temptxt)/2)*9;

        //glTranslatef(1,10,0);
        if (strlen(temptxt)<=14) glcRenderString(temptxt);
        else {
            temptxt[14]=0;
            glcRenderString(temptxt);

        }
*/
        glPopMatrix();
        xof=xof+buttonsize+6;
        i++;
    }
    radio_oversigt_antal=radiooversigt.radioantal();
    if (i==0) {
      // show error message
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      glBindTexture(GL_TEXTURE_2D,_textureIdloading);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/3), 200 , 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/3), 200+150, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/3)+450, 200+150 , 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/3)+450, 200 , 0.0);
      glEnd();
      glPushMatrix();
      glTranslatef((orgwinsizex/3)+30, 275.0f , 0.0f);
      glDisable(GL_TEXTURE_2D);
      glScalef(24.0, 24.0, 1.0);
      glColor3f(0.6f, 0.6f, 0.6f);
      sprintf(temptxt,"Error no radio stations load");
      glcRenderString(temptxt);
      glPopMatrix();
    }
    glPopMatrix();
    show_all_kode_errors=true;                                                  // stop loging.
    return(radio_pictureloaded);
}


// ****************************************************************************************
//
// skal vi opdatere sort type oversigt første gang
//
// ****************************************************************************************

static bool hentradioart=false;


void radiostation_class::show_radio_options() {
    int i;
    char sqlselect[512];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char tmptxt[80];
    float sizex=4.0f;
    float sizey=3.0f;
    int winsizx=1200;
    int winsizy=800;
    int xpos=0;
    int ypos=0;
    // background
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, radiooptions);					// _textureId18);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    // draw  front box
    glTexCoord2f(0, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2)) , 0.0);
    glEnd();
    glPopMatrix();
    i=0;
    if (hentradioart==false) {
      hentradioart=true;
      strcpy(sqlselect,"SELECT typename,radiotypes.art,count(radio_stations.art) FROM `radiotypes`,radio_stations where radiotypes.art=radio_stations.art or radiotypes.art=0 and radio_stations.online=1 group by (radiotypes.art)");
      conn=mysql_init(NULL);
      // Connect to database
      if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, dbname, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        while (((row = mysql_fetch_row(res)) != NULL) && (i<radiooptionsmax)) {
          strcpy(radiosortopt[i].radiosortopt,row[0]);
          radiosortopt[i].radiosortoptart=atoi(row[1]);
          radiosortopt[i].antal=atoi(row[2]);
          i++;
        }
      }
    }
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    //glTranslatef(0.0f, 0.0f, 0.0f);
    glTranslatef(380.0f, 750.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    glScalef(25.0f, 25.0f, 1.00f);
    glcRenderString("Sort options.");
    glPopMatrix();
    i=0;
    while ((strcmp(radiosortopt[i].radiosortopt,"")!=0) && (i<40)) {
      glPushMatrix();
      if (i<19) {
        glTranslatef(600.0f, 700-(i*20.0f), 0.0f);
        glRasterPos2f(0.0f, 0.0f);
      } else {
        glTranslatef(1100.0f, 1050-(i*20.0f), 0.0f);
        glRasterPos2f(0.0f, 0.0f);
      }
      if (i==radiooptionsselect) glColor4f(0.0f,0.0f,1.0f,1.0f); else glColor4f(1.0f,1.0f,1.0f,1.0f);
      glScalef(25.0f, 25.0f, 1.00f);
      glcRenderString(radiosortopt[i].radiosortopt);
      glPopMatrix();
      glPushMatrix();
      if (i<19) {
        glTranslatef(500.0f, 700-(i*20.0f), 0.0f);
        glRasterPos2f(0.0f, 0.0f);
      } else {
        glTranslatef(1000.0f, 1050-(i*20.0f), 0.0f);
        glRasterPos2f(0.0f, 0.0f);
      }
      if (i==radiooptionsselect) glColor4f(0.0f,0.0f,1.0f,1.0f); else glColor4f(1.0f,1.0f,1.0f,1.0f);
      if ((radiosortopt[i].radiosortoptart!=27) && (radiosortopt[i].radiosortoptart!=28)) {
        sprintf(tmptxt,"%5d",radiosortopt[i].antal);
        glScalef(25.0f, 25.0f, 1.00f);
        glcRenderString(tmptxt);
      }
      i++;
      glPopMatrix();
    }
}




//
// *********************************************************************************
// opdatere list set numbers of aflytninger

int radiostation_class::set_radio_popular(int stationid) {
    char sqlselect[512];
    MYSQL *conn;
    MYSQL_RES *res;
    // write debug log
    write_logfile((char *) "Update played radio station.");
    sprintf(sqlselect,"update radio_stations set popular=popular+1,lastplayed=now() where intnr=%ld",stack[stationid]->intnr);
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, dbname, 0, NULL, 0)) {
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      mysql_close(conn);
      return(1);
    }
    return(0);
}




// ****************************************************************************************
//
// set online/offline status on radio station (stationid)
//
// ****************************************************************************************

int radiostation_class::set_radio_online(int stationid,bool onoff) {
    char sqlselect[512];
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    write_logfile((char *) "Update played radio station online.");
    if (onoff) sprintf(sqlselect,"update radio_stations set online=1 where intnr=%ld",stack[stationid]->intnr);
      else sprintf(sqlselect,"update radio_stations set online=0 where intnr=%ld",stack[stationid]->intnr);
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, dbname, 0, NULL, 0)) {
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          ;
        }
      }
      mysql_close(conn);
      return(1);
    }
    stack[stationid]->online=onoff;
    return(0);
}

// ****************************************************************************************
//
// get radio online flag
//
// ****************************************************************************************

int radiostation_class::get_radio_online(int stationid) {
    if (((unsigned int) stationid<(unsigned int) antal) && (stack[stationid])) {
        return(stack[stationid]->online);
    } else {
        return(-1);
    }
}

// ****************************************************************************************
//
// set radio station online flag internal use
//
// ****************************************************************************************

int radiostation_class::set_radio_intonline(int arraynr) {
    if (((unsigned int) arraynr<(unsigned int) antal) && (stack[arraynr])) stack[arraynr]->online=true; else return(0);
    return(1);
}

// ****************************************************************************************
//
// check if radio station is ofline in database and return true if redio station exist in db
//
// ****************************************************************************************

bool radiostation_class::check_radio_online_bool() {
    char sqlselect[512];
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    try {
      conn=mysql_init(NULL);
      strcpy(sqlselect,"select intnr from radio_stations where online=0 order by intnr limit 100");
      if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, dbname, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        mysql_close(conn);
        return(1);
      } else return(0);		// we are done check all radio stations in database
    }
    catch (...) {
      fprintf(stdout,"Error connect to mysql db.\n");
      return(0);
    }
}


// ****************************************************************************************
//
// ****************************************************************************************

int radiostation_class::set_radio_aktiv(int stationid,bool onoff) {
//    stack[stationid]->intnr;
    stack[stationid]->aktiv=onoff;
    return(0);
}

// ****************************************************************************************
//
// ****************************************************************************************


int init_sockaddr (struct sockaddr_in *name,const char *hostname,uint16_t port) {
    int error=0;
    struct hostent *hostinfo;
    name->sin_family = AF_INET;
    name->sin_port = htons (port);
    hostinfo = gethostbyname (hostname);
    if (hostinfo == NULL) {
      fprintf (stderr, "Unknown host %s.\n", hostname);
      error=1;
    }
    if (error==0) name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
    return(error);
}

// ****************************************************************************************
//
// returm port nr from url if exist in url or 0
//
// ****************************************************************************************

int get_url_data(char *url,char *ipadd) {
    char *sted;
    char *langurl=NULL;
    int destport=0;
    sted=(char*) strrchr(url, ':');		               // find : fra start af url
    if (sted) {
      if (strncmp(sted,"://",3)!=0) {		             // er vi fra start af url
        destport=atoi(sted+1);		                   // hent port nr
        if (strncmp(url,"http",4)==0) {
          *sted='\0';
          strcpy(ipadd,url+7); 		                   // get rest of url
        } else if (strncmp(url,"mms",3)==0) {
          *sted='\0';
          strcpy(ipadd,url+6); 		                   // get rest of url
        }
        langurl=strchr(ipadd, '/');		               // find first /
        if  (langurl) *langurl='\0';
      } else {
        if (strncmp(url,"http",4)==0) {
          strcpy(ipadd,url+7);		                   // get rest of url
        }
        if (strncmp(url,"mms",3)==0) {
          strcpy(ipadd,url+6);                       // get rest of url
        }
        langurl=strchr(ipadd, '/');		               // find first /
        if  (langurl) *langurl='\0';
      }
    }
    return(destport);
}



// ****************************************************************************************
//
// check if radio station is ofline in database and set it online again if it is back online
// return where we are ind the database
// if start recordnr>0 do it
//
// ****************************************************************************************

bool check_radio_online_switch=true;

unsigned long radiostation_class::check_radio_online(unsigned int radioarrayid) {
    short int port=0;
    int sock;
    struct sockaddr_in servername;
    char hostname[1024];
    char ipadresse[1024];
    char st_name[1024];
    int error=0;
    unsigned long radiostation=0;
    char sqlselect[512];
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    int nn;
    bool nfundet;
    bool radiook=false;
    bool cerror;
    struct timeval tv;
    fd_set myset;
    write_logfile((char *) "Check radio stations.");
    if (check_radio_online_switch) {
      conn=mysql_init(NULL);
      strcpy(sqlselect,"select name,aktiv,intnr,stream_url from radio_stations where online=1 and aktiv=1 order by popular desc,name limit 1");
      if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, dbname, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        if (res) {
          while ((row = mysql_fetch_row(res)) != NULL) {
            // port=80;
            strncpy(hostname,row[3],1000);
            strncpy(st_name,row[0],1000);
            if (strcmp(hostname,"")!=0) {
              // get port and ip
              port=get_url_data(hostname,ipadresse);
              // write debug log
              sprintf(debuglogdata,"Checking Station : %-50s - hostname : %s port %d ",row[0],hostname,port);
              write_logfile((char *) debuglogdata);
              sock=socket(PF_INET, SOCK_STREAM, 0);
              if (sock) {
                //fcntl(sock, F_SETFL, O_NONBLOCK);
                tv.tv_sec = 5;
                tv.tv_usec = 0;
                FD_ZERO(&myset);
                FD_SET(sock, &myset);
                error=(init_sockaddr(&servername,ipadresse,port));
                if ((error==0) && (cerror=connect(sock,(struct sockaddr *) &servername,sizeof (servername)))) {
                  if (cerror==0) {
                    write_logfile((char *) "Station OK.");
                    radiook=true;
                  } else radiook=false;
                } else {
                  write_logfile((char *) "Station BAD.");
                  radiook=false;
                }
                close (sock);
              }
              radiostation=atol(row[2]);
            }
          }
          nn=0;
          // find radio station
          nfundet=false;
          while ((nn<antal) && (nfundet==false)) {
            if  (stack[nn]->station_name) {
              // if found set active again
              if (strcmp(stack[nn]->station_name,st_name)==0) {
                stack[nn]->online=1;
                nfundet=true;
              } else nn++;
            } else nn++;
          }
        }
        if ((conn) && (radiostation)) {
          if ((radiook) && (nfundet)) {
            sprintf(sqlselect,"update radio_stations set online=1 where intnr=%ld \n",radiostation);
          } else {
            sprintf(sqlselect,"update radio_stations set online=0,aktiv=0 where intnr=%ld \n",radiostation);
          }
          mysql_query(conn,sqlselect);
          res = mysql_store_result(conn);
        }
      }
      if (conn) mysql_close(conn);
    }
    return(radiostation);		// we are done check all radio stations in database
}
