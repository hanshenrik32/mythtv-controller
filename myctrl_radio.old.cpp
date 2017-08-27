#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>  
#include <GL/glu.h> 
#include <string.h>
#include <mysql.h>
#include <GL/glc.h>

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
extern int screen_size;
extern int screensizey;
extern int screeny;
extern int debugmode;
extern unsigned int musicoversigt_antal;
extern int radio_key_selected;
extern int music_select_iconnr;
extern int do_music_icon_anim_icon_ofset;
extern GLuint _textureIdback1;
extern GLuint onlineradio;
extern GLuint onlineradio192;
extern GLuint onlineradio320;
extern GLuint onlineradiomask;
extern GLuint radiooptions,radiooptionsmask;
extern int fonttype;
extern fontctrl aktivfont;

extern GLuint gfxlande[45];
extern GLuint gfxlandemask;

extern radiostation_class radiooversigt;

extern GLint cur_avail_mem_kb;

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


void radiostation_class::set_texture(int nr,GLuint idtexture) {
    stack[nr]->textureId=idtexture; 
}

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

void radiostation_class::load_radio_stations_gfx() {
    int i=0;
    GLuint texture;
    char tmpfilename[200];
    char gfxfilename[40];
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
            printf("Radio station gfx file %s for %s is missing.\n",get_station_gfxfile(i),get_station_name(i));
        }
        i++;
    }
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
    char tmptxt1[80];
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
        mysql_close(conn);
        if (antal>0) return(antal-1); else return(0);
    } else printf("Failed to connect to database: Error: %s\n",mysql_error(conn));
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
        strcpy(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 order by popular desc,name");
    else if (radiosortorder==28)		// bit rate
        sprintf(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 order by bitrate desc,popular desc,name");
    else if (radiosortorder==27)		// land kode
        sprintf(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 order by landekode desc,popular desc,name");
    else if (radiosortorder==19)		// mest hørt
        sprintf(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 order by lastplayed desc,popular desc,name");
    else 					// ellers efter art
        sprintf(sqlselect,"select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and art=%d order by popular desc,name",radiosortorder);
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        if (res) {
            while (((row = mysql_fetch_row(res)) != NULL) && (antal<maxantal)) {
            //            printf("Hent info om radio station nr %4d %-20s\n",antal,row[0]);
            
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
        mysql_close(conn);
        //load_radio_stations_gfx();
        return(antal-1);
    } else printf("Failed to update radiodb, can not connect to database: Error: %s\n",mysql_error(conn));
    return(0);
}



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
  float buttonsizex=68.0f;
  float buttonsizey=58.0f;
  float buttonzoom=0.0f;
//  float scale=1.0f;
  float boffset;
  switch(screen_size) {
      case 1: xof=-370.0f;
              yof= 260.0f;
              sofset=(_mangley/41)*5;
              bonline=5;					// numbers of icons in x direction
              xvgaz=-800.0f;
              lradiooversigt_antal=5*5;				// numbers of icons in y direction
              buttonsizex=88.0f;				// 68
              buttonsizey=68.0f;
              break;
      case 2: xof=-380.0f;
              yof= 270.0f;				// 250.0f
              sofset=(_mangley/41)*5;
              bonline=5;
              xvgaz=-850.0f; 
              lradiooversigt_antal=6*4;
              buttonsizex=88.0f;
              buttonsizey=78.0f;
              break;
      case 3: xof=-660.0f;
              yof= 336.0f;
              sofset=(_mangley/41)*9;
              bonline=9;
              xvgaz=-1000.0f;
              lradiooversigt_antal=9*6;
              buttonsizex=84.0f;
              buttonsizey=74.0f;
              break;
      case 4: xof=-660.0f;
              yof= 336.0f;
              sofset=(_mangley/41)*8;
              bonline=8;
              xvgaz=-1000.0f;
              lradiooversigt_antal=8*5;
              buttonsizex=96.0f;
              buttonsizey=86.0f;
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
              break;
  }
  boffset=buttonsizey*1.4;
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
          yof=yof-(boffset+30);
          xof=-660;
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
    // mask
    glEnable(GL_BLEND);     
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    if (stack[i+sofset]->textureId) {
        glBindTexture(GL_TEXTURE_2D,onlineradiomask);					// stack[i+sofset]->textureId
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glBindTexture(GL_TEXTURE_2D,onlineradiomask);
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
    
    if (stack[i+sofset]->textureId) {
        glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        if (stack[i+sofset]->kbps==192) glBindTexture(GL_TEXTURE_2D,onlineradio192); 
        else if (stack[i+sofset]->kbps==320) glBindTexture(GL_TEXTURE_2D,onlineradio320);
        else glBindTexture(GL_TEXTURE_2D,onlineradio);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // zoom valgte    
    if (i+1==(int) radio_key_selected) buttonzoom=8.0f;
    else buttonzoom=0.0f;

    glTranslatef(xof,yof,xvgaz);
    glRotatef(45.0f, 0.0f, 0.0f, 0.0f);    
    glLoadName(100+i+sofset);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-(buttonsizex+buttonzoom), -(buttonsizey+buttonzoom), 0.0);
    glTexCoord2f(0, 1); glVertex3f(-(buttonsizex+buttonzoom),  buttonsizey+buttonzoom, 0.0);
    glTexCoord2f(1, 1); glVertex3f( buttonsizex+buttonzoom,  buttonsizey+buttonzoom, 0.0);
    glTexCoord2f(1, 0); glVertex3f( buttonsizex+buttonzoom, -(buttonsizey+buttonzoom), 0.0);
    glEnd(); //End quadrilateral coordinates    

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
    switch(screen_size) {
        case 1: if (cur_avail_mem_kb==0) xof+=(buttonsizex*2.0)+8; else  xof+=(buttonsizex*1.6);
                break; 
        case 2: if (cur_avail_mem_kb==0) xof+=(buttonsizex*2.0)+8; else xof+=(buttonsizex*1.6);
                break;
        case 3: if (cur_avail_mem_kb==0) xof+=(buttonsizex*2.0)+8; else xof+=(buttonsizex*1.7);
                break;
        case 4: if (cur_avail_mem_kb==0) xof+=(buttonsizex*2.0)+8; else xof+=(buttonsizex*1.65);
                break;
        default:xof+=(buttonsizex*2)+8;
                break;
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

    // mask
    glPushMatrix();
    glLoadIdentity(); //Reset the drawing perspective
    glEnable(GL_TEXTURE_2D);
    glTranslatef(-1.0f, -0.00f, -10.0f); 
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glColor4f(1.0f,1.0f,1.0f,0.8f);
    glEnable(GL_BLEND);     
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);    
    glBindTexture(GL_TEXTURE_2D, radiooptionsmask); 					//_textureId18_1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates    
    // draw  front box
    glTexCoord2f(0.0, 0.0); glVertex3f(-sizex, -sizey, 0);
    glTexCoord2f(1.0, 0.0); glVertex3f( sizex-0.5f, -sizey, 0);
    glTexCoord2f(1.0, 1.0); glVertex3f( sizex-0.5f, sizey, 0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-sizex, sizey, 0);
    glEnd();
    glPopMatrix();

    // back
    glPushMatrix();
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
//  glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glTranslatef(-1.0f, -0.00f, -10.0f); 
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f); 
    glBindTexture(GL_TEXTURE_2D, radiooptions);					// _textureId18);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates    
    // draw  front box
    glTexCoord2f(0.0, 0.0); glVertex3f(-sizex, -sizey, 0);
    glTexCoord2f(1.0, 0.0); glVertex3f( sizex-0.5f, -sizey, 0);
    glTexCoord2f(1.0, 1.0); glVertex3f( sizex-0.5f, sizey, 0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-sizex, sizey, 0);
    glEnd();

    i=0;
    if (hentradioart==false) {
        hentradioart=true;
        strcpy(sqlselect,"SELECT typename,radiotypes.art,count(radio_stations.art) FROM `radiotypes`,radio_stations where radiotypes.art=radio_stations.art or radiotypes.art=0 group by (radiotypes.art)");
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
    glDisable(GL_TEXTURE_2D);
    glLoadIdentity();
    glTranslatef(-8.0f, 10.0f*(1.00f), -40.0f);
    glcScale(0.2, 0.2);
    glcRenderString("Sort options.");
    
    i=0;
    while ((strcmp(radiosortopt[i].radiosortopt,"")!=0) && (i<40)) {
        glLoadIdentity();
        if (i<18) glTranslatef(-18.0f, (8-i)*(1.00f), -40.0f); else glTranslatef(-4.0f, ((18-i)*(1.00f))+8.0f, -40.0f); 
        if (i==radiooptionsselect) glColor4f(0.0f,0.0f,1.0f,1.0f); else glColor4f(1.0f,1.0f,1.0f,1.0f);
        glcRenderString(radiosortopt[i].radiosortopt);
        glLoadIdentity();
        if (i<18) glTranslatef(-7.0f, (8-i)*(1.00f), -40.0f); else glTranslatef(6.0f, ((18-i)*(1.00f))+8.0f, -40.0f); 
        if (i==radiooptionsselect) glColor4f(0.0f,0.0f,1.0f,1.0f); else glColor4f(1.0f,1.0f,1.0f,1.0f);
        if ((radiosortopt[i].radiosortoptart!=27) && (radiosortopt[i].radiosortoptart!=28)) {
            sprintf(tmptxt,"%4d",radiosortopt[i].antal);
            glcRenderString(tmptxt);
        }
        i++;
    }
    glPopMatrix();
}





// *********************************************************************************
// opdatere list set numbers of aflytninger

int radiostation_class::set_radio_popular(int stationid) {
    char sqlselect[512];
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
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
    return(0);
}

int radiostation_class::set_radio_intonline(int arraynr) {
    if (((unsigned int) arraynr<(unsigned int) antal) && (stack[arraynr])) stack[arraynr]->online=true; else return(0);
    return(1);
}

