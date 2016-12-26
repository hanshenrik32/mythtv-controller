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

extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];
extern int configmythtvver;
extern int screen_size;
extern int screensizey;
extern int screeny;
extern int debugmode;                                          // set in main
extern unsigned int musicoversigt_antal;
extern int radio_key_selected;
extern int music_select_iconnr;
extern int do_music_icon_anim_icon_ofset;
extern GLuint _textureIdback1;
extern GLuint onlineradio;					//
extern GLuint onlineradio_empty;				//
extern GLuint onlineradio192;					//
extern GLuint onlineradio320;					//
extern GLuint onlineradiomask;					//
extern GLuint radiooptions,radiooptionsmask;			//
extern int fonttype;
extern fontctrl aktivfont;

extern GLuint _textureIdloading,_textureIdloading1;
extern GLuint _textureIdloading_mask;


extern GLuint gfxlande[45];
extern GLuint gfxlandemask;

extern radiostation_class radiooversigt;

extern GLint cur_avail_mem_kb;

extern bool radio_oversigt_loaded;
extern bool radio_oversigt_loaded_done;


// constructor
radiostation_class::radiostation_class() : antal(0) {
    int i;
    for(i=0;i<maxantal;i++) stack[i]=0;
    for(i=0;i<radiooptionsmax;i++) {
        strcpy(radiosortopt[i].radiosortopt,"");
        radiosortopt[i].antal=0;
    }
    radiooptionsselect=0;							// selected line in radio options
}

// destructor
radiostation_class::~radiostation_class() {
    clean_radio_oversigt();
}


// clean up number of created

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


//
// update radio stations gfx link
//

int radiostation_class::opdatere_radiostation_gfx(int nr,char *gfxpath) {
    char sqlselect[512];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    char *database = (char *) "mythtvcontroller";
    sprintf(sqlselect,"update radio_stations set gfx_link='%s' where intnr=%d",gfxpath,nr);
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        mysql_close(conn);
    }
    return(1);
}


// load all radio stations gfx

int radiostation_class::load_radio_stations_gfx() {
    int i=0;
    GLuint texture;
    char tmpfilename[200];
    char gfxfilename[80];
    char resl[200];
    FILE *filhandle;
    filhandle=fopen("radio_gfx.log","w");
    if (startup_loaded) return(0);
    startup_loaded=true;
    while(i<radiooversigt.radioantal()) {
        strcpy(tmpfilename,"/usr/share/mythtv-controller/images/radiostations/");	// hent path
        strcpy(gfxfilename,radiooversigt.get_station_gfxfile(i));			// hent radio icon gfx filename
        strcat(tmpfilename,gfxfilename);        					// add filename to path
        if ((strcmp(gfxfilename,"")!=0) && (file_exists(tmpfilename))) {		// den har et navn samt gfx filen findes.
            texture=loadTexture ((char *) tmpfilename);					// load texture
            set_texture(i,texture);							// save it in radio station struct to show
        } else if (strcmp(gfxfilename,"")==0) {
            // check hvis ikke noget navn om der findes en fil med radio station navnet *.png/jpg
            // hvis der gør load denne fil.
            strcpy(tmpfilename,"/usr/share/mythtv-controller/images/radiostations/");
            strcat(tmpfilename,radiooversigt.get_station_name(i));
            strcat(tmpfilename,".png");
            if (file_exists(tmpfilename)) {
                texture=loadTexture ((char *) tmpfilename);                                 // load texture
                set_texture(i,texture);                         		            // save it in radio station struct
                strncpy(stack[i]->gfxfilename,get_station_name(i),stationamelength-1);      // update station gfxfilename to station name
                strcat(stack[i]->gfxfilename,".png");
                opdatere_radiostation_gfx(stack[i]->intnr,stack[i]->gfxfilename);           // and update db filename
            } else {
                strcpy(tmpfilename,"/usr/share/mythtv-controller/images/radiostations/");
                strcat(tmpfilename,radiooversigt.get_station_name(i));
                strcat(tmpfilename,".jpg");
                if (file_exists(tmpfilename)) {
                    texture=loadTexture ((char *) tmpfilename);                                 // load texture
                    set_texture(i,texture);     		                                // save it in radio station struct
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
    return(1);
}

//
// search radio station in db after searchtxt
// OVERLOAD
//
int radiostation_class::opdatere_radio_oversigt(char *searchtxt) {
    char sqlselect[512];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    int art,intnr,kbps;
    int land;
    char *database = (char *) "mythtvcontroller";
//    char tmptxt1[80];
    bool online;
    strcpy(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and name like '%");
    strcat(sqlselect,searchtxt);
    strcat(sqlselect,"%'");
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
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
        if (conn) mysql_close(conn);
        if (antal>0) return(antal-1); else return(0);
    } else fprintf(stderr,"Failed to connect to database: Error: %s\n",mysql_error(conn));
    return(0);
}




//
// opdatere liste efter sort order (radiosortorder)
// OVERLOAD
//
int radiostation_class::opdatere_radio_oversigt(int radiosortorder) {
    char sqlselect[512];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    int art,intnr,kbps;
    int land;
    char *database = (char *) "mythtvcontroller";
    bool online;
    //gotoxy(10,13);
    //printf("Opdatere radio oversigt fra database. type %d \n",radiosortorder);
    if (radiosortorder==0)			// start order default by hørst mest
        strcpy(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by popular desc,name");
    else if (radiosortorder==28)		// bit rate
        sprintf(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by bitrate desc,popular desc,name");
    else if (radiosortorder==27)		// land kode
        sprintf(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by landekode desc,popular desc,name");
    else if (radiosortorder==19)		// mest hørt
        sprintf(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by lastplayed desc,popular desc,name");
    else 					// ellers efter art
        sprintf(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 and art=%d order by popular desc,name",radiosortorder);
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
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
        mysql_close(conn);
        //load_radio_stations_gfx();
        return(antal-1);
    } else {
      fprintf(stderr,"Failed to update radiodb, can not connect to database: mythtvcontroller Error: %s\n",mysql_error(conn));
    //  exit(0);
    }
    return(0);
}



extern int orgwinsizey;
extern int orgwinsizex;



bool radiostation_class::show_radio_oversigt1(GLuint normal_icon,GLuint normal_icon_mask,GLuint back_icon,GLuint dirplaylist_icon,int _mangley) {
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
    const char *radiostation_iconsgfx="/usr/share/mythtv-controller/images/radiostations/";

    sofset=(_mangley/40)*8;

    //static bool radio_oversigt_loaded=false;
    static int radio_oversigt_loaded_nr=0;
    GLuint texture;
    // er gfx loaded
    // if no load 1 at eatch run
    if ((radio_oversigt_loaded==false) && (radio_oversigt_loaded_nr<radiooversigt.radioantal())) {
        radio_pictureloaded=false;
        strcpy(tmpfilename,radiostation_iconsgfx);      		// hent path
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
        if (radio_oversigt_loaded_nr>=radiooversigt.radioantal()) {
            radio_oversigt_loaded=true;
            radio_oversigt_loaded_done=true;
        } else radio_oversigt_loaded_nr++;
    }
//    printf("sofset =  %d  _mangley = %d \n ",sofset,_mangley);
//    glBindTexture(GL_TEXTURE_2D, onlineradiomask);
    glPushMatrix();
    while((i<lradiooversigt_antal) && ((int) i+(int) sofset<(int) antal) && (stack[i+sofset]!=NULL)) {
        if (((i % bonline)==0) && (i>0)) {
            xof=5;
            yof=yof-(buttonsizey+46);
        }

        if (i+1==(int) radio_key_selected) buttonsizey=180.0f; else buttonsizey=150.0f;

        // mask
        glPushMatrix();
        // er radio station online (hvis ja full color ellers 0.3)
        if (stack[i+sofset]->online) glColor4f(1.0f, 1.0f, 1.0f,1.0f); else glColor4f(.3f, .3f, .3f, 1.0f);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_DST_COLOR, GL_ZERO);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,onlineradiomask);
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
            if (gfxlande[stack[i+sofset]->land]!=0) {
                glBindTexture(GL_TEXTURE_2D,gfxlande[stack[i+sofset]->land]);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex3f(10+ xof, yof+10 , 0.0);
                glTexCoord2f(0, 1); glVertex3f(10+ xof,yof+30+10, 0.0);
                glTexCoord2f(1, 1); glVertex3f(10+ xof+40, yof+30+10 , 0.0);
                glTexCoord2f(1, 0); glVertex3f(10+ xof+40,yof +10, 0.0);
                glEnd();
            } else {
                if (debugmode & 64) printf("Contry kode %d missing flag, File name %s\n",stack[i+sofset]->land,gfxlande[stack[i+sofset]->land]);
            }
        }

        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        glTranslatef(xof,yof-218,0);
        glScalef(20.0, 20.0, 1.0);
        strcpy(temptxt,stack[i+sofset]->station_name);        // radio station navn
        lastslash=strrchr(temptxt,'/');
        if (lastslash) strcpy(temptxt,lastslash+1);
        float ytextofset=0.0f;
        int ii,j,k,pos,ofs;
        ii=pos=0;
        char word[16000];
        ofs=(strlen(temptxt)/2)*9;

        glTranslatef(1,10,0);

        if (strlen(temptxt)<=14) {
            glcRenderString(temptxt);
        } else {
            temptxt[15]=0;
            glcRenderString(temptxt);

        }
        glPopMatrix();

        xof=xof+buttonsize+6;
        i++;
    }
    if (radio_oversigt_loaded_nr<radiooversigt.radioantal()) {
        // show radio icon loader status
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,_textureIdloading);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(1470+200, 75 , 0.0);
        glTexCoord2f(0, 1); glVertex3f(1470+200, 75+130, 0.0);
        glTexCoord2f(1, 1); glVertex3f(1470+200+250, 75+130 , 0.0);
        glTexCoord2f(1, 0); glVertex3f(1470+200+250, 75 , 0.0);
        glEnd();

        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        glTranslatef(1680+20,95,0);
        glScalef(24.0, 24.0, 1.0);
        glColor3f(0.6f, 0.6f, 0.6f);
        sprintf(temptxt,"%4d of %4d ",radio_oversigt_loaded_nr,radiooversigt.radioantal());
        glcRenderString(temptxt);
        glPopMatrix();
    }

    if (i==0) {
        // show error message
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,_textureIdloading1);
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
    return(radio_pictureloaded);
}


// old ver


void radiostation_class::show_radio_oversigt(GLuint normal_icon,GLuint normal_icon_mask,GLuint back_icon,GLuint dirplaylist_icon,int _mangley) {

  int lradiooversigt_antal;
  int i=0;
//  int txtbrede;
  float ofs=0.0f;		// used to calc the text length
  float yof=0.0f;
  float xof=0.0f;
//  float yofset=10.0f;
//  float xofset=22.0f;
  char *lastslash;
  float xvgaz=0.0f;
  char temptxt[200];
  unsigned int sofset;
  int bonline;
  float buttonsizex=68.0f;		// 68
  float buttonsizey=58.0f;
  float buttonzoom=0.0f;
//  float scale=1.0f;
  float boffset;

  int loader_xpos,loader_ypos;
  char tmpfilename[200];
  char gfxfilename[200];

  static bool radio_oversigt_loaded=false;
  static int radio_oversigt_loaded_done=0;
  static int radio_oversigt_loaded_nr=0;
  GLuint texture;

  if ((debugmode) && (radio_oversigt_loaded==false)) printf("loading station : %d \n",radio_oversigt_loaded_nr);

  if ((radio_oversigt_loaded==false) && (radio_oversigt_loaded_nr<radiooversigt.radioantal())) {
    strcpy(tmpfilename,"/usr/share/mythtv-controller/images/radiostations/");      	 // hent path
    strcpy(gfxfilename,radiooversigt.get_station_gfxfile(radio_oversigt_loaded_nr));       // hent radio icon gfx filename
    strcat(tmpfilename,gfxfilename);        					// add filename to path
    if ((strcmp(gfxfilename,"")!=0) && (file_exists(tmpfilename))) {		// den har et navn samt gfx filen findes.
        texture=loadTexture ((char *) tmpfilename);					// load texture
        set_texture(radio_oversigt_loaded_nr,texture);							// save it in radio station struct to show
    } else if (strcmp(gfxfilename,"")==0) {
        // check hvis ikke noget navn om der findes en fil med radio station navnet *.png/jpg
        // hvis der gør load denne fil.
        strcpy(tmpfilename,"/usr/share/mythtv-controller/images/radiostations/");
        strcat(tmpfilename,radiooversigt.get_station_name(radio_oversigt_loaded_nr));
        strcat(tmpfilename,".png");
        if (file_exists(tmpfilename)) {
            texture=loadTexture ((char *) tmpfilename);                                 // load texture
            set_texture(radio_oversigt_loaded_nr,texture);                         		            // save it in radio station struct
            strncpy(stack[radio_oversigt_loaded_nr]->gfxfilename,get_station_name(radio_oversigt_loaded_nr),stationamelength-1);      // update station gfxfilename to station name
            strcat(stack[radio_oversigt_loaded_nr]->gfxfilename,".png");
            opdatere_radiostation_gfx(stack[radio_oversigt_loaded_nr]->intnr,stack[radio_oversigt_loaded_nr]->gfxfilename);           // and update db filename
        } else {
            strcpy(tmpfilename,"/usr/share/mythtv-controller/images/radiostations/");
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
    if (radio_oversigt_loaded_nr==radiooversigt.radioantal()) {
        radio_oversigt_loaded==true;
        radio_oversigt_loaded_done=true;
    } else radio_oversigt_loaded_nr++;
  }


//  printf("screen size %d \n",screen_size);

  switch(screen_size) {
      case 1: xof=-370.0f;
              yof= 260.0f;
              sofset=(_mangley/41)*5;
              bonline=5;					// numbers of icons in x direction
              xvgaz=-800.0f;
              lradiooversigt_antal=5*5;				// numbers of icons in y direction
              buttonsizex=88.0f;				// 68
              buttonsizey=68.0f;
              boffset=buttonsizey*1.4;
              break;
      case 2: xof=-380.0f;
              yof= 270.0f;				// 250.0f
              sofset=(_mangley/41)*5;
              bonline=5;
              xvgaz=-850.0f;
              lradiooversigt_antal=6*4;
              buttonsizex=88.0f;
              buttonsizey=78.0f;
              boffset=buttonsizey*1.4;
              break;
      case 3: xof=-660.0f;				// xof=-660.0f;
              yof= 336.0f;				// yof= 336.0f;
              sofset=(_mangley/40)*8;			//	sofset=(_mangley/41)*9;
              bonline=9;				// numbers of icons in x direction
              xvgaz=-1000.0f;				//
              lradiooversigt_antal=6*bonline;		// numbers of icons in y direction
              if (cur_avail_mem_kb==0) buttonsizex=73.0f;			// 83
              else buttonsizex=83.0f;
              buttonsizey=54.0f;			// 58
              boffset=buttonsizey*1.7;			// afstand mellem icons up/down
              break;
      case 4: xof=-660.0f;
              yof= 336.0f;
              sofset=(_mangley/41)*8;
              bonline=8;
              xvgaz=-1000.0f;
              lradiooversigt_antal=8*5;
              buttonsizex=96.0f;
              buttonsizey=86.0f;
              boffset=buttonsizey*1.4;
              break;
      default:
              xof=-380.0;
              yof= 250.0f;
              sofset=(_mangley/41)*5;
              bonline=5;
              xvgaz=-800.0f;
              lradiooversigt_antal=5*4;
              buttonsizex=68.0f;
              buttonsizey=58.0f;
              boffset=buttonsizey*1.4;
              break;
  }
  //
  // hvis der ikke brugfes nvidia core ret størelser
  //
  if (cur_avail_mem_kb==0) {
      buttonsizex-=20.0f;
      buttonsizey-=20.0f;
  }

//  printf("*Show music oversigt * antal = %d \n",musicoversigt_antal);
  // viser det antal radiostationer som kan være på skærmen på en gang
  // ellers er der et start ofset (sofset) som beskriver start ofset fra array (bliver rettet andet sted) pilup/pildown osv osv
  while((i<lradiooversigt_antal) && ((int) i+(int) sofset<(int) antal) && (stack[i+sofset]!=NULL)) {
    switch(screen_size) {
      case 1:
        if (((i % bonline)==0) && (i>0)) {
          yof=yof-(boffset+30);
          xof=-370;
        }
        break;
      case 2:
        if (((i % bonline)==0) && (i>0)) {
          yof=yof-(boffset+30);
          xof=-380;
        }
        break;
      case 3:
        if (((i % bonline)==0) && (i>0)) {
          yof=yof-(boffset+40);
          xof=-660;		// -660
        }
        break;
      case 4:
        if (((i % bonline)==0) && (i>0)) {
          yof=yof-(boffset+30);
          xof=-660;
        }
        break;
    }


    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    // draw mask
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    //glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);

    if (stack[i+sofset]->textureId) {
        glBindTexture(GL_TEXTURE_2D,onlineradiomask);					// stack[i+sofset]->textureId
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glBindTexture(GL_TEXTURE_2D,onlineradiomask);					// onlineradio_mask.(jpg/png)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glTranslatef(xof,yof,xvgaz);
    glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-(buttonsizex+buttonzoom), -(buttonsizey+buttonzoom), 0.0);
    glTexCoord2f(0, 1); glVertex3f(-(buttonsizex+buttonzoom),  buttonsizey+buttonzoom, 0.0);
    glTexCoord2f(1, 1); glVertex3f( buttonsizex+buttonzoom,  buttonsizey+buttonzoom, 0.0);
    glTexCoord2f(1, 0); glVertex3f( buttonsizex+buttonzoom, -(buttonsizey+buttonzoom), 0.0);
    glEnd(); //End quadrilateral coordinates


    glLoadIdentity();
    glBlendFunc(GL_ONE, GL_ONE);

    // er radio station online (hvis ja full color ellers 0.3)
    if (stack[i+sofset]->online) glColor4f(1.0f, 1.0f, 1.0f,1.0f); else glColor4f(.3f, .3f, .3f, 1.0f);

    // radio har et icon in db
    if (stack[i+sofset]->textureId) {
        // draw icon no indhold
        glBindTexture(GL_TEXTURE_2D,onlineradio_empty);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // zoom valgte
        if (i+1==(int) radio_key_selected) buttonzoom=22.0f;
        else buttonzoom=18.0f;

        glTranslatef(xof,yof,xvgaz);
        glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-(buttonsizex+buttonzoom), -(buttonsizey+buttonzoom), 0.0);
        glTexCoord2f(0, 1); glVertex3f(-(buttonsizex+buttonzoom),  buttonsizey+buttonzoom, 0.0);
        glTexCoord2f(1, 1); glVertex3f( buttonsizex+buttonzoom,  buttonsizey+buttonzoom, 0.0);
        glTexCoord2f(1, 0); glVertex3f( buttonsizex+buttonzoom, -(buttonsizey+buttonzoom), 0.0);
        glEnd(); //End quadrilateral coordinates



        // draw icon indhold
        glLoadIdentity();
        glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBlendFunc(GL_ONE, GL_ONE);
        // zoom valgte
        if (i+1==(int) radio_key_selected) buttonzoom=22.0f;
        else buttonzoom=18.0f-10;


        glTranslatef(xof,yof,xvgaz);
        glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-(buttonsizex+buttonzoom), -(buttonsizey+buttonzoom), 0.0);
        glTexCoord2f(0, 1); glVertex3f(-(buttonsizex+buttonzoom),  buttonsizey+buttonzoom, 0.0);
        glTexCoord2f(1, 1); glVertex3f( buttonsizex+buttonzoom,  buttonsizey+buttonzoom, 0.0);
        glTexCoord2f(1, 0); glVertex3f( buttonsizex+buttonzoom, -(buttonsizey+buttonzoom), 0.0);
        glEnd(); //End quadrilateral coordinates

    } else {
        // no db icon show default icon
        if (stack[i+sofset]->kbps==192) glBindTexture(GL_TEXTURE_2D,onlineradio192);
        else if (stack[i+sofset]->kbps==320) glBindTexture(GL_TEXTURE_2D,onlineradio320);
        else glBindTexture(GL_TEXTURE_2D,onlineradio);
        glBlendFunc(GL_ONE, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // zoom valgte
        if (i+1==(int) radio_key_selected) buttonzoom=22.0f;
        else buttonzoom=18.0f;

        glTranslatef(xof,yof,xvgaz);
        glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-(buttonsizex+buttonzoom), -(buttonsizey+buttonzoom), 0.0);
        glTexCoord2f(0, 1); glVertex3f(-(buttonsizex+buttonzoom),  buttonsizey+buttonzoom, 0.0);
        glTexCoord2f(1, 1); glVertex3f( buttonsizex+buttonzoom,  buttonsizey+buttonzoom, 0.0);
        glTexCoord2f(1, 0); glVertex3f( buttonsizex+buttonzoom, -(buttonsizey+buttonzoom), 0.0);
        glEnd(); //End quadrilateral coordinates

    }


    buttonzoom=0.0f;
    // draw flag
    if (stack[i+sofset]->land>0) {

        // draw mask
        glLoadIdentity();
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_DST_COLOR, GL_ZERO);
        glTranslatef(xof+38,yof-27,xvgaz);
        glBindTexture(GL_TEXTURE_2D,gfxlandemask);
        glColor4f(1.0f, 1.0f, 1.0f,1.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-16, -11, 0.0);
        glTexCoord2f(0, 1); glVertex3f(-16,  11, 0.0);
        glTexCoord2f(1, 1); glVertex3f( 16,  11, 0.0);
        glTexCoord2f(1, 0); glVertex3f( 16, -11, 0.0);
        glEnd(); //End quadrilateral coordinates

        // draw flag
        glLoadIdentity();
        glBlendFunc(GL_ONE, GL_ONE);
        glTranslatef(xof+38,yof-27,xvgaz);
        glColor4f(1.0f, 1.0f, 1.0f,1.0f);
        if (gfxlande[stack[i+sofset]->land]!=0)
            glBindTexture(GL_TEXTURE_2D,gfxlande[stack[i+sofset]->land]);
        else
            printf("Contry kode %d missing flag\n",stack[i+sofset]->land);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-16, -11, 0.0);
        glTexCoord2f(0, 1); glVertex3f(-16,  11, 0.0);
        glTexCoord2f(1, 1); glVertex3f( 16,  11, 0.0);
        glTexCoord2f(1, 0); glVertex3f( 16, -11, 0.0);
        glEnd(); //End quadrilateral coordinates
    }

    strcpy(temptxt,stack[i+sofset]->station_name);        // album navn
    lastslash=strrchr(temptxt,'/');
    if (lastslash) strcpy(temptxt,lastslash+1);
//    temptxt[13]=0;
//    txtbrede=t3dDrawWidth(musicoversigt[i+sofset].album_name);

    glPushMatrix();

    // show radio station name under icon

    float ytextofset=0.0f;
    int ii,j,k,pos;
    ii=pos=0;
    char word[16000];

    if (strlen(temptxt)<=14) {
        glLoadIdentity();
        ofs=(strlen(temptxt)/2)*9;
        switch(screen_size) {
            case 3: glTranslatef(xof-ofs,  yof-60-4 ,xvgaz);
                    break;
            case 4: glTranslatef(xof-ofs,  yof-60-10 ,xvgaz);
                    break;
            default:glTranslatef(xof-ofs,  yof-60 ,xvgaz);
                    break;

        }
        glRasterPos2f(0.0f, 0.0f);
        glDisable(GL_TEXTURE_2D);
        glScalef(14.0, 14.0, 1.0);
        glcRenderString(temptxt);
    } else {

        temptxt[26]='\0';
        glLoadIdentity();
        ofs=(strlen(temptxt)/2)*9;
        switch(screen_size) {
            case 3: glTranslatef(xof-50,  yof-60-4 ,xvgaz);
                    break;
            case 4: glTranslatef(xof-50,  yof-60-10 ,xvgaz);
                    break;
            default:glTranslatef(xof-50,  yof-60 ,xvgaz);
                    break;
        }
        glRasterPos2f(0.0f, 0.0f);
        glDisable(GL_TEXTURE_2D);
        glScalef(14.0, 14.0, 1.0);


        while((1) && (ytextofset<=10.0)) {		// max 2 linier
            j=0;
            while(!isspace(temptxt[ii])) {
                if (temptxt[ii]=='\0') break;
                word[j]=temptxt[ii];
                ii++;
                j++;
            }

            word[j]='\0';	// j = word length

            if (j>13) {		// print char by char
                k=0;
                while(word[k]!='\0') {
                    if (pos>=13) {
                        if ( k != 0 ) glcRenderChar('-');
                        pos=0;
                        ytextofset+=15.0f;
                        glLoadIdentity();
                        ofs=0;
                        switch(screen_size) {
                            case 3: glTranslatef(xof-50,  yof-60-4-ytextofset ,xvgaz);
                                    break;
                            case 4: glTranslatef(xof-50,  yof-60-10-ytextofset ,xvgaz);
                                    break;
                            default:glTranslatef(xof-50,  yof-60-ytextofset ,xvgaz);
                                    break;
                        }
                        glRasterPos2f(0.0f, 0.0f);
                        glScalef(14.0, 14.0, 1.0);
                    }
                    glcRenderChar(word[k]);
                    pos++;
                    k++;
                }
            } else {
                if (pos+j>13) {	// word doesn't fit line
                    ytextofset+=15.0f;
                    pos=0;
                    glLoadIdentity();
                    ofs=(int) (strlen(word)/2)*9;
                    switch(screen_size) {
                        case 3: glTranslatef(xof-50,  yof-60-4-ytextofset ,xvgaz);
                                break;
                        case 4: glTranslatef(xof-50,  yof-60-10-ytextofset ,xvgaz);
                                break;
                        default:glTranslatef(xof-50,  yof-60-ytextofset ,xvgaz);
                                break;
                    }
                    glRasterPos2f(0.0f, 0.0f);
                    glScalef(14.0, 14.0, 1.0);
                }
                glcRenderString(word);
                pos+=j;
            }
            if (pos<12) {
                glcRenderChar(' ');
                pos++;
            }

            if (temptxt[ii]=='\0') break;
            ii++;	// skip space
        }
    }
/*
     // lav ramme om valgte radio station
     if (i+1==radio_key_selected) {
          // lav valgte border
          glLoadIdentity();
          glTranslatef((xof), (yof), xvgaz);
          glColor3f(1.0f,1.0f,1.0f);
          glBindTexture(GL_TEXTURE_2D,0);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glBegin(GL_QUADS); //Begin quadrilateral coordinates

          // left
          glTexCoord2f(0.0, 0.0); glVertex3f(-72, -75, 0.0);
          glTexCoord2f(0.0, 1.0); glVertex3f(-75, -75, 0.0);
          glTexCoord2f(1.0, 1.0); glVertex3f(-75, 75-15, 0.0);
          glTexCoord2f(1.0, 0.0); glVertex3f(-72, 75-15, 0.0);
          // right
          glTexCoord2f(0.0, 0.0); glVertex3f(72, -75, 0.0);
          glTexCoord2f(0.0, 1.0); glVertex3f(75, -75, 0.0);
          glTexCoord2f(1.0, 1.0); glVertex3f(75, 75-15, 0.0);
          glTexCoord2f(1.0, 0.0); glVertex3f(72, 75-15, 0.0);

          // button
          glVertex3f(-75, -72, 0.0);
          glVertex3f(-75, -75, 0.0);
          glVertex3f(75, -75, 0.0);
          glVertex3f(75, -72, 0.0);

          // top
          glVertex3f(-75, 72-15, 0.0);
          glVertex3f(-75, 75-15, 0.0);
          glVertex3f(75, 75-15, 0.0);
          glVertex3f(75, 72-15, 0.0);
          glEnd();
    }

*/

    glPopMatrix();
    i++;

  //
  // hvis der ikke brugfes nvidia core ret størelser
  //
  // space on icons in x pos

    switch(screen_size) {
        case 1: if (cur_avail_mem_kb==0) xof+=(buttonsizex*2.0)+8; else  xof+=(buttonsizex*1.6);
                break;
        case 2: if (cur_avail_mem_kb==0) xof+=(buttonsizex*2.0)+8; else xof+=(buttonsizex*1.6);
                break;
        case 3: if (cur_avail_mem_kb==0) xof+=(buttonsizex*2.6)+8; else xof+=(buttonsizex*1.75);
                break;
        case 4: if (cur_avail_mem_kb==0) xof+=(buttonsizex*2.0)+8; else xof+=(buttonsizex*1.65);
                break;
        default:xof+=(buttonsizex*2)+8;
                break;
    }
  }


  if (radio_oversigt_loaded_nr<radiooversigt.radioantal()) {

    switch (screen_size) {
          case 1: loader_xpos=-250;
                  loader_ypos=-230+2;
                  break;
          case 2: loader_xpos=-250;
                  loader_ypos=-230+2;
                  break;
          case 3: loader_xpos=-380;
                  loader_ypos=-230+2;
                  break;
          case 4: loader_xpos=-250;
                  loader_ypos=-230+2;
                  break;
          default:loader_xpos=-250;
                  loader_ypos=-230+2;
                  break;
    }

    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
//    glDisable(GL_BLEND);
//    glBlendFunc(GL_DST_COLOR, GL_ZERO);
//    glBlendFunc(GL_ONE, GL_ONE);


    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);

    glTranslatef(loader_xpos,loader_ypos,-600);
    glBindTexture(GL_TEXTURE_2D,_textureIdloading_mask);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-170/2, -60/2, 0.0);
    glTexCoord2f(0, 1); glVertex3f(-170/2,  60/2, 0.0);
    glTexCoord2f(1, 1); glVertex3f( 170/2,  60/2, 0.0);
    glTexCoord2f(1, 0); glVertex3f( 170/2, -60/2, 0.0);
    glEnd(); //End quadrilateral coordinates

    glLoadIdentity();
    glTranslatef(loader_xpos,loader_ypos,-600);
    glBindTexture(GL_TEXTURE_2D,_textureIdloading);
    glBlendFunc(GL_ONE, GL_ONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-170/2, -60/2, 0.0);
    glTexCoord2f(0, 1); glVertex3f(-170/2,  60/2, 0.0);
    glTexCoord2f(1, 1); glVertex3f( 170/2,  60/2, 0.0);
    glTexCoord2f(1, 0); glVertex3f( 170/2, -60/2, 0.0);
    glEnd(); //End quadrilateral coordinates




    glLoadIdentity();
    glTranslatef(loader_xpos-36,loader_ypos-16,-600);
    glRasterPos2f(0.0f, 0.0f);
    glDisable(GL_TEXTURE_2D);
    glScalef(8.0, 8.0, 1.0);

    sprintf(temptxt,"%4d of %4d ",radio_oversigt_loaded_nr,radiooversigt.radioantal());

    glcRenderString(temptxt);
  }


  if (radiooversigt.radioantal()<1) {

    glLoadIdentity();
    glTranslatef(-10,10,-600);
    if (_textureIdloading1) {
        glBindTexture(GL_TEXTURE_2D,_textureIdloading1);
        glBlendFunc(GL_ONE,GL_ZERO);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-180, -60, 0.0);
        glTexCoord2f(0, 1); glVertex3f(-180,  60, 0.0);
        glTexCoord2f(1, 1); glVertex3f( 180,  60, 0.0);
        glTexCoord2f(1, 0); glVertex3f( 180, -60, 0.0);
        glEnd(); //End quadrilateral coordinates
    }
    glLoadIdentity();
    glTranslatef(-62,6,-300);
    glRasterPos2f(0.0f, 0.0f);
    glDisable(GL_TEXTURE_2D);
    glScalef(8.0, 8.0, 1.0);
    sprintf(temptxt,"No radiostations loaded.");
    glcRenderString(temptxt);

    if (configmythtvver==0) {
        glLoadIdentity();
        glTranslatef(-52,-6,-300);
        glRasterPos2f(0.0f, 0.0f);
        glDisable(GL_TEXTURE_2D);
        glScalef(8.0, 8.0, 1.0);
        strcpy(temptxt,"No backend ");
        strcat(temptxt,configmysqlhost);
        glcRenderString(temptxt);
    }
  }
}





// skal vi opdatere sort type oversigt første gang

static bool hentradioart=false;


void radiostation_class::show_radio_options() {
    int i;
    char sqlselect[512];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
//    int art,intnr,kbps;
//    GLuint texture;
    char *database = (char *) "mythtvcontroller";
    char tmptxt[80];
    float sizex=4.0f;
    float sizey=3.0f;

    int winsizx=1200;
    int winsizy=800;
    int xpos=0;
    int ypos=0;

    // mask
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glColor4f(1.0f,1.0f,1.0f,0.8f);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glBindTexture(GL_TEXTURE_2D, radiooptionsmask); 					//_textureId18_1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    // draw  front box
    glTexCoord2f(0, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2)) , 0.0);
    glEnd();
    glPopMatrix();

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
        if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
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





// *********************************************************************************
// opdatere list set numbers of aflytninger

int radiostation_class::set_radio_popular(int stationid) {
    char sqlselect[512];
    MYSQL *conn;
    MYSQL_RES *res;
    char *database = (char *) "mythtvcontroller";
    sprintf(sqlselect,"update radio_stations set popular=popular+1,lastplayed=now() where intnr=%ld",stack[stationid]->intnr);
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, database, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        mysql_close(conn);
        return(1);
    }
    return(0);
}





// *********************************************************************************
// set online/offline status on radio station (stationid)
//

int radiostation_class::set_radio_online(int stationid,bool onoff) {
    char sqlselect[512];
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *database = (char *) "mythtvcontroller";
    if (onoff) sprintf(sqlselect,"update radio_stations set online=1 where intnr=%ld",stack[stationid]->intnr);
        else sprintf(sqlselect,"update radio_stations set online=0 where intnr=%ld",stack[stationid]->intnr);
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, database, 0, NULL, 0)) {
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


// get radio online flag

int radiostation_class::get_radio_online(int stationid) {
    if (((unsigned int) stationid<(unsigned int) antal) && (stack[stationid])) {
        return(stack[stationid]->online);
    } else {
        return(-1);
    }
}




// set radio station online flag internal use

int radiostation_class::set_radio_intonline(int arraynr) {
    if (((unsigned int) arraynr<(unsigned int) antal) && (stack[arraynr]))
       stack[arraynr]->online=true; else return(0);
    return(1);
}




// check if radio station is ofline in database and return true if redio station exist in db

bool radiostation_class::check_radio_online_bool() {
    char sqlselect[512];
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *database = (char *) "mythtvcontroller";
    conn=mysql_init(NULL);
    strcpy(sqlselect,"select intnr from radio_stations where online=0 order by intnr limit 100");
    if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, database, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        if (res) {
        }
        mysql_close(conn);
        return(1);
    } else return(0);		// we are done check all radio stations in database
}



int radiostation_class::set_radio_aktiv(int stationid,bool onoff) {
//    stack[stationid]->intnr;
    stack[stationid]->aktiv=onoff;
    return(0);
}




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


// returm portnr or null

int get_url_data(char *url,char *ipadd) {
    char *sted;
    char *langurl=NULL;
    int destport=0;
    sted=(char*) strrchr(url, ':');		// find : fra start af url
    if (sted) {
        if (strncmp(sted,"://",3)!=0) {		// er vi fra start af url
            destport=atoi(sted+1);		// hent port
            if (strncmp(url,"http",4)==0) {
                *sted='\0';
                strcpy(ipadd,url+7); 		// get rest of url
            } else if (strncmp(url,"mms",3)==0) {
                *sted='\0';
                strcpy(ipadd,url+6); 		// get rest of url
            }
            langurl=strchr(ipadd, '/');		// find first /
            if  (langurl) {
                *langurl='\0';
            }
        } else {
            if (strncmp(url,"http",4)==0) {
                strcpy(ipadd,url+7);		// get rest of url
            }
            if (strncmp(url,"mms",3)==0) {
                strcpy(ipadd,url+6);            // get rest of url
            }

            langurl=strchr(ipadd, '/');		// find first /
            if  (langurl) {
                *langurl='\0';
            }
        }
    }
    return(destport);
}




// check if radio station is ofline in database and set it online again if it is back online
// return where we are ind the database
// if start recordnr>0 do it

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
    char *database = (char *) "mythtvcontroller";
    int nn;
    bool nfundet;
    bool radiook=false;
    bool cerror;


    struct timeval tv;
    fd_set myset;

    if (debugmode) printf("*Check radio stations*\n");

    if (check_radio_online_switch) {
        conn=mysql_init(NULL);
        strcpy(sqlselect,"select name,aktiv,intnr,stream_url from radio_stations where online=1 and aktiv=1 order by popular desc,name limit 1");
        if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, database, 0, NULL, 0)) {
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

                        if (debugmode) fprintf(stderr,"Checking Station : %-50s - hostname : %s ",row[0],hostname);

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
                                    if (debugmode) fprintf(stderr," Station OK. \n ");
                                    radiook=true;
                                } else radiook=false;
                            } else {
                                if (debugmode) fprintf(stderr," Station BAD. \n ");
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
            mysql_close(conn);
        }
    }
    return(radiostation);		// we are done check all radio stations in database
//	    return(1);			// enable to task to check 4 ever
}
