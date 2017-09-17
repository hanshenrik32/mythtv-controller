#include <mysql.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glut.h>    // Header File For The GLUT Library
#include <GL/gl.h>      // Header File For The OpenGL32 Library
#include <GL/glu.h>     // Header File For The GLu32 Library
#include <GL/glx.h>     // Header file fot the glx libraries.
#include <GL/glc.h>             // danish ttf support

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <math.h>
#include <ctype.h>
#include <ical.h>
#include <libxml/parser.h>


//#include "text3d.h"
#include "utility.h"
#include "myctrl_tvprg.h"
#include "myth_ttffont.h"

#define MAXSIZE 16000+1;

extern int screen_size;
extern int debugmode;
extern int fonttype;

extern int configland;

extern GLuint _tvbar1;
extern GLuint _tvbar2;
extern GLuint _tvbar3;
extern GLuint _textureIdclose;
extern GLuint tvprginfobig;
extern GLuint _tvprgrecorded;
extern GLuint _tvprgrecordedr;
extern GLuint _tvprgrecorded1;
extern GLuint _tvprgrecorded_mask;
extern GLuint _tvrecordbutton;
GLuint _textureId13;
extern GLuint _tvrecordcancelbutton;
extern GLuint _tvoldrecorded;
//extern GLuint _tvoldrecordedmask;

extern GLuint _tvoldprgrecordedbutton;
extern GLuint _tvnewprgrecordedbutton;
extern GLuint _tvmaskprgrecordedbutton;
extern GLuint _tvoverskrift;
extern GLuint _tv_prgtype;
extern fontctrl aktivfont;
extern int orgwinsizex,orgwinsizey;

extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];
extern char configmusicmypath[];

extern bool loading_tv_guide;

//extern earlyrecorded oldrecorded;
//extern earlyrecorded newtcrecordlist;


const float prgtypeRGB[]={0.4f,0.4f,0.4f ,0.5f,0.9f,0.0f,                 /* Action, Series     */
                              0.0f,0.5f,0.0f ,0.5f,0.1f,0.1f,                   /* news,   Kids         */
                              1.0f,0.498039f,0.0f ,0.1f,0.2f,0.1f,              /* music,  animation    */
                               0.556863f, 0.137255f, 0.419608f ,0.2f,0.2f,0.2f, /* Horror, drama        */
                               0.91f,0.76f, 0.65f ,0.0f,0.5f,0.9f,              /* Sci fi, Cimedies     */
                              0.8f,0.8f,0.7f ,0.2f,0.3f,0.2f,                   /* Romance, Thriller    */
                              0.8f,0.3f,0.8f ,0.3f,0.2f,0.5f,                   /* Fiction, Musical     */
                              0.6f,0.6f,0.6f ,0.5f,0.5f,0.5f,                   /* wae,     Documentary */
                              0.2f,0.8f,0.3f ,0.8f,0.2f,0.6f,                   /* Series,  Kids        */
                              0.0f,0.3f,0.8f ,0.2f,0.4f,0.7f,                   /* Animation,Drama      */
                              0.0f,0.0f,0.8f ,0.0f,0.8f,0.0f,                   /*                      */
                              0.5f,0.0f,0.5f ,0.0f,0.5f,0.4f,                   /* Adventure,Comedie    */
                              0.4f,0.7f,0.7f ,0.1f,0.1f,0.1f};                 /*none,  ukendt (last)  */




const char *prgtypee[2*10]={"Action"," Series",
                          "News","Kids",
                          " Music","Animation",
                          " Horror"," Drama",
                          " sci fi","Comedies",
                          "Romance","Thriller",
                          "Fiction","Musical",
                          "War"," Doc.",
                          "Adventure","Comedie",
                          "None","Unknown"};



// bruges af show_tvoversigt

void myglprinttv(char *string) {
    int len,i;
    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
       glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
    }
}



unsigned int ELFHash(const char *s) {
    /* ELF hash uses unsigned chars and unsigned arithmetic for portability */
    const unsigned char *name = (const unsigned char *)s;
    unsigned long h = 0, g;
    while (*name) {
        h = (h << 4) + (unsigned long)(*name++);
        if ((g = (h & 0xF0000000UL))!=0)
            h ^= (g >> 24);
        h &= ~g;
    }
    return (int)h;
}


// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str)
{
  char *end;
  // Trim leading space
  while(isspace((unsigned char)*str)) str++;
  if(*str == 0)  // All spaces?
    return str;
  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;
  // Write new null terminator
  *(end+1) = 0;
  return str;
}

//
// get channel id
// OK

unsigned long get_cannel_id(char *channelname) {
  char sql[4096];
  char *database = (char *) "mythconverg";
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  unsigned long id=0;
  // mysql stuf
  conn=mysql_init(NULL);
  // Connect to database
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
  mysql_query(conn,"set NAMES 'utf8'");
  res = mysql_store_result(conn);
  if (conn) {
    sprintf(sql,"select chanid from channel where callsign like '%s'",channelname);
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) id=atol(row[0]);
    }
    mysql_free_result(res);
    mysql_close(conn);
  }
  return(id);
}


//
// check if channelname exist in db
//

bool do_cannel_exist(char *channelname) {
  char sql[4096];
  char *database = (char *) "mythconverg";
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  unsigned long id=0;
  // mysql stuf
  conn=mysql_init(NULL);
  mysql_query(conn,"set NAMES 'utf8'");
  res = mysql_store_result(conn);
  // Connect to database
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
  if (conn) {
    sprintf(sql,"select chanid from channel where callsign like '%s'",channelname);
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) id=atol(row[0]);
    }
    mysql_free_result(res);
    mysql_close(conn);
  }
  if (id==0) return(false); else return(true);
}



//
// check if program exist in db
//

bool do_program_exist(int pchanid,char *ptitle,char *pstarttime) {
  char sql[4096];
  char *database = (char *) "mythconverg";
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  unsigned long id=0;
  // mysql stuf
  conn=mysql_init(NULL);
  mysql_query(conn,"set NAMES 'utf8'");
  res = mysql_store_result(conn);
  // Connect to database
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
  if (conn) {
    sprintf(sql,"select chanid from program where chanid=%d and starttime like '%s' limit 1",pchanid,pstarttime);
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) id=atol(row[0]);
    }
    mysql_free_result(res);
    mysql_close(conn);
  }
  if (id==0) {
    return(false);
  } else {
    return(true);
  }
}



//
// get first line from string having \n in
//

void getfirstlinefromstring(char *resl,char *line) {
  int n=0;
  while((line[n]!='\n') && (line[n]!='\0') && (n<strlen(line))) {
    resl[n]=line[n];
    n++;
  }
  resl[n]='\0';
}


//
// read tv guide from file. And update tvguide db (create if not exist)
//

int tv_oversigt::parsexmltv(const char *filename) {
  xmlChar *content;
  xmlChar *content1;
  char userhomedir[200];
  char path[1024];
  char result[1024];
  char sql[4096];
  char *s;
  int error=0;
  bool cidfundet=false;
  xmlChar *tmpdat;
  xmlDoc *document;
  xmlNode *root, *first_child, *node, *node1 ,*subnode;
  xmlChar *xmltvid;
  char starttime[64];
  bool getstart=false;
  char endtime[64];
  bool getend=false;
  char prgtitle[1024];
  char channelname[1024];
  char channelidname[1024];
  xmlChar *title;
  bool gettchannel=false;
  xmlChar *desc;
  unsigned long channelid;
  bool fundet=false;
  char *database = (char *) "mythconverg";
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;

  loading_tv_guide=true;        // set loadtv guide flag to show in show_tv_guide then xml files is passed
  // mysql stuf
  conn=mysql_init(NULL);
  // Connect to database
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, NULL, 0, NULL, 0);
  if (!(conn)) error=1;
  mysql_query(conn,"set NAMES 'utf8'");
  res = mysql_store_result(conn);
  mysql_free_result(res);

  sprintf(sql,"CREATE DATABASE IF NOT EXISTS %s",database);
  mysql_query(conn,sql);
  res = mysql_store_result(conn);
  if (res) mysql_free_result(res);

  sprintf(sql,"use %s",database);
  mysql_query(conn,sql);
  res = mysql_store_result(conn);
  mysql_free_result(res);

  // check if db exist
  sprintf(sql,"select chanid from channel limit 1");
  mysql_query(conn,sql);
  res = mysql_store_result(conn);
  if (res) {
    while ((row = mysql_fetch_row(res)) != NULL) fundet=true;
  }
  mysql_free_result(res);
  if (!(fundet)) {
    strcpy(sql,"create table IF NOT EXISTS channel(chanid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,channum varchar(10),freqid varchar(10) ,sourceid int(10) unsigned,callsign varchar(20),name  varchar(64), icon varchar(255), finetune int(11) , videofilters varchar(255), xmltvid varchar(64), recpriority int(10), contrast int(11) DEFAULT 32768, brightness int(11) DEFAULT 32768, colour int(11) DEFAULT 32768, hue int(11) DEFAULT 32768, tvformat varchar(10), visible tinyint(1) DEFAULT 1, outputfilters  varchar(255) ,useonairguide tinyint(1)  DEFAULT 0, mplexid  smallint(6), serviceid  mediumint(8) unsigned, atsc_major_chan int(10) unsigned DEFAULT 0, atsc_minor_chan int(10) unsigned DEFAULT 0, last_record datetime, default_authority varchar(32), commmethod int(11) DEFAULT +1, iptvid smallint(6) unsigned,orderid int(12) unsigned DEFAULT 0)");
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    mysql_free_result(res);
    strcpy(sql,"create table IF NOT EXISTS program(chanid int(10) unsigned NOT NULL,starttime datetime, endtime datetime ,title varchar(128),subtitle varchar(128), description text, category varchar(64), category_type varchar(64), airdate year(4),stars float,previouslyshown tinyint(4), title_pronounce varchar(128), stereo tinyint(1), subtitled tinyint(1),hdtv tinyint(1), closecaptioned tinyint(1), partnumber int(11), parttotal int(11), seriesid  varchar(12), originalairdate date, showtype varchar(30), colorcode varchar(20), syndicatedepisodenumber varchar(20), programid varchar(64), manualid int(10) unsigned, generic tinyint(1), listingsource int(11), first tinyint(1), last tinyint(1) ,audioprop varchar(8),subtitletypes varchar(8),videoprop varchar(8))");
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    mysql_free_result(res);
    strcpy(sql,"create table IF NOT EXISTS programgenres(chanid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,starttime datetime,relevance char(1),genre varchar(30))");
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    mysql_free_result(res);
    strcpy(sql,"create table IF NOT EXISTS programrating(chanid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,starttime datetime,system varchar(8), rating varchar(16))");
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    mysql_free_result(res);

    // crete index
    // strcpy(sql,"create index chanid on program (chanid)");
    // mysql_query(conn,sql);
    // res = mysql_store_result(conn);
  }
  if (conn) {
    //  add user homedir and open file
    getuserhomedir(userhomedir);
    strcpy(path,userhomedir);
    strcat(path,"/");
    strcat(path,filename);                            // add filename to xmlfile name
    document = xmlReadFile(path, NULL, 0);            // open xml file
    // if exist do all the parse and update db
    // it use REPLACE in mysql to create/update records if changed in xmlfile
    if (document) {
      root = xmlDocGetRootElement(document);
      first_child = root->children;
      for (node = first_child; node; node = node->next) {
        if (node->type==XML_ELEMENT_NODE) {
          //fprintf(stdout, "\t Child is <%s> (%i) \n", node->name,node->type);
          if (strcmp((char *) node->name,"channel")==0) {
            content = xmlNodeGetContent(node);
            if (content) {
              strcpy(result,(char *) content);
              s=trimwhitespace(result);
              if (debugmode & 256) printf("TV chanel found : %s \n",s);
            }
            subnode=node->xmlChildrenNode;
            while(subnode) {
              xmltvid=xmlGetProp(node,( xmlChar *) "id");
              strcpy(channelidname,(char *) xmltvid);
              subnode=subnode->next;
            }
            cidfundet=do_cannel_exist(channelidname);
            if (cidfundet==0) {
              sprintf(sql,"insert into channel (chanid,callsign,name,xmltvid) values(0,'%s','%s','%s')",channelidname,s,xmltvid);
              mysql_query(conn,sql);
              res = mysql_store_result(conn);
            }
          }

          // create tv programs in guide from xmltag programme
          if (strcmp((char *) node->name,"programme")==0) {
            content = xmlNodeGetContent(node);
            if (content) {
              strcpy(result,(char *) content);
              s=trimwhitespace(result);
              getfirstlinefromstring(prgtitle,s);
              //printf("\t Program : %s \n",s);
            }
            subnode=node->xmlChildrenNode;
            while(subnode) {
              getstart=false;
              getend=false;
              gettchannel=false;
              node1=node;

              tmpdat=xmlGetProp(node1,( xmlChar *) "start");
              //if (tmpdat) printf("tmpdat : %s \n", tmpdat);
              if (tmpdat) {
                getstart=true;
                strcpy(starttime,"");
                strncpy(starttime,( char *) tmpdat,4);
                starttime[4]='\0';
                strcat(starttime,"-");
                strncat(starttime,( char *) tmpdat+4,2);
                strcat(starttime,"-");
                strncat(starttime,( char *) tmpdat+6,2);
                strcat(starttime," ");
                strncat(starttime,( char *) tmpdat+8,2);
                strcat(starttime,":");
                strncat(starttime,( char *) tmpdat+10,2);
                strcat(starttime,":");
                strncat(starttime,( char *) tmpdat+12,2);
                //starttime[16]=0;
                if (debugmode & 256) printf("From: %20s", starttime);
              }
              //xmlFree(tmpdat);


              tmpdat=xmlGetProp(node,( xmlChar *) "category");
              if (tmpdat) {
                if (tmpdat) printf("category: %s\n", tmpdat);
              }

              tmpdat=xmlGetProp(node1,( xmlChar *) "stop");
              if (tmpdat) {
                getend=true;
                strcpy(endtime,"");
                strncpy(endtime,( char *) tmpdat,4);
                endtime[4]='\0';
                strcat(endtime,"-");
                strncat(endtime,( char *) tmpdat+4,2);
                strcat(endtime,"-");
                strncat(endtime,( char *) tmpdat+6,2);
                strcat(endtime," ");
                strncat(endtime,( char *) tmpdat+8,2);
                strcat(endtime,":");
                strncat(endtime,( char *) tmpdat+10,2);
                strcat(endtime,":");
                strncat(endtime,( char *) tmpdat+12,2);
                //endtime[16]=0;
                if (debugmode & 256) printf(" ->%20s", endtime);
              }
              //xmlFree(tmpdat);

              xmltvid=xmlGetProp(node1,( xmlChar *) "channel");
              if ((xmltvid) && (gettchannel==false)) {
                gettchannel=true;
                strcpy(channelname,(char *) xmltvid);
                if (debugmode & 256) printf(" %s",channelname);
              }
              // save channelname to show in update
              strcpy(this->loadinginfotxt,channelname);

              //xmlFree(tmpdat);
              if (debugmode & 256) printf("\n");
              subnode=subnode->next;
            }
            //title=xmlGetProp(node1,( xmlChar *) "title");
            //if (title) printf("title : %s\n", title);

            //desc=xmlGetProp(node1,( xmlChar *) "desc");
            //if (desc) printf("desc: %s\n", desc);

//            tmpdat=xmlGetProp(subnode,( xmlChar *) "category");
//            if (tmpdat) printf("category: %s\n", tmpdat);

            // get changel id to db
            channelid=get_cannel_id(channelname);
            if (!(do_program_exist(channelid,prgtitle,starttime))) {
              // create/update record in program guide table
              sprintf(sql,"REPLACE into program (chanid,starttime,endtime,title,subtitle,description,category) values(%d,'%s','%s','%s','%s','%s','%s')",channelid,starttime,endtime,prgtitle,"","","");
              mysql_query(conn,sql);
              res = mysql_store_result(conn);
              mysql_free_result(res);
              if (debugmode & 256) fprintf(stdout,"Tvguide Program created %s->%s %s \n",starttime,endtime,prgtitle);
            } else {
              if (debugmode & 256) fprintf(stdout,"Tvguide Program exist,no update %s->%s %s \n",starttime,endtime,prgtitle);
            }
          }
        }
      }
      fprintf(stdout, "...\n");
      xmlFreeDoc(document);
    } else {
      printf("tvguide.xml not found \n");
    }
  }
  loading_tv_guide=false;
  mysql_close(conn);
  return(!(error));
}




//
// constructor
//
tv_oversigt_prgtype::tv_oversigt_prgtype() {
    strcpy(program_navn,"");
    strcpy(starttime,"");
    strcpy(endtime,"");
    program_length_minuter=0;
    starttime_unix=0;
    prg_type=0;
    aktiv=false;
    brugt=true;
    recorded=0;
    updated=false;
}


//
// destructor
//

tv_oversigt_prgtype::~tv_oversigt_prgtype() {

}


int tv_oversigt_prgtype::putprograminfo(char *prgname,char *stime,char *etime,char *prglength,char *sunixtime,char *eunixtime,char *desc,char *subtitle,int ptype,int prgrecorded) {
    strcpy(program_navn,prgname);
    strcpy(starttime,stime);
    strcpy(endtime,etime);
    strcpy(description,desc);
    strcpy(sub_title,subtitle);
    starttime_unix=atol(sunixtime);
    endtime_unix=atol(eunixtime);
    program_length_minuter=atoi(prglength);
    prg_type=ptype;
    brugt=true;
    updated=false;
    recorded=prgrecorded;
    return(1);
}


void tv_oversigt_prgtype::getprograminfo(char *prgname,char *stime,char *etime,int *prglength,unsigned long *sunixtime,unsigned long *eunixtime,char *desc,char *subtitle,int *ptype,bool *bgt,int *prgrecorded) {
    strcpy(prgname,program_navn);
    strcpy(stime,starttime);
    strcpy(etime,endtime);
    strcpy(subtitle,sub_title);
    *sunixtime=starttime_unix;
    *eunixtime=endtime_unix;
    *prglength=program_length_minuter;
    *ptype=prg_type;
    *bgt=brugt;
    *prgrecorded=recorded;
    strcpy(desc,description);
}


void tv_oversigt_prgtype::getprogramrecinfo(char *prgname,char *stime,char *etime) {
    strcpy(prgname,program_navn);
    strcpy(stime,starttime);
    strcpy(etime,endtime);
}



// constructor

tv_oversigt_pr_kanal::tv_oversigt_pr_kanal() {
    programantal=0;
    chanid=0;
    strcpy(chanel_name,"");
}


// destructor

tv_oversigt_pr_kanal::~tv_oversigt_pr_kanal() {
}




void tv_oversigt_pr_kanal::putkanalname(char *kname) {
    strcpy(chanel_name,kname);
}


void tv_oversigt_pr_kanal::cleanprogram_kanal() {
  for(int ii=0;ii<maxprogram_antal-1;ii++) {
      strcpy(tv_prog_guide[ii].program_navn,"");
      strcpy(tv_prog_guide[ii].starttime,"");
      strcpy(tv_prog_guide[ii].endtime,"");
      strcpy(tv_prog_guide[ii].sub_title,"");
      strcpy(tv_prog_guide[ii].description,"");
      tv_prog_guide[ii].program_length_minuter=0;
      tv_prog_guide[ii].starttime_unix=0;
      tv_prog_guide[ii].prg_type=0;
      tv_prog_guide[ii].aktiv=false;
      tv_prog_guide[ii].brugt=true;
      tv_prog_guide[ii].recorded=0;
  }
  strcpy(chanel_name,"");
}

// constructor

tv_oversigt::tv_oversigt() {
    kanal_antal=0;
    strcpy(mysqllhost,"");
    strcpy(mysqlluser,"");
    strcpy(mysqllpass,"");
    strcpy(loadinginfotxt,"");
}


// destructor

tv_oversigt::~tv_oversigt() {
}




//
// Clean tvprogram oversigt
//

int tv_oversigt::cleanchannels() {
  /*
    for(int i=0;i<MAXKANAL_ANTAL-1;i++) {
        for(int ii=0;ii<maxprogram_antal-1;ii++) {
            strcpy(tvkanaler[i].tv_prog_guide[ii].program_navn,"");
            strcpy(tvkanaler[i].tv_prog_guide[ii].starttime,"");
            strcpy(tvkanaler[i].tv_prog_guide[ii].endtime,"");
            strcpy(tvkanaler[i].tv_prog_guide[ii].sub_title,"");
            strcpy(tvkanaler[i].tv_prog_guide[ii].description,"");
            tvkanaler[i].tv_prog_guide[ii].program_length_minuter=0;
            tvkanaler[i].tv_prog_guide[ii].starttime_unix=0;
            tvkanaler[i].tv_prog_guide[ii].prg_type=0;
            tvkanaler[i].tv_prog_guide[ii].aktiv=false;
            tvkanaler[i].tv_prog_guide[ii].brugt=true;
            tvkanaler[i].tv_prog_guide[ii].recorded=0;
        }
        strcpy(tvkanaler[i].chanel_name,"");
    }
    */
    return(1);
}





//
// Checker om et program er optaget tidligere og retunere antal
//

int tv_oversigt::tvprgrecordedbefore(char *ftitle,unsigned int fchannelid) {
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    // mysql stuf
    static int recantal=0;
    char *database = (char *) "mythconverg";
    bool fundet=false;
    char sqlselect[200];
    sprintf(sqlselect,"SELECT count(title) FROM recorded where chanid=%d and title=\"%s\"",fchannelid,ftitle);
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, mysqllhost,mysqlluser, mysqllpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (fundet==false)) {
            fundet=true;
            recantal=atoi(row[0]);
        }
    }
    mysql_close(conn);
    if (fundet) return(recantal); else return(0);
}



//
// Checker om et program er optaget retunere type
//

int tv_oversigt::tvprgrecorded(char *fstarttime,char *ftitle,char *fchannelid) {
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    // mysql stuf
    static int rectype=0;
    char *database = (char *) "mythconverg";
    bool fundet=false;
    char sqlselect[400];
    sprintf(sqlselect,"SELECT type FROM record where (chanid=%s and title=\"%s\" and starttime='%s') or (chanid=%s and title=\"%s\" and type=10)",fchannelid,ftitle,fstarttime,fchannelid,ftitle);
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, mysqllhost,mysqlluser, mysqllpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (fundet==false)) {
            fundet=true;
            rectype=atoi(row[0]);
        }
    }
    mysql_close(conn);
    if (fundet) return(rectype); else return(0);
}


//
// Fjerner et program som skal optages
//

int tv_oversigt::removetvprgrecorded(char *fstarttime,char *ftitle,char *fchannelid) {
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    // mysql stuf
    static int rectype=0;
    char *database = (char *) "mythconverg";
    bool fundet=false;
    char sqlselect[200];
    sprintf(sqlselect,"DELETE FROM record where chanid=%s and title=\"%s\" and starttime='%s'",fchannelid,ftitle,fstarttime);
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, mysqllhost,mysqlluser, mysqllpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (fundet==false)) {
            fundet=true;
        }
    }
    mysql_close(conn);
    if (fundet) return(rectype); else return(0);
}


//
// Indsæt into table record to sectule new tv recording
//

int tv_oversigt::tvprgrecord_addrec(int tvvalgtrecordnr,int tvsubvalgtrecordnr) {
    // mysql vars
    MYSQL *conn;
    MYSQL *conn1;
    MYSQL_RES *res;
    MYSQL_RES *res1;
    MYSQL_ROW row;
    char *sqlselect;
    sqlselect=new char[40000];
    // mysql stuf
//    static int rectype=0;
    char *database = (char *) "mythconverg";
    bool doneok=false;

    time_t aktueltid;
    time_t prgtid;
    time(&aktueltid);					// hent hvad klokken er
    struct tm *timeinfo;
    struct tm prgtidinfo;
    timeinfo=localtime(&aktueltid);				// convert to localtime

    if (strptime(tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime,"%Y-%m-%d %H:%M:%S",&prgtidinfo)==NULL) {
        printf("DO INSERT RECORDED PROGRAM DATE FORMAT ERROR can't convert. by strptime\n");
    }
    // lav tv proram starttid om til time_t format
    prgtid=mktime(&prgtidinfo);

    if ((difftime(aktueltid,prgtid)<=0) && (sqlselect)) {
        sprintf(sqlselect,"SELECT channel.name as channelname, TIME(starttime) as starttime,DATE(starttime) as startdate,TIME(endtime) as endtime,DATE(endtime) as enddate,NOW() as datenu, program.chanid, program.category from program left join channel on program.chanid=channel.chanid where program.title='%s' and program.starttime='%s' and program.endtime='%s'",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn,tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime,tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].endtime);
        conn=mysql_init(NULL);
        conn1=mysql_init(NULL);
        // Connect to databases
        mysql_real_connect(conn, mysqllhost,mysqlluser, mysqllpass, database, 0, NULL, 0);
        mysql_real_connect(conn1, mysqllhost,mysqlluser, mysqllpass, database, 0, NULL, 0);
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);

        mysql_query(conn1,"set NAMES 'utf8'");
        res1 = mysql_store_result(conn1);

        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        if (res) {
            while (((row = mysql_fetch_row(res)) != NULL) && (doneok==false)) {
                //tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn
                sprintf(sqlselect,"INSERT INTO record values (0,1,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",'Default',0,0,1,0,0,0,'Default',6,15,\"%s\",%u,'',0,0,0,0,0,0,0,0,TIME('%s'),%lu,0,0,0,'Default',0,'0000-00-00 00:00:00','','0000-00-00 00:00:00','Default',100)", row[6], row[1], row[2], row[3],row[4], tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn, tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].sub_title, tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].description, row[7],row[0], ELFHash(tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn) , row[1], ((tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime_unix)/60/60/24)+719528);
                mysql_query(conn1,sqlselect);
                res1 = mysql_store_result(conn1);
                doneok=true;
            }
        }
        mysql_close(conn);
        mysql_close(conn1);
        delete sqlselect;
    }
    return(doneok);
}



//
// Find tv guide kanal med samme eller senere tidspunkt som tidspunkt og retunre hviken element i array som har den start tid
// hvis ingen findes gå til start dvs array element 0
// bruges ved pil up/down i tv kanal listen i vis_tv_oversigt
//

int tv_oversigt::findguidetvtidspunkt(int kanalnr,time_t tidspunkt) {
    int prgnr=0;
    bool fundet=false;
    while((prgnr<tvkanaler[kanalnr].program_antal()) && (!(fundet))) {
        if ((time_t) tvkanaler[kanalnr].tv_prog_guide[prgnr].starttime_unix<tidspunkt) prgnr++; else fundet=true;
    }
    if (fundet) return(prgnr); else return(0);
}



// retunere tvprg starttid in unix start time

time_t tv_oversigt::hentprgstartklint(int kanalnr,int prgnr) {
    if ((kanalnr>=0) && (kanalnr<kanal_antal)) return(tvkanaler[kanalnr].tv_prog_guide[prgnr].starttime_unix); else return(0);
}



//
// henter aktiv tv overigt fra mythtv or internal localdb created like mythtv in use
//

void tv_oversigt::opdatere_tv_oversigt(char *mysqlhost,char *mysqluser,char *mysqlpass,time_t nystarttid) {
    int i,ii;
    int prgtype;
    bool recorded;
    char sqlselect[512];
    char dagsdato[128];
    char enddate[128];
    time_t rawtime;
    time_t rawtime2;
    struct tm *timeinfo;
    struct tm *timeinfo2;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    // mysql stuf
    char *database = (char *) "mythconverg";
    char tmptxt[100];

    strcpy(this->mysqllhost,mysqlhost);
    strcpy(this->mysqlluser,mysqluser);
    strcpy(this->mysqllpass,mysqlpass);

    loading_tv_guide=true;

    if (nystarttid==0) {
        // get time now in a string format (yyyy-mm-dd hh:mm:ss)
        rawtime=time( NULL );				     			 // hent nu tid
        rawtime2=time( NULL );					   	   // hent nu tid
        rawtime2+=60*60*24;                   //  + 1 døgn
    } else {
        // hent ny starttid
        rawtime=this->starttid;                // this
        rawtime2=this->sluttid;                // this
    }

    timeinfo = localtime ( &rawtime );			            		// lav om til local time
    strftime(dagsdato, 128, "%Y-%m-%d 00:00:00", timeinfo );		// lav nu tids sting strftime(dagsdato, 128, "%Y-%m-%d %H:%M:%S", timeinfo );
    timeinfo2= localtime ( &rawtime2 );	            				//
    strftime(enddate, 128, "%Y-%m-%d 23:59:59", timeinfo2 );		// lav nu tids sting
    this->starttid=rawtime;						// gem tider i class
    this->sluttid=rawtime2;						//
    printf("\nGet/update Tvguide.\n");
    printf("Tvguide from %-20s to %-20s \n",dagsdato,enddate);
    // clear last tv guide array
    cleanchannels();
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, mysqlhost,mysqluser, mysqlpass, database, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);

        // do select from db count nr of records
        strcpy(sqlselect,"SELECT count(channel.name) FROM program left join channel on program.chanid=channel.chanid where channel.visible=1 and endtime<='");
        strcat(sqlselect,enddate);
        strcat(sqlselect,"' and endtime>='");
        strcat(sqlselect,dagsdato);
        strcat(sqlselect,"' order by chanid,orderid,abs(channel.channum),starttime");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        if (res) {
          while ((row = mysql_fetch_row(res)) != NULL) {
              printf("Antal channels/tvguide %s \n",row[0]);
          }
        }

        // do select from db
        strcpy(sqlselect,"SELECT channel.name,program.starttime,program.endtime,title,subtitle,TIMESTAMPDIFF(MINUTE,starttime,endtime),UNIX_TIMESTAMP(program.starttime),UNIX_TIMESTAMP(program.endtime),category,category_type,description,program.chanid FROM program left join channel on program.chanid=channel.chanid where channel.visible=1 and endtime<='");
        strcat(sqlselect,enddate);
        strcat(sqlselect,"' and endtime>='");
        strcat(sqlselect,dagsdato);
        strcat(sqlselect,"' order by chanid,orderid,abs(channel.channum),starttime");

        if (debugmode & 256) printf("Tv guide sql %s \n",sqlselect);

        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        i=0;
        ii=0;
        if (res) {
            while (((row = mysql_fetch_row(res)) != NULL) && (ii<=maxprogram_antal) && (i<MAXKANAL_ANTAL)) {
                if (ii==0) {
                    tvkanaler[i].putkanalname(row[0]);
                    tvkanaler[i].chanid=atoi(row[11]);
                    strcpy(tmptxt,row[0]);
                }

                if (row[7]) {
                  if (strcmp("series",row[8])==0) prgtype=1;					           	// serie
                  else if (strcmp("dansk underholdning.",row[8])==0) prgtype=1;   //
                  else if (strcmp("sport",row[8])==0) prgtype=2;                  //
                  else if (strncmp("børn",row[8],4)==0) prgtype=3;                //
                  else if (strncmp("dukkefilm",row[8],9)==0) prgtype=3;           //
                  else if (strstr(row[8],"tegnefilm")!=0) prgtype=3;              //
                  else if (strstr(row[8],"animationsfilm")!=0) prgtype=3;         //
                  else if (strcmp("news",row[8])==0) prgtype=4;                   //
                  else if (strcmp("movie",row[8])==0) prgtype=5;                  //
                  else if (strstr(row[8],"dokumentarserie")!=0) prgtype=6;        //
                  else if (strcmp("engelsk madprogram",row[8])==0) prgtype=11;    //
                  else if (strcmp("dansk reportageserie.",row[8])==0) prgtype=7;  //
                  else if (strcmp("amerikansk krimi.",row[8])==0) prgtype=5;      //
                  else if (strcmp("debatprogram.",row[8])==0) prgtype=7;          //
                  else if (strcmp("music",row[8])==0) prgtype=8;                  //
                  else if (strcmp("dyr",row[8])==0) prgtype=9;                    //
                  else prgtype=0;                                                 // default panic
                } else prgtype=0;                                                 // default panic
                if (prgtype==0) {
                    if (row[8]) {
                      if (strcmp("series",row[10])==0) prgtype=1;
                      else if (strcmp("movie",row[10])==0) prgtype=5;
                      else prgtype=0;
                    }
                }
                recorded=tvprgrecorded(row[1],row[3],row[11]);			// get recorded status from backend
                tvkanaler[i].tv_prog_guide[ii].putprograminfo(row[3],row[1],row[2],row[5],row[6],row[7],row[10],row[4],prgtype,recorded);
                ii++;
                if ((strcmp(tmptxt,row[0])!=0) || (ii>=maxprogram_antal)) {
                    tvkanaler[i].set_program_antal(ii-1);
                    ii=0;
                    i++;								// next tv channel
                }
            }
            this->kanal_antal=i;
            printf("Found nr of tv channels %d \n",this->kanal_antal);
        }
        mysql_close(conn);
    }
    loading_tv_guide=false;
}







void tv_oversigt::show_canal_names() {
    int i;
    int kanalantal;
    float yofset=4.2f;
//    float zofset=-110.0f;
    // make boxes bihint canal name
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, _tvbar2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    kanalantal=16;
    i=0;
    while (i<kanalantal) {
        glLoadIdentity();
        glTranslatef(-98, 27.8-(i*(yofset)), -99.5f-40); 	//
        glRotatef(45.0f,0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(-10, -0.6, -10.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(7.0, -0.6 , -10.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(7.0, -6.4,-10.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(-10, -6.4,-10.0);
        glEnd();
        i++;
    }
}



// Outputs a string wrapped to N columns
void WordWrap( char *str, int N ) {
    int i,j,k,pos;
    i = pos = 0;
    char word[16000];

    while( 1 ) {
        j = 0;
        while( !isspace( str[i]) ) // Find next word
        {
            if( str[i] == '\0' )
                break;
            word[j] = str[i];
            i++;
            j++;
        }

        word[j] = '\0'; // The value of j is the word length

        if( j > N ) // Word length is greater than column length
        { // print char-by-char
            k = 0;
            while( word[k] != '\0' ) {
                if( pos >= N-1 ) {
                    if( k != 0 ) putchar('-'); // print '-' if last-but-one column
                    pos = 0;
                    putchar('\n');
                }
                putchar( word[k] );
                pos++;
                k++;
            }
        } else {
            if( pos + j > N ) // Word doesn't fit in line
            {
                putchar('\n');
                pos = 0;
            }

            printf("%s", word );
            pos += j;
        }
        if( pos < N-1 ) // print space if not last column
        {
            putchar(' ');
            pos++;
        }
        if( str[i] == '\0' ) break;

        i++; // Skip space
    } // end of while(1)
}







//
// Vis tv oversigt
// den som bruges
//

void tv_oversigt::show_fasttv_oversigt(int selectchanel,int selectprg) {

  int totaltid=0;
  int omgang=0;
  int startyofset=0;
  int textofset=0;
  int startofset=0;
  // husk tv kanalid
  // program loop


    struct tm *timeinfo;
    struct tm nowtime_h;
    struct tm endnowtime_h;

    time_t nutidtime;

    int i,j,k,pos;
    int iii;
    int kanalantal=12;
    char tmptxt[250];
    char tmptxt1[250];
    int xpos,ypos;
    int xsiz,ysiz;

    int pstartofset=0;
    int kanalnr=0;
    int ptype=1;

    int cstartofset=0;

    int xtid;
    int xtidlength;
    //size 1870*150

    xpos=10;
    ypos=orgwinsizey-200;
    xsiz=(orgwinsizex-50);
    ysiz=150;

    if (selectchanel>13) cstartofset=selectchanel-13;
    else cstartofset=0;

    glPushMatrix();
    glTranslatef(10,50, 0.0f);
    // top
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_tvoverskrift);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(27);     // 40
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();


    glPushMatrix();
    aktivfont.selectfont((char *) "Norasi");
    glTranslatef(300.0f,orgwinsizey-100.0f, 0.0f);
    glDisable(GL_TEXTURE_2D);
    glScalef(70.0, 70.0, 1.0);
    glColor3f(1.0f, 1.0f, 1.0f);
    starttid=time( NULL );
    timeinfo=localtime(&starttid);
    switch (configland) {
      case 0: sprintf(tmptxt,"TV Guiden %s %02d-%02d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
              break;
      case 1: sprintf(tmptxt,"TV Guiden %s den %02d-%02d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
              break;
      case 2: sprintf(tmptxt,"TV Guide %s %02d-%02d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
              break;
      case 3: sprintf(tmptxt,"TV Guide %02s %d-%02d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
              break;
      case 4: sprintf(tmptxt,"دليل التلفزيون %d-%d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
              break;
      default:sprintf(tmptxt,"TV Guide %s %02d-%02d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
              break;
    };
    glcRenderString(tmptxt);
    glPopMatrix();


    if (loading_tv_guide) {
      // show loading tv guide
      xsiz=450;
      ysiz=100;
      xpos=(orgwinsizex/2)-xsiz/2;
      ypos=(orgwinsizey/2)-ysiz/2;
      glPushMatrix();
      glTranslatef(10,50, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glBindTexture(GL_TEXTURE_2D,_tvbar3);
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 0.0); glVertex3f(xpos+225-(xsiz/2), ypos-(ysiz/2), 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(xpos+225-(xsiz/2), ypos+ysiz-(ysiz/2), 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(xpos+225+xsiz-(xsiz/2), ypos+ysiz-(ysiz/2), 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(xpos+225+xsiz-(xsiz/2), ypos-(ysiz/2), 0.0);
      glEnd(); //End quadrilateral coordinates
      // print
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(xpos+120,ypos, 0.0f);
      glScalef(20.0, 20.0,1);
      glDisable(GL_TEXTURE_2D);
      glcRenderString("Loading tv guide....");
      glTranslatef(-9.0f,-1.5f, 0.0f);
      if (strcmp("",this->loadinginfotxt)!=0) {
        strcpy(tmptxt,"Updating ch ");
        strcat(tmptxt,this->loadinginfotxt);
        glcRenderString(tmptxt);
      }
      glPopMatrix();
    }


    // make time string
    glPushMatrix();
    strcpy(tmptxt,"");
    int zz;
    for (int z=timeinfo->tm_hour;z<timeinfo->tm_hour+5;z++) {
      zz=z;
      if (zz>24) zz=z-24;
      sprintf(tmptxt1,"%02d:00                                               ",zz);
      strcat(tmptxt,tmptxt1);
    }
    glDisable(GL_TEXTURE_2D);
    glTranslatef(10,50, 0.0f);
    glTranslatef(200-30,850, 0.0f);
    glScalef(20.0, 20.0,1);
    glcRenderString(tmptxt);
    glPopMatrix();


    if (this->kanal_antal>0) {
      endnowtime_h.tm_min=0;
      endnowtime_h.tm_sec=0;
      endnowtime_h.tm_hour=timeinfo->tm_hour+4;
      if (endnowtime_h.tm_hour>24) {
        endnowtime_h.tm_hour=endnowtime_h.tm_hour-24;
        endnowtime_h.tm_mday=timeinfo->tm_mday+1;
        endnowtime_h.tm_mon=timeinfo->tm_mon;
        endnowtime_h.tm_mday=timeinfo->tm_mday;
        endnowtime_h.tm_year=timeinfo->tm_year;
        endnowtime_h.tm_wday=timeinfo->tm_wday;
        endnowtime_h.tm_yday=timeinfo->tm_yday;
        endnowtime_h.tm_isdst=timeinfo->tm_isdst;
      } else {
        endnowtime_h.tm_mday=timeinfo->tm_mday;
        endnowtime_h.tm_mon=timeinfo->tm_mon;
        endnowtime_h.tm_mday=timeinfo->tm_mday;
        endnowtime_h.tm_year=timeinfo->tm_year;
        endnowtime_h.tm_wday=timeinfo->tm_wday;
        endnowtime_h.tm_yday=timeinfo->tm_yday;
        endnowtime_h.tm_isdst=timeinfo->tm_isdst;
      }
      //printf("slut kl %2d:%2d unixtime %d \n",endnowtime_h.tm_hour,endnowtime_h.tm_min,mktime(&endnowtime_h));
      nutidtime=time(NULL);

      nutidtime+=60*60*24;
      iii=0;
      int chanid=0;
      // 14 channel over view
      while (iii<10) {
        xpos=10;
        ypos=orgwinsizey-300-(iii*50);
        xsiz=180;
        ysiz=50;
        glPushMatrix();
        glTranslatef(10,50, 0.0f);
        if (strlen(tvkanaler[kanalnr+cstartofset].chanel_name)>0) {
          if (selectchanel==iii+cstartofset) glColor3f(0.6f, 0.6f, 0.6f); else glColor3f(1.0f, 1.0f, 1.0f);
          glEnable(GL_TEXTURE_2D);
          glBlendFunc(GL_ONE, GL_ONE);
          glBindTexture(GL_TEXTURE_2D,_tvbar1);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glBegin(GL_QUADS); //Begin quadrilateral coordinates
          glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
          glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
          glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
          glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
          glEnd(); //End quadrilateral coordinates
        }
        glPopMatrix();
          //
        // endnowtime_h = endtime to show i while
        //
        chanid=tvkanaler[kanalnr+cstartofset].chanid;

        //printf("chanid = %d \n",chanid);

        while((tvkanaler[kanalnr+cstartofset].chanid==chanid) && (omgang<tvkanaler[kanalnr+cstartofset].program_antal()) && (tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix<nutidtime+(60*60*24))) {

          //printf("*");
          
            //printf("program start kl %s max time %s \n",ctime((time_t *) &tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix),ctime((time_t *)&endnowtime_h));
            //
          xpos=189+2;
          ypos=orgwinsizey-300-(iii*50);
          xsiz=1000;
          ysiz=50;
            // hent program læmgde
          xsiz=(int) tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].program_length_minuter*7;
          strcpy(tmptxt,tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime+14); // min start
          tmptxt[2]='\0';
          nowtime_h.tm_hour=timeinfo->tm_hour;
          nowtime_h.tm_min=0;
          nowtime_h.tm_sec=0;
          nowtime_h.tm_mon=timeinfo->tm_mon;
          nowtime_h.tm_mday=timeinfo->tm_mday;
          nowtime_h.tm_year=timeinfo->tm_year;
          nowtime_h.tm_wday=timeinfo->tm_wday;
          nowtime_h.tm_yday=timeinfo->tm_yday;
          nowtime_h.tm_isdst=timeinfo->tm_isdst;
          char ttmp[80];
          time_t start=tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix;
          time_t nutid=mktime(&nowtime_h);
          strcpy(ttmp,tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].program_navn);

          if (((tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix<=nutidtime) && (tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].endtime_unix>nutidtime)) ||
            ((tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix<=nutidtime) && (tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].endtime_unix>mktime(&nowtime_h))) ||
            //(tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix>starttid)) {
            (tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix>0)) {
            nowtime_h.tm_hour=timeinfo->tm_hour;
            nowtime_h.tm_min=0;
            nowtime_h.tm_sec=0;
            nowtime_h.tm_mon=timeinfo->tm_mon;
            nowtime_h.tm_mday=timeinfo->tm_mday;
            nowtime_h.tm_year=timeinfo->tm_year;
            nowtime_h.tm_wday=timeinfo->tm_wday;
            nowtime_h.tm_yday=timeinfo->tm_yday;
            nowtime_h.tm_isdst=timeinfo->tm_isdst;
            // nowtime_h is the clock now bud only the hour like 13:00:00
            // starttid=now() (time)
            if (tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix<starttid) {
              // beregn antal minuter fra program start til starttid på overview eks (14:00) start tid som vises i window
              xtid=difftime(mktime(&nowtime_h),tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix)/60;

              //printf("xtid ofset =%d\n",xtid);

              if (xtid<=0) {
                // find lænde siden last hele clock (timer)
                xtid=difftime(tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].endtime_unix,mktime(&nowtime_h))/60;
                xsiz=xsiz+(xtid);
                startofset=((tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix-mktime(&nowtime_h))/60)*7;
              } else {
                startofset=0;
              }
              // diff in min
              xtidlength=difftime(tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].endtime_unix,tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix)/60;
              //xsiz=xsiz-(xtid*7);
            } else {
              // length in min
              //xtid=difftime(tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].endtime_unix,tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix)/60;
              xtid=(tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].endtime_unix-tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix)/60;
              // diff in min
              xtidlength=difftime(tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].endtime_unix,tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime_unix)/60;
              startofset+=xsiz;
              xsiz=xsiz+(xtid*7);
            }
/*
              if (chanid==2) {
                printf("\nChannelid %d xtid=%d ",tvkanaler[kanalnr+pstartofset].chanid,xtid);
                printf(" Program %20s \n",tvkanaler[kanalnr+pstartofset].tv_prog_guide[omgang].program_navn);
                printf("Start time %s ", ctime ((const time_t*) &tvkanaler[kanalnr+pstartofset].tv_prog_guide[omgang].starttime_unix));
                printf("End   time %s \n", ctime ((const time_t*) &tvkanaler[kanalnr+pstartofset].tv_prog_guide[omgang].endtime_unix));
              }

*/
              //tvkanaler[kanalnr+pstartofset].tv_prog_guide[omgang].starttime_unix
            if (startofset>0) {
              // make line box around program in xsiz
              glPushMatrix();
              glTranslatef(10,50, 0.0f);
              glBegin(GL_LINE_LOOP); //Begin quadrilateral coordinates
              glTexCoord2f(0.0, 0.0); glVertex3f(xpos+startofset, ypos, 0.0);
              glTexCoord2f(0.0, 1.0); glVertex3f(xpos+startofset, ypos+ysiz, 0.0);
              glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz+startofset, ypos+ysiz, 0.0);
              glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz+startofset, ypos, 0.0);
              glEnd(); //End quadrilateral coordinates
              glPopMatrix();
              // show progrma name
              glPushMatrix();
              glDisable(GL_TEXTURE_2D);
              glTranslatef(10,50, 0.0f);
              //glTranslatef(210+startofset,ypos+12, 0.0f);
              glTranslatef(xpos+startofset+2,ypos+12, 0.0f);
              if (xtidlength>10) {
                if (debugmode & 256) sprintf(tmptxt,"startofset %d start %s %-14s %d",startofset ,tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime,tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].program_navn,xtid);
                else sprintf(tmptxt,"%-14s",tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].program_navn);
                //tmptxt[15]='\0';
              } else {
                if (debugmode & 256) sprintf(tmptxt,"startofset %d start %s %-3s",startofset ,tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].starttime,tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].program_navn);
                else sprintf(tmptxt,"%-3s..",tvkanaler[kanalnr+cstartofset].tv_prog_guide[omgang].program_navn);
                tmptxt[3]='\0';
              }
              glScalef(17.0, 17.0,1);
              glcRenderString(tmptxt);
              glScalef(20.0, 20.0,1);
              glPopMatrix();
            }
          } else {
            /*
            printf("\n\tChannelid %d xtid=%d ",tvkanaler[kanalnr+pstartofset].chanid,xtid);
            printf("\tProgram %20s \n",tvkanaler[kanalnr+pstartofset].tv_prog_guide[omgang].program_navn);
            printf("\tStart time %s ", ctime ((const time_t*) &tvkanaler[kanalnr+pstartofset].tv_prog_guide[omgang].starttime_unix));
            printf("\tEnd   time %s \n", ctime ((const time_t*) &tvkanaler[kanalnr+pstartofset].tv_prog_guide[omgang].endtime_unix));
            */
          }
          startyofset+=xsiz;
          totaltid++;
          omgang++;
          //tidsloop++;                   // plus one hour
        }
          // show kanal navn text
        glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(26,orgwinsizey-230-(iii*50), 0.0f);
        glScalef(20.0, 20.0,1);
        glDisable(GL_TEXTURE_2D);
        sprintf(tmptxt,"%-14s",tvkanaler[kanalnr+cstartofset].chanel_name);
        tmptxt[15]='\0';
        glcRenderString(tmptxt);
        glPopMatrix();
        kanalnr++;
        iii++;
      }


      if (!(loading_tv_guide)) {
        // show the clock line
        xpos=200+timeinfo->tm_min*7;
        ypos=orgwinsizey-930;
        xsiz=3;
        ysiz=688;
        glPushMatrix();
        glTranslatef(10,50, 0.0f);
        glBegin(GL_LINE_LOOP); //Begin quadrilateral coordinates
        glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
        glEnd(); //End quadrilateral coordinates
        glPopMatrix();
      }
    } else {
        //printf("Kanal antal=0\n");
    }
    //printf("Kanal antal=%d\n",kanal_antal);
/*
    // tv program info window
    xpos=1300;
    ypos=orgwinsizey-900;

    xsiz=500;
    ysiz=600;
    glPushMatrix();
    glTranslatef(10,50, 0.0f);
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,tvprginfobig);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
    glEnd(); //End quadrilateral coordinates
*/

    // show text
    // size 1000*120

    glPushMatrix();
    glTranslatef(10,50, 0.0f);
    xsiz=1000;
    ysiz=120;
    xpos=460;
    ypos=0;

    //glTranslatef(0,0, 0.0f);
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,_tvbar2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates120
    glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

    ptype=0;
    // under menu in colors
    iii=0;
    i=0;
    while(iii<10) {
      // size
      xsiz=100;
      ysiz=60;
      xpos=470+(iii*100);
      ypos=110;
      glPushMatrix();
      glTranslatef(0,0, 0.0f);
      glDisable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      switch (ptype) {
        case 0:
          glColor3f(prgtypeRGB[0], prgtypeRGB[1], prgtypeRGB[2]);		// film
          break;
        case 1:
          glColor3f(prgtypeRGB[3], prgtypeRGB[4], prgtypeRGB[5]);      	// serier
          break;
        case 2:
          glColor3f(prgtypeRGB[6], prgtypeRGB[7], prgtypeRGB[8]);      	// div
          break;
        case 3:
          glColor3f(prgtypeRGB[9], prgtypeRGB[10], prgtypeRGB[11]);      	// action
          break;
        case 4:
          glColor3f(prgtypeRGB[12], prgtypeRGB[13], prgtypeRGB[14]);      	// nyheder
          break;
        case 5:
          glColor3f(prgtypeRGB[15], prgtypeRGB[16], prgtypeRGB[17]);		// komedier
          break;
        case 6:
          glColor3f(prgtypeRGB[18], prgtypeRGB[19], prgtypeRGB[20]);      	// underholdning
          break;
        case 7:
          glColor3f(prgtypeRGB[21], prgtypeRGB[22], prgtypeRGB[23]);      	// music
          break;
        case 8:
          glColor3f(prgtypeRGB[24], prgtypeRGB[25], prgtypeRGB[26]);      	// andet
          break;
        case 9:
          glColor3f(prgtypeRGB[27], prgtypeRGB[28], prgtypeRGB[29]);		// sifi
          break;
        case 10:
          glColor3f(prgtypeRGB[30], prgtypeRGB[31], prgtypeRGB[32]);		// ukdentd
          break;
        case 11:
          glColor3f(prgtypeRGB[33], prgtypeRGB[34], prgtypeRGB[35]);		// rejser
          break;
        case 12:
          glColor3f(prgtypeRGB[36], prgtypeRGB[37], prgtypeRGB[38]);		//
          break;
        case 13:
          glColor3f(prgtypeRGB[39], prgtypeRGB[40], prgtypeRGB[41]);		// ukendt
          break;
        case 14:
          glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		// rejser
          break;
        default:
          glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		// rejser
          break;
      }

      glBindTexture(GL_TEXTURE_2D,_tv_prgtype);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS); //Begin quadrilateral coordinates120
      glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
      glEnd(); //End quadrilateral coordinates
      glPopMatrix();

      // text
      glPushMatrix();
      glColor4f(1.0f,1.0f,1.0f,1.0f);
      glTranslatef(480+(iii*100),130,0.0f);
      glScalef(20.0, 20.0,1);
      glDisable(GL_TEXTURE_2D);
      sprintf(tmptxt,"%s",prgtypee[i]);
      glcRenderString(tmptxt);
      glPopMatrix();

      ptype++;

      xsiz=100;
      ysiz=60;
      xpos=470+(iii*100);
      ypos=50;
      glPushMatrix();
      glTranslatef(0,0, 0.0f);
//      glEnable(GL_TEXTURE_2D);
      glDisable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      switch (ptype) {
          case 0:
              glColor3f(prgtypeRGB[45], prgtypeRGB[46], prgtypeRGB[47]);		// film
              break;
          case 1:
              glColor3f(prgtypeRGB[48], prgtypeRGB[49], prgtypeRGB[50]);      	// serier
              break;
          case 2:
              glColor3f(prgtypeRGB[51], prgtypeRGB[52], prgtypeRGB[53]);      	// div
              break;
          case 3:
              glColor3f(prgtypeRGB[54], prgtypeRGB[55], prgtypeRGB[56]);      	// action
              break;
          case 4:
              glColor3f(prgtypeRGB[57], prgtypeRGB[58], prgtypeRGB[59]);      	// nyheder
              break;
          case 5:
              glColor3f(prgtypeRGB[60], prgtypeRGB[61], prgtypeRGB[62]);		// komedier
              break;
          case 6:
              glColor3f(prgtypeRGB[63], prgtypeRGB[64], prgtypeRGB[65]);      	// underholdning
              break;
          case 7:
              glColor3f(prgtypeRGB[66], prgtypeRGB[67], prgtypeRGB[68]);      	// music
              break;
          case 8:
              glColor3f(prgtypeRGB[69], prgtypeRGB[70], prgtypeRGB[71]);      	// andet
              break;
          case 9:
              glColor3f(prgtypeRGB[72], prgtypeRGB[73], prgtypeRGB[74]);		// sifi
              break;
          case 10:
              glColor3f(prgtypeRGB[75], prgtypeRGB[76], prgtypeRGB[77]);		// ukdentd
              break;
          case 11:
              glColor3f(prgtypeRGB[78], prgtypeRGB[79], prgtypeRGB[80]);		// rejser
              break;
          case 12:
              glColor3f(prgtypeRGB[81], prgtypeRGB[82], prgtypeRGB[83]);		//
              break;
          case 13:
              glColor3f(prgtypeRGB[84], prgtypeRGB[85], prgtypeRGB[86]);		// ukendt
              break;
          case 14:
              glColor3f(prgtypeRGB[87], prgtypeRGB[88], prgtypeRGB[89]);		// rejser
              break;
          default:
              glColor3f(prgtypeRGB[90], prgtypeRGB[91], prgtypeRGB[92]);		// rejser
              break;
      }
      glBindTexture(GL_TEXTURE_2D,_tv_prgtype);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS); //Begin quadrilateral coordinates120
      glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
      glEnd(); //End quadrilateral coordinates
      glPopMatrix();


      // text
      glPushMatrix();
      glColor4f(1.0f,1.0f,1.0f,1.0f);
      glTranslatef(480+(iii*100),70,0.0f);
      glScalef(20.0, 20.0,1);
      glDisable(GL_TEXTURE_2D);
      sprintf(tmptxt,"%s",prgtypee[i+1]);
      glcRenderString(tmptxt);
      glPopMatrix();


      i+=2;
      iii+=1;
    }
}








// viser teksten i fast tvguide

void tv_oversigt::show_fasttv_oversigt_old(int selectchanel,int selectprg)
{
    char tmptxt[250];
    char tmptxt1[200];
    char tmptxt2[200];
    int kanalnr=0;
//    int tidspunkt=0;
    int kanalantal;
    int prglength;
    int pstartofset;
    int upstartofset;
    struct tm * timeinfo;
    int i,j,k,pos;
    int iii;
    const int MAXLINES=40;
//    const int MAXLINESLENGTH=80;
    i = pos = 0;
    int N=36;
    char word[1600];
    char str[16000];
    char desclines[40][80];
    for(int ii=0;ii<40;ii++) desclines[ii][0]='\0';
    if (selectchanel>11) pstartofset=selectchanel-11;
    else pstartofset=0;

    if (selectprg>11) upstartofset=selectprg-11;
    else upstartofset=0;

//printf("startofset %d \n",starttidofset);
    glColor3f(1.0f, 1.0f, 1.0f);      				// color
    switch(screen_size) {
        case 1: kanalantal=12;
                break;
        case 2: kanalantal=12;
                break;
        case 3: kanalantal=12;
                break;
        case 4: kanalantal=12;
                break;
        default:kanalantal=12;
    }

    //aktivfont.selectfont("Umpush");
    aktivfont.selectfont((char *) "Norasi");


    // show time/date in the guide
    glLoadIdentity();

    glTranslatef(300.0f,orgwinsizey-100.0f, 0.0f);
    glDisable(GL_TEXTURE_2D);
    glScalef(70.0, 70.0, 1.0);
    glColor3f(1.0f, 1.0f, 1.0f);


    //glTranslatef(-34.0f, (23.6)+2.6, -80.0f);
    //glScalef(3.6, 3.6, 3.6);
    starttid=time( NULL );
    timeinfo=localtime(&starttid);
    sprintf(tmptxt,"TV Guiden %s den %d-%d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
    glcRenderString(tmptxt);


    glColor3f(1.0f,1.0f,1.0f);
    glDisable(GL_TEXTURE_2D);
    while (kanalnr<12) {				// kanalantal
        glLoadIdentity();
        switch (screen_size) {
            case 1: glTranslatef(-62.8+3, (23.6)-(kanalnr*(4.6)), -110.0f);	// 47.8 = center/55.8 left align
                    break;
            case 2: glTranslatef(-62.8+3, (23.6)-(kanalnr*(4.6)), -110.0f);
                    break;
            case 3: glTranslatef(-88.8+3, (23.6)-(kanalnr*(4.6)), -110.0f);
                    break;
            case 4: glTranslatef(-85.8+3, (23.6)-(kanalnr*(4.6)), -110.0f);
                    break;
        }
        if (kanalnr==selectchanel-pstartofset)
            glColor3f(1.0f,1.0f,1.0f);
        else
            glColor3f(0.5f,0.5f,0.5f);
        glScalef(1.6, 1.6,1.6);
        sprintf(tmptxt,"%16s",tvkanaler[kanalnr+pstartofset].chanel_name);
        glcRenderString(tmptxt);
        kanalnr++;
    }

    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(-48.8+3, (27.6), -110.0f);	// 47.8 = center/55.8 left align
                break;
        case 2: glTranslatef(-58.8+3, (27.6), -110.0f);
                break;
        case 3: glTranslatef(-72.8+3, (27.6), -110.0f);
                break;
        case 4: glTranslatef(-70.8+3, (27.6), -110.0f);
                break;
    }

    i=0;
    while(i<12) {
        prglength=tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].program_length_minuter;
        glLoadIdentity();
        switch (screen_size) {
            case 1: glTranslatef(-46.8+2, (23.6)-(i*(4.6)), -110.0f);	// 47.8 = center/55.8 left align
                      break;
            case 2: glTranslatef(-46.8+2, (23.6)-(i*(4.6)), -110.0f);
                    break;
            case 3: glTranslatef(-72.8+4, (23.6)-(i*(4.6)), -110.0f);
                    break;
            case 4: glTranslatef(-68.8+2, (23.6)-(i*(4.6)), -110.0f);
                    break;
        }
        if (i==selectprg-upstartofset) glColor3f(1,1,1);
        else glColor3f(0.5f,0.5f,0.5f);
        glScalef(1.8, 1.8,1.8);
        if (strlen(tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].program_navn)>0) {
            sprintf(tmptxt1,"%s",tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].starttime+11);
            tmptxt1[5]='\0';
            sprintf(tmptxt2,"%s",tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].endtime+11);
            tmptxt2[5]='\0';
            sprintf(tmptxt,"%s-%s %s ",tmptxt1,tmptxt2,tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].program_navn);
            if (strlen(tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].sub_title)>0) {
//                strcat(tmptxt,"- ");
                strcat(tmptxt,tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].sub_title);
            }
            glcRenderString(tmptxt);
        } else {
            glcRenderString("Ingen program information.");
        }
        i++;
    }



    strcpy(str,tvkanaler[selectchanel].tv_prog_guide[selectprg].description);
    iii=0;			// aktiv linie i array
    i=0;
    while( 1 ) {
        j = 0;
        while( !isspace( str[i]) ) // Find next word
        {
            if( str[i] == '\0' )
                break;
            word[j] = str[i];
            i++;
            j++;
        }

        word[j] = '\0'; // The value of j is the word length

        if( j > N ) // Word length is greater than column length
        { // print char-by-char
            k = 0;
            while( word[k] != '\0' ) {
                if( pos >= N-1 ) {
                    if( k != 0 ) {
                        desclines[iii][strlen(desclines[iii])]='-';
                        desclines[iii][strlen(desclines[iii])+1]='\0';
                        //putchar('-'); // print '-' if last-but-one column
                    }
                    pos = 0;
                    //putchar('\n');
                    if (iii<MAXLINES) iii++;
                }
                desclines[iii][strlen(desclines[iii])]=word[k];
                //putchar( word[k] );
                pos++;
                k++;
            }
        } else {
            if( pos + j > N ) // Word doesn't fit in line
            {
                //putchar('\n');
                pos = 0;
                if (iii<MAXLINES) iii++;
            }
            strcat(desclines[iii],word);				// add word
            //printf("%s", word );
            pos += j;
        }
        if( pos < N-1 ) // print space if not last column
        {
            strcat(desclines[iii]," ");
            //putchar(' ');
            pos++;
        }
        if( str[i] == '\0' ) break;
        i++; // Skip space
    }

    // show tv prgoram subtitle
    glLoadIdentity();
    glColor3f(1,1,1);
//    glScalef(3.8, 3.8, 3.8);
    glRotatef(45.0f,0.0f, 0.0f, 0.0f);
    switch(screen_size) {
        case 1:	glTranslatef(39.0f-30.0f, 0.0f+12.5f, -60.0f);
                break;
        case 2:	glTranslatef(39.0f-30.0f, 0.0f+12.5f, -60.0f);
                break;
        case 3:	glTranslatef(48.0f-30.0f, 0.0f+12.5f, -60.0f);
                break;
        case 4:	glTranslatef(50.0f-30.0f, 0.0f+12.5f, -60.0f);
                break;
    }
    glcRenderString(tvkanaler[selectchanel].tv_prog_guide[selectprg].sub_title);

    //
    // draw tv program big info
    //
    i=0;
    while(i<22) {
        if (strcmp(desclines[i],"")!=0) {
            glLoadIdentity();
            glcScale(3.0f,3.0f);
            glColor3f(1.0f, 1.0f, 1.0f);
            switch(screen_size) {
                case 1:	glTranslatef(39.0f-30.0f, 0.0f+10.5f-(i*1.2f), -60.0f);
                        break;
                case 2:	glTranslatef(39.0f-30.0f, 0.0f+10.5f-(i*1.2f), -60.0f);
                        break;
                case 3:	glTranslatef(48.0f-30.0f, 0.0f+10.5f-(i*1.2f), -60.0f);
                        break;
                case 4:	glTranslatef(50.0f-30.0f, 0.0f+10.5f-(i*1.2f), -60.0f);
                        break;
            }
            glcRenderString(desclines[i]);
        }
        i++;
    }
    // show clock
    glLoadIdentity();
    sprintf(tmptxt,"%02d:%02d",timeinfo->tm_hour,timeinfo->tm_min);
    glcScale(3.0f,3.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    switch(screen_size) {
        case 1: glTranslatef(8.0f, -8.0f, -22.0f);
                break;
        case 2: glTranslatef(8.0f, -8.0f, -22.0f);
                break;
        case 3: glTranslatef(8.0f, -8.0f, -22.0f);
                break;
        case 4: glTranslatef(8.0f, -8.0f, -22.0f);
                break;
    }
//    glcRenderStyle(GLC_BITMAP);
    glcRenderString(tmptxt);    				// render clock
//    glcRenderStyle(GLC_TRIANGLE );


    glEnable(GL_TEXTURE_2D);
    // old recordeings button mask
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
        case 2: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
        case 3: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
        case 4: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glBindTexture(GL_TEXTURE_2D, _tvmaskprgrecordedbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(10, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-10, 5, 0.0);
    glEnd();

    // old recordings button gfx
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
        case 2: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
        case 3: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
        case 4: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
                //            glColor4f(1.0f,1.0f,1.0f,1.0f);
                //            glBlendFunc(GL_ONE, GL_ONE);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, _tvoldprgrecordedbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(44);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(10, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-10, 5, 0.0);
    glEnd();

    // End button 1 (old recordings)

    // new recordeings button mask
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
        case 2: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
        case 3: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
        case 4: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glBindTexture(GL_TEXTURE_2D, _tvmaskprgrecordedbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(10, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-10, 5, 0.0);
    glEnd();

    // new recordings button gfx
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
        case 2: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
        case 3: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
        case 4: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
                //            glColor4f(1.0f,1.0f,1.0f,1.0f);
                //            glBlendFunc(GL_ONE, GL_ONE);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, _tvnewprgrecordedbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(45);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(10, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-10, 5, 0.0);
    glEnd();

    // End button 2 (new recordings)

}


//
// viser et prgrams record info.
//

void tv_oversigt::showandsetprginfo(int kanalnr,int tvprgnr) {
    char tmptxt[200];
    time_t aktueltid;
    time_t prgtid;
    time(&aktueltid);					// hent hvad klokken er
    struct tm *timeinfo;
    struct tm prgtidinfo;
    static int antalrec=-1;
    timeinfo=localtime(&aktueltid);				// convert to localtime


    glLoadIdentity();
    glScalef(3.6f, 3.6f, 3.6f);
    glDisable(GL_BLEND);
//    glDisable(GL_TEXTURE);
    switch (screen_size) {
        case 1: glTranslatef(-20.0f, 8.0f, -60.0f);
                break;
        case 2: glTranslatef(-20.0f, 8.0f, -60.0f);
                break;
        case 3: glTranslatef(-20.0f, 8.0f, -60.0f);
                break;
        case 4: glTranslatef(-20.0f, 8.0f, -60.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D,0);
    glcRenderString("Kanal");
    glLoadIdentity();
    glTranslatef(-14.0f, 8.0f, -60.0f);
    sprintf(tmptxt,"%s",tvkanaler[kanalnr].chanel_name);
    glcRenderString(tmptxt);

    glLoadIdentity();
    glTranslatef(-20.0f, 8.0f-1.2f, -60.0f);
    glcRenderString("Navn");
    glLoadIdentity();
    glTranslatef(-14.0f, 8.0f-1.2f, -60.0f);
    sprintf(tmptxt,"%s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn);
    glcRenderString(tmptxt);


    glLoadIdentity();
    glTranslatef(-20.0f, 8.0f-2.4f, -60.0f);
    glcRenderString("Start");
    glLoadIdentity();
    glTranslatef(-14.0f, 8.0f-2.4f, -60.0f);
    sprintf(tmptxt,"%s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime);
    glcRenderString(tmptxt);

    glLoadIdentity();
    glTranslatef(-20.0f, 8.0f-3.6f, -60.0f);
    glcRenderString("Længde ");
    glLoadIdentity();
    glTranslatef(-14.0f, 8.0f-3.6f, -60.0f);
    sprintf(tmptxt,"%d min.",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_length_minuter);
    glcRenderString(tmptxt);



    glLoadIdentity();
    glTranslatef(-20.0f, 8.0f-4.8f, -60.0f);
    glcRenderString("Type ");
    glLoadIdentity();
    glTranslatef(-14.0f, 8.0f-4.8f, -60.0f);
    if (tvkanaler[kanalnr].tv_prog_guide[tvprgnr].prg_type<=7)
        sprintf(tmptxt,"%s",prgtypee[tvkanaler[kanalnr].tv_prog_guide[tvprgnr].prg_type]);
    else strcpy(tmptxt,"");
    glcRenderString(tmptxt);

    if (antalrec==-1) antalrec=tvprgrecordedbefore(tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn,tvkanaler[kanalnr].chanid);

    glLoadIdentity();
    glTranslatef(-20.0f, 8.0f-6.0f, -60.0f);
    glcRenderString("Recorded ");
    glLoadIdentity();
    glTranslatef(-14.0f, 8.0f-6.0f, -60.0f);
    sprintf(tmptxt,"%d times before.",antalrec);
    glcRenderString(tmptxt);


    if (strptime(tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime,"%Y-%m-%d %H:%M:%S",&prgtidinfo)==NULL) {
        printf("RECORDED PROGRAM DATE FORMAT ERROR can't convert. by strptime\n");
    }
    prgtid=mktime(&prgtidinfo);

    //printf("kl er %d prg  starttid er %d diff is %f \n",aktueltid,prgtid,difftime(aktueltid,prgtid));

    sprintf(tmptxt,"%d",tvkanaler[kanalnr].chanid);

    if ((difftime(aktueltid,prgtid)<=0) && (tvprgrecorded(tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime,tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn,tmptxt)==0)) {
        // record button mask
        glLoadIdentity();
        switch (screen_size) {
            case 1: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
            case 2: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
            case 3: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
            case 4: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
        }
        glColor3f(1.0f, 1.0f, 1.0f);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_DST_COLOR, GL_ZERO);

        // error
        glBindTexture(GL_TEXTURE_2D, _textureId13);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(-5, -5, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(5, -5, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(5, 5, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(-5, 5, 0.0);
        glEnd();

        // record button gfx
        glLoadIdentity();
        switch (screen_size) {
            case 1: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
            case 2: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
            case 3: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
            case 4: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
        }
        glColor3f(1.0f, 1.0f, 1.0f);
                    //            glColor4f(1.0f,1.0f,1.0f,1.0f);
                    //            glBlendFunc(GL_ONE, GL_ONE);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindTexture(GL_TEXTURE_2D, _tvrecordbutton);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(41);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(-5, -5, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(5, -5, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(5, 5, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(-5, 5, 0.0);
        glEnd();
        // End button 1
    }

    // cancel button mask
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
        case 2: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
        case 3: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
        case 4: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glBindTexture(GL_TEXTURE_2D, _textureId13);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-5, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(5, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(5, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-5, 5, 0.0);
    glEnd();

    // cancel button gfx
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
        case 2: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
        case 3: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
        case 4: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
                //            glColor4f(1.0f,1.0f,1.0f,1.0f);
                //            glBlendFunc(GL_ONE, GL_ONE);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, _tvrecordcancelbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(40);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-5, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(5, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(5, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-5, 5, 0.0);
    glEnd();

    // End button 1
}

//
//*******************************************************************************************************************************//
//

//
// construktor
//


earlyrecorded::earlyrecorded() {
    for(int i=0;i<199;i++) {
        strcpy(this->programinfo[i].name,"");
        strcpy(this->programinfo[i].dato,"");
    }
}


//
// load recorded history not programs
//


void earlyrecorded::earlyrecordedload(char *mysqlhost,char *mysqluser,char *mysqlpass) {
    int i;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *database = (char *) "mythconverg";
    // mysql stuf
    char sqlselect[200];
    strcpy(sqlselect,"SELECT title,starttime FROM recorded order by starttime limit 60");
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, mysqlhost,mysqluser, mysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    i=0;
    if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (i<=200)) {
            strcpy(this->programinfo[i].dato,row[1]);
            strcpy(this->programinfo[i].name,row[0]);
            i++;
        }
    }
    this->antal=i--;
}


//
// viser liste over record(historie) som har været optaget en gang.
// dvs det er ike optagelser som ligger
//

void earlyrecorded::showearlyrecorded() {
    int i;
    glColor3f(1.0f, 1.0f, 1.0f);
    // img bg
    glLoadIdentity();
    glEnable(GL_TEXTURE);
    glTranslatef(-10.0f, 0.0f, -110.0f);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, _tvoldrecorded);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-50.0, -33.0, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f( 70.0, -33.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f( 70.0, 30.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-50.0, 30.0, 0.0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glColor3f(1.0f, 1.0f, 1.0f);
    glLoadIdentity();
    glTranslatef(-4.5f, 9.7f, -40.0f);
    glcRenderString(oldrecordning);

    i=0;
    glColor3f(0.8f, 0.8f, 0.8f);
    while((i<this->antal) && (i<40)) {
        if (strcmp(this->programinfo[i].name,"")!=0) {
            glLoadIdentity();
            if (i<20) glTranslatef(-30.0f, 12.5f-(i*1.2f), -60.0f); else glTranslatef(5.0f, 12.5f-((i-20)*1.2f), -60.0f);
            glcRenderCountedString(16,this->programinfo[i].dato);
            glLoadIdentity();
            if (i<20) glTranslatef(-22.0f, 12.5f-(i*1.2f), -60.0f); else glTranslatef(13.0f, 12.5f-((i-20)*1.2f), -60.0f);
            glcRenderString(this->programinfo[i].name);
        }
        i++;
    }

    // Close button
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
        case 2: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
        case 3: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
        case 4: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, _tvrecordcancelbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(40);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-7.5f, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(7.5f, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(7.5f, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-7.5f, 5, 0.0);
    glEnd();

    // End button 1

}


// ***********************************************************************************************************************************


char *read_stream(char *stream,size_t size, void *d) {
    char *c=fgets(stream,size,(FILE *) d);
    return c;
}

//
// load recorded history not programs
//


void earlyrecorded::getrecordprogram(char *mysqlhost,char *mysqluser,char *mysqlpass) {
    int i=-1;
    bool startcoll=false;
//    icalvalue *v;
    char* line;
    char endtid[200];
    FILE* stream;
//    icalcomponent *c;
    /* Create a new parser object */
    icalparser *parser = icalparser_new();
    stream=fopen("rectv.rss","r");
    /* Tell the parser what input routie it should use. */
    if (stream) {
        icalparser_set_gen_data(parser,stream);
        do {
            strcpy(endtid,"");
            line = icalparser_get_line(parser,read_stream);
            if (line) {
//            printf("LINE IS = %s i is = %d \n",line,i);
//      	  c = icalparser_add_line(parser,line);
                if (strncmp(line,"BEGIN:VEVENT",12)==0) {
                    i++;
                    startcoll=true;
                    strcpy(endtid,"");					// reset
                }
                if ((i>=0) && (startcoll)) {
                    if (strncmp(line,"DTSTART:",8)==0) {
                        strncpy(this->programinfo[i].dato,line+8,4);		// hente dato
                        strcat(this->programinfo[i].dato,"-");			//
                        strncat(this->programinfo[i].dato,line+12,2);
                        strcat(this->programinfo[i].dato,"-");
                        strncat(this->programinfo[i].dato,line+14,2);
                        strcat(this->programinfo[i].dato," ");
                        strncat(this->programinfo[i].dato,line+17,2);             	// hente tidspunkt
                        strcat(this->programinfo[i].dato,":");
                        strncat(this->programinfo[i].dato,line+19,2);             	// hente tidspunkt
                    }
                    if (strncmp(line,"DTEND:",6)==0) {
                        strncat(this->programinfo[i].endtime,line+15,2);              // hente tid
                        strcat(this->programinfo[i].endtime,":");
                        strncat(this->programinfo[i].endtime,line+17,2);
                    }
                    if (strncmp(line,"SUMMARY:",8)==0) {
                        strcpy(this->programinfo[i].name,line+8);			// hente dato
                    }
                    if (strncmp(line,"LOCATION:",9)==0) {

                    }
                }
                if (strncmp(line,"END:VEVENT",10)==0) {
                    startcoll=false;
                }
            }
        } while ((line!=0) && (i<21));
        this->antal=i;
        icalparser_free(parser);
    }
    if (stream) fclose(stream);
}



//
// viser liste over tv programmer som skal optages.
//

void earlyrecorded::showtvreclist() {
//    char tmptxt[200];
    int i;
//    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);

    glColor4f(1.0f, 1.0f, 1.0f,1.0f);
        // mask
    glLoadIdentity();
    glTranslatef(-10.0f, 0.0f, -110.0f);
    glEnable(GL_TEXTURE);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    // img
    glLoadIdentity();
    glEnable(GL_TEXTURE);
    glTranslatef(-10.0f, 0.0f, -110.0f);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, _tvoldrecorded);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-50.0, -33.0, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f( 70.0, -33.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f( 70.0, 30.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-50.0, 30.0, 0.0);
    glEnd();
    // overskrift
    glLoadIdentity();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE);
    glBindTexture(GL_TEXTURE_2D,0);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(-6.0f, 9.7f, -40.0f);
    glcRenderString(upcommingrec);
    i=0;
    glColor4f(1.0f, 1.0f, 1.0f,1.0f);
    while((i<this->antal) && (i<50)) {
        if (strcmp(this->programinfo[i].name,"")!=0) {
            glLoadIdentity();
            if (i<25) glTranslatef(-30.0f, 12.5f-(i*1.2f), -60.0f); else glTranslatef(5.0f-3.4f, 12.5f-((i-25)*1.2f), -60.0f);
            glcRenderString(this->programinfo[i].dato);
            glcRenderString("-");
            glcRenderString(this->programinfo[i].endtime);
            glLoadIdentity();
            if (i<25) glTranslatef(-18.5f, 12.5f-(i*1.2f), -60.0f); else glTranslatef(12.0f-3.4f, 12.5f-((i-25)*1.2f), -60.0f);
            glcRenderString(this->programinfo[i].name);
        }
        i++;
    }



    // Close button
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
        case 2: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
        case 3: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
        case 4: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, _tvrecordcancelbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(40);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-7.5f, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(7.5f, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(7.5f, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-7.5f, 5, 0.0);
    glEnd();

    // End button 1
}
