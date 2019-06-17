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
#include <GL/glc.h>       		// danish ttf support
// mysql support
#include <mysql.h>
#include "text3d.h"
#include "readjpg.h"
#include "utility.h"
#include "myctrl_recorded.h"
#include "myctrl_storagedef.h"

extern int fonttype;
extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];
extern char configrecordpath[256];
extern recorded_overigt recordoversigt;
extern bool reset_recorded_texture;
extern storagedef configstoragerecord[];      // storage array (for recorded programs)
extern GLuint _textureId22;    // movie open info box1
extern GLuint _textureId23;    // movie open info box2
extern GLuint _textureId24;    // movie open info box3
extern int screen_size;

// constructor
// this fault why ?

recorded_oversigt_type::recorded_oversigt_type() {
//    strcpy(title,"");
//    strcpy(subtitle,"");
//    strcpy(recordedpath,"");
//    strcpy(startdato,"");
//    strcpy(enddato,"");
//    strcpy(description,"");
}


void recorded_oversigt_type::put_recorded(char *tit,char *subtit,char *startdag,char *enddag,char *filepath,char *desc)

{
    strcpy(title,tit);
    strcpy(subtitle,subtit);
    strcpy(recordedpath,filepath);
    strcpy(startdato,startdag);
    strcpy(enddato,enddag);
    strncpy(description,desc,200);
    description[199]=0;
}


void recorded_oversigt_type::get_recorded(int recnr,char *tit,char *subtit,char *startdag,char *enddag,char *desc)

{
    strcpy(tit,title);
    strcpy(subtit,subtitle);
    strcpy(startdag,startdato);
    strcpy(enddag,enddato);
    strcpy(desc,description);
}

void recorded_oversigt_type::get_recorded_filepath(char *filepath)

{
    strcpy(filepath,recordedpath);
}


// constructor

recorded_top_type::recorded_top_type()

{
    strcpy(title,"");
    prg_antal=0;
}



void recorded_top_type::put_recorded_top(char *title)

{
    strcpy(this->title,title);
}


void recorded_top_type::get_recorded_top(char *title)

{
    strcpy(title,this->title);
}



void draw_ccover() {

    int zofset=0;
    int xofset=0;
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0.0, 0.0); glVertex3f(-3.5+xofset, -3.5, 0.0-zofset);
    glTexCoord2f(0.0, 1.0); glVertex3f(-3.5+xofset, 3.5, 0.0-zofset);
    glTexCoord2f(1.0, 1.0); glVertex3f(3.5+xofset, 3.5, 0.0-zofset);
    glTexCoord2f(1.0, 0.0); glVertex3f(3.5+xofset, -3.5, 0.0-zofset);
    glEnd(); //End quadrilateral coordinates
}



void recorded_overigt::get_recorded_filepath(char *filepath,int valgtnr,int subvalgtnr)

{
    char temptxt[255];
    programs[valgtnr].recorded_programs[subvalgtnr].get_recorded_filepath(temptxt);
    strcpy(filepath,temptxt);
}


int find_storagegroupfile(char *filename) {
    bool fundet=false;
    char filepath[512];
    int storagegrpnr=0;
    while((!(fundet)) && (storagegrpnr<storagegroupantal)) {
      strcpy(filepath,configstoragerecord[storagegrpnr].path);
      strcat(filepath,filename);
      if (file_exists(filepath)) {
        fundet=true;
        strcpy(filename,filepath);
      }
      storagegrpnr++;
    }
    if (fundet) return(1); else return(0);
}


int recorded_overigt::opdatere_recorded_oversigt() {
    char sqlselect[1024];
    char title[128];
    int n,nn;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *database = (char *) "mythconverg";
    char filename[512];
    int storagegroupused=0;
    //gotoxy(10,17);
    // Connect to database
    conn=mysql_init(NULL);
    if (conn) {
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      mysql_query(conn,"set NAMES 'utf8'");
      strcpy(sqlselect,"create table IF NOT EXISTS recorded(chanid int, starttime datetime, endtime datetime, title varchar(128), subtitle varchar(128), description text, season int,episode int ,category varchar(64), hostname varchar(255), bookmark int,editing int, cutlist int,autoexpire int, commflagged int,recgroup varchar(32), recordid int, seriesid varchar(64), inetref varchar(64), lastmodified datetime, filesize int.stars float, previouslyshown int, originalairdate date, preserve int, findid int, deletepending int, transcoder int, timestretch float, recpriority int,basename  varchar(255), progstart datetime, progend datetime, playgroup varchar(32), profile varchar(32), duplicate int, transcoded int, watched int, storagegroup varchar(32), bookmarkupdate datetime)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS recordedprogram(chanid int(10) unsigned,starttime datetime,endtime datetime,title varchar(128),subtitle varchar(128),description text,category varchar(64),category_type varchar(64),airdate year(4),stars float unsigned,previouslyshown tinyint(4),title_pronounce varchar(128),stereo tinyint(1),subtitled tinyint(1),hdtv tinyint(1),closecaptioned tinyint(1),partnumber int,parttotal int,seriesid varchar(12),originalairdate date,showtype varchar(30),colorcode  varchar(20),syndicatedepisodenumber varchar(20),programid varchar(64),manualid int(10) unsigned,generic tinyint(1),listingsource int(11),first tinyint(1),last tinyint(1),audioprop varchar(20),subtitletypes varchar(20),videoprop varchar(20))");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      printf("Update recorded programs from mythtv database.\n");
      strcpy(sqlselect,"select title,subtitle,starttime,endtime,basename,description from recorded order by title,starttime desc");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      n=-1;
      nn=0;
      strcpy(title,"");
      if (res) {
          while ((row = mysql_fetch_row(res)) != NULL) {
              if (strcmp(title,row[0])!=0) {
                n++;
                nn=0;
                strcpy(title,row[0]);
              } else {
                if (n==-1) n++;
                nn++;
              }
              // printf("title=%s n=%d nn=%d  \n",title,n,nn);
              strcpy(filename,row[4]);
              storagegroupused=find_storagegroupfile(filename);						// hent storage group info
              if (storagegroupused==0) strcpy(filename,row[4]);
              if ((n<40) && (nn<200)) {
                recordoversigt.programs[n].put_recorded_top(title);
                recordoversigt.programs[n].recorded_programs[nn].put_recorded(title,row[1],row[2],row[3],filename,row[5]);
                recordoversigt.programs[n].prg_antal++;
              }
          }        	// end while
      } else {
        printf("SQL DATBASE ERROR                                                    \n");
        n=0;
      }
      set_top_antal(n);					// sætter hvor mange der er i array
      if (n>0) {
        printf("Fundet %d recorded programs.                                                \n",n);
      }
      mysql_close(conn);
    }
    return(n);
}


void myglprint3(char *string)
{
    int len,i;
    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
    }
}


void stroke_output1(GLfloat x, GLfloat y, char *format,...) {
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


// Viser recorded programer
// valgtnr= den valgte som skal vises med mark
// subvalgtnr = den valgte som skal vises i under liste

void recorded_overigt::show_recorded_oversigt(int valgtnr,int subvalgtnr) {
    static GLuint texture=0;
    unsigned int i=0;
    int ii,iii;
    unsigned int startofset=0;
    unsigned int substartofset=0;
    float glprintyofset=0.0f;
    float screen_zofset;
    char title[128];
    char subtitle[128];
    char startdato[40];
    char slutdato[40];
    char desc[200];			// desc
    float yofset=0.5f;
    float yof=0.0f;
    char temptxt[256];
    char temptxt1[256];
    if (valgtnr>10) {
      startofset=valgtnr-10;
      valgtnr=10;
    } else startofset=0;
    switch(screen_size) {
        case 0: screen_zofset=0;
                break;
        case 1: screen_zofset=0;
                break;
        case 2: screen_zofset=-5;
                break;
        case 3: screen_zofset=-1;
                break;
    }
    // box2
    glLoadIdentity();
    glTranslatef(-2.0f, 4.0f,-15.0f+screen_zofset);
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_textureId23);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0.0, 0.0); glVertex3f(-6, -4.5f, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-6, 2.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(-1, 2.0, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1, -4.5f, 0.0);
    glEnd(); //End quadrilateral coordinates
    // box3
    glLoadIdentity();
    glTranslatef(3.5f, 4.0f,-15.0f+screen_zofset);
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_textureId24);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0.0, 0.0); glVertex3f(-6, -4.5f, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-6, 2.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1, 2.0, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1, -4.5f, 0.0);
    glEnd(); //End quadrilateral coordinates
    // box1
    glLoadIdentity();
    glTranslatef(-3.0f, -2.7f,-15.0f+screen_zofset);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_textureId22);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0.0, 0.0); glVertex3f(-5, -2.5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-5, 1.5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(7, 1.5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(7, -2.5, 0.0);
    glEnd(); //End quadrilateral coordinates
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    while ((i<=recordoversigt.top_antal()) && (i<11)) { 				// vis max
        recordoversigt.programs[i+startofset].recorded_programs[0].get_recorded(i,title,subtitle,startdato,slutdato,desc);
        if (i+startofset==valgtnr+startofset) {
          glLoadIdentity();
          glColor3f(0.0f, 0.0f, 1.0f);
          glTranslatef(-7.8f, 5.1f+yof,-14.9f+screen_zofset);
          glBegin(GL_QUADS); //Begin quadrilateral coordinates
          glVertex3f(-0.0f, -0.2f, 0.0f);
          glVertex3f(-0.0f, 0.3f, 0.0f);
          glVertex3f(4.5f, 0.3f, 0.0f);
          glVertex3f(4.5f, -0.2f, 0.0f);
          glEnd(); //End quadrilateral coordinates
        }
        glLoadIdentity();
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(-7.7f, 5.0f+yof,-14.8f+screen_zofset);
        glRasterPos2f(0.0f, 0.0f);
        strcpy(temptxt,title);
        if ((i+startofset)==valgtnr+startofset) {
          temptxt[25]=0;
          glScalef(0.25f, 0.25f, 0.25f);
          glcRenderString(temptxt);
        } else {
          temptxt[28]=0;
          glScalef(0.2f, 0.2f, 0.2f);
          glcRenderString(temptxt);
        }
        i++;
        yof-=yofset;
    }
    yof=0;
    i=0;
    if (subvalgtnr>10) {
      substartofset=subvalgtnr-10;
      subvalgtnr=10;
    } else substartofset=0;

    while ((i<recordoversigt.programs[valgtnr+startofset].prg_antal) && (i<11)) { 				// vis max
        recordoversigt.programs[valgtnr+startofset].recorded_programs[i+substartofset].get_recorded(i+substartofset,title,subtitle,startdato,slutdato,desc);
        if (i+substartofset==subvalgtnr+substartofset) {
          glLoadIdentity();
          glColor3f(0.0f, 0.0f, 1.0f);
          glTranslatef(-1.7f, 5.1f+yof,-14.9f+screen_zofset);
          glBegin(GL_QUADS); //Begin quadrilateral coordinates
          glVertex3f(-0.5, -0.2, 0.0);
          glVertex3f(-0.5, 0.3, 0.0);
          glVertex3f(5.9, 0.3, 0.0);
          glVertex3f(5.9, -0.2, 0.0);
          glEnd(); //End quadrilateral coordinates
        }
        glLoadIdentity();
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(-2.1f, 5.0f+yof,-14.8f+screen_zofset);
        glRasterPos2f(0.0f, 0.0f);
        strcpy(temptxt,subtitle);
        if (strcmp(temptxt,"")==0) strcpy(temptxt,desc);		// Hvis der ikke er nogle subtitle bruge description i stedet
        temptxt[26]=0;
        if (i+substartofset==subvalgtnr+substartofset) {
          glScalef(0.25f, 0.25f, 0.25f);
          glcRenderString(temptxt);
        } else {
          glScalef(0.2f, 0.2f, 0.2f);
          glcRenderString(temptxt);
        }
        i++;
        yof-=yofset;
    }
    recordoversigt.programs[valgtnr+startofset].recorded_programs[subvalgtnr+substartofset].get_recorded(subvalgtnr,title,subtitle,startdato,slutdato,desc);
    glLoadIdentity();
    glTranslatef(-4.0f, -2.0f,-14.8f+screen_zofset);
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(0.0f, 0.0f);
    glScalef(0.25f, 0.25f, 0.25f);
    glcRenderString("Date...:");
    strcpy(temptxt,startdato);
    temptxt[10]=0;
    glcRenderString(temptxt);
    strcpy(temptxt,startdato+11);
    temptxt[5]=0;
    glcRenderString(" KL.:");
    glcRenderString(temptxt);
    static time_t tm;				// this time (now)
    struct tm *nutid;
    struct tm prgstarttid;
    struct tm prgsluttid;
    char dagsdag[40];
    tm=time(0);				// get time
    nutid=localtime(&tm);
    strftime(dagsdag, 40, "%Y-%m-%d %H:%M:%S", nutid );
    strptime(startdato,"%Y-%m-%d %H:%M:%S",&prgstarttid);
    strptime(slutdato,"%Y-%m-%d %H:%M:%S",&prgsluttid);
    prgstarttid.tm_isdst=1;
    prgsluttid.tm_isdst=1;
    time_t st=mktime(&prgstarttid);
    time_t et=mktime(&prgsluttid);
    if ((tm>st) && (tm<et)) {				// er tiden inden for nu så hvis at vi optager live nu
      glColor3f(1.0f, 0.0f, 0.0f);
      glScalef(0.2f, 0.2f, 0.2f);
      glcRenderString((char *) "Recording");
    }
    recordoversigt.programs[valgtnr+startofset].recorded_programs[subvalgtnr].get_recorded_filepath(temptxt1);
    strcat(temptxt1,".png");
    strcpy(temptxt,temptxt1);
    // check local paths
    if (!(file_exists(temptxt))) {
      strcpy(temptxt,"~/.mythtv/remotecache/");
      strcat(temptxt,temptxt1);
      //printf("Not found. Check %s \n",temptxt);
    }
    if (file_exists(temptxt)) {
      glLoadIdentity();
      glTranslatef(-5.9f, -3.5f,-14.8f+screen_zofset);
      glRotatef(0.0f, 0.0f,0.0f, 0.0f);
      glEnable(GL_BLEND);
      glDisable(GL_DEPTH_TEST);
      glBlendFunc(GL_DST_COLOR, GL_ZERO);
      glEnable(GL_TEXTURE_2D);
      //glBindTexture(GL_TEXTURE_2D, _textureId25);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS); //Begin quadrilateral coordinates
      glTexCoord2f(0.0, 0.0); glVertex3f(-1.55f, -1.2f, 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(-1.55f, 1.2f, 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(1.55f, 1.2f, 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(1.55f, -1.2f, 0.0);
      glEnd(); //End quadrilateral coordinates
      if ((texture) && (reset_recorded_texture)) {
        reset_recorded_texture=false;				// load kun texture 1 gang
        glDeleteTextures (1, &texture);
        texture=0;
      }
      if (!(texture)) texture=loadTexture(temptxt);
      glLoadIdentity();
      glTranslatef(-5.9f, -3.5f,-14.8f+screen_zofset);
      glRotatef(0.0f, 0.0f,0.0f, 0.0f);
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE,GL_ONE);
      glBindTexture(GL_TEXTURE_2D,texture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS); //Begin quadrilateral coordinates
      glTexCoord2f(0.0, 0.0); glVertex3f(-1.55f, -1.2f, 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(-1.55f, 1.2f, 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(1.55f, 1.2f, 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(1.55f, -1.2f, 0.0);
      glEnd(); //End quadrilateral coordinates
    }
    glDisable(GL_TEXTURE_2D);
    ii=0;
    iii=0;
    while (ii<(int) strlen((char *) desc)) {
      // check max linie længe
      if (iii<41)  {
        temptxt[iii]=desc[ii];
        temptxt[iii+1]=0;
        iii++;
        ii++;
      } else {
        glLoadIdentity();
        glTranslatef(-4.1f, -2.6f+glprintyofset,-14.8f+screen_zofset);
        glRasterPos2f(0.0f, 0.0f);
        glScalef(0.32f, 0.32f, 0.32f);
        glcRenderString(temptxt);
        temptxt[0]=0;
        iii=0;
        glprintyofset-=0.3;
      }
    }
    // er der mere tekst
    if (iii>0) {
      glLoadIdentity();
      glTranslatef(-4.1f, -2.6f+glprintyofset,-14.8f+screen_zofset);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(0.32f, 0.32f, 0.32f);
      glcRenderString(temptxt);
    }
    //printf("valgtnr=%d subvalgtnr=%d antal= %d \n ",valgtnr,subvalgtnr,recordoversigt.top_antal());
}





void recorded_overigt::show_recorded_oversigt1(int valgtnr,int subvalgtnr) {
    static GLuint texture=0;
    unsigned int i=0;
    int ii,iii;
    unsigned int startofset=0;
    unsigned int substartofset=0;
    float glprintyofset=0.0f;
    float screen_zofset;
    char title[128];
    char subtitle[128];
    char startdato[40];
    char slutdato[40];
    char desc[200];			// desc
    float yofset=0.5f;
    float yof=0.0f;
    char temptxt[256];
    char temptxt1[256];
    int xpos,ypos;
    if (valgtnr>10) {
      startofset=valgtnr-10;
      valgtnr=10;
    } else startofset=0;
    // box2 mask
    glTranslatef(0.0f, 0.0f,0);
    xpos=100;
    ypos=250;
    // box2
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_textureId23);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(xpos+100, ypos+100, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(xpos+100, ypos+600, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xpos+500, ypos+600, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xpos+500, ypos+100, 0.0);
    glEnd();
    xpos=510;
    ypos=250;
    // box2
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_textureId24);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(xpos+100, ypos+100, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(xpos+100, ypos+600, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xpos+800, ypos+600, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xpos+800, ypos+100, 0.0);
    glEnd();
    //box3 mask
    xpos=100;
    ypos=40;
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(xpos+100, ypos+100, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(xpos+100, ypos+300, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xpos+1210, ypos+300, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xpos+1210, ypos+100, 0.0);
    glEnd();
    // box3
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,_textureId22);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(xpos+100, ypos+100, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(xpos+100, ypos+300, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xpos+1210, ypos+300, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xpos+1210, ypos+100, 0.0);
    glEnd();
    glPushMatrix();
    glTranslatef(210.0f, 820.0f,0);
    glRasterPos2f(0.0f, 0.0f);
    glScalef(20.0f, 20.0f, 1.0f);
    glcRenderString("temptxt");
    glPopMatrix();
    while ((i<=recordoversigt.top_antal()) && (i<11)) { 				// vis max
      glPushMatrix();
      recordoversigt.programs[i+startofset].recorded_programs[0].get_recorded(i,title,subtitle,startdato,slutdato,desc);
      // hvis valgte
      if (i+startofset==valgtnr+startofset) {
      }
      strcpy(temptxt,title);
      glTranslatef(210.0f, 820.0f,0);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.0f, 20.0f, 1.0f);
      glcRenderString("temptxt");
      glPopMatrix();
      i++;
    }
    i=0;
    while ((i<recordoversigt.programs[valgtnr+startofset].prg_antal) && (i<11)) { 				// vis max
      glPushMatrix();
      recordoversigt.programs[valgtnr+startofset].recorded_programs[i+substartofset].get_recorded(i+substartofset,title,subtitle,startdato,slutdato,desc);
      strcpy(temptxt,title);
      glTranslatef(620.0f, 820.0f,0);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.0f, 20.0f, 1.0f);
      strcpy(temptxt,subtitle);					// get sub title
      if (strcmp(temptxt,"")==0) strcpy(temptxt,desc);                // Hvis der ikke er nogle subtitle bruge description i stedet
      glcRenderString("temptxt");
      glPopMatrix();
      i++;
    }
    recordoversigt.programs[valgtnr+startofset].recorded_programs[subvalgtnr+substartofset].get_recorded(subvalgtnr,title,subtitle,startdato,slutdato,desc);
    glPushMatrix();
    glTranslatef(220.0f, 310.0f,0);
    glRasterPos2f(0.0f, 0.0f);
    glScalef(20.0f, 20.0f, 1.0f);
    glcRenderString("Date...:");
    strcpy(temptxt,startdato);
    temptxt[10]=0;
    glcRenderString(temptxt);
    glcRenderString(" KL.:");
    strcpy(temptxt,startdato);
    temptxt[5]=0;
    glcRenderString(temptxt);
    glPopMatrix();
    static time_t tm;				// this time (now)
    struct tm *nutid;
    struct tm prgstarttid;
    struct tm prgsluttid;
    char dagsdag[40];
    tm=time(0);				// get time
    nutid=localtime(&tm);
    strftime(dagsdag, 40, "%Y-%m-%d %H:%M:%S", nutid );
    strptime(startdato,"%Y-%m-%d %H:%M:%S",&prgstarttid);
    strptime(slutdato,"%Y-%m-%d %H:%M:%S",&prgsluttid);
    prgstarttid.tm_isdst=1;
    prgsluttid.tm_isdst=1;
    time_t st=mktime(&prgstarttid);
    time_t et=mktime(&prgsluttid);
    if ((tm>st) && (tm<et)) {				// er tiden inden for nu så hvis at vi optager live nu
      glColor3f(1.0f, 0.0f, 0.0f);
      glScalef(18.0f, 18.0f, 1.0f);
      glcRenderString((char *) "Recording");
    }
}
