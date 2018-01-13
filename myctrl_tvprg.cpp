#include <mysql.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

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
#include "myth_setup.h"

#define MAXSIZE 16000+1;

extern GLuint setupnetworkwlanback;

extern tv_graber_config aktiv_tv_graber;                                        // xmltv graber config

extern int screen_size;
extern int debugmode;
extern int fonttype;

extern int configland;

extern GLuint _tvbar1;
extern GLuint _tvbar3;
extern GLuint _textureIdclose;
extern GLuint _textureclose;                                                  // in use
extern GLuint tvprginfobig;
extern GLuint _tvprgrecorded;
extern GLuint _tvprgrecordedr;
extern GLuint _tvprgrecorded1;
extern GLuint _tvprgrecorded_mask;
extern GLuint _tvrecordbutton;
GLuint _textureId13;
extern GLuint _texturemovieinfobox;
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

extern GLuint _textureutvbgmask;
extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];                              //
extern char configmusicmypath[];                               //
extern char configbackend_tvgraber[256];                       // internal tv graber to use
extern long configtvguidelastupdate;                           //

extern bool loading_tv_guide;

//extern earlyrecorded oldrecorded;
//extern earlyrecorded newtcrecordlist;


const float prgtypeRGB[]={0.2f,0.8f,0.2f ,0.5f,0.9f,0.0f,                       /* Action, Series       */
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
                              0.4f,0.7f,0.7f ,0.1f,0.1f,0.1f};                  /*none,  ukendt (last)  */




const char *prgtypee[2*12]={"Action"," Series",
                            "News","Kids",
                            " Music","Animation",
                            " Horror"," Drama",
                            " sci fi","Comedies",
                            "Romance","Thriller",
                            "Fiction","Musical",
                            "War"," Doc.",
                            "Adventure","Comedie",
                            "Documentary","Adult",
                            "Sport","Miniseries",
                            "None","Unknown"};

// bruges af show_tvoversigt

void myglprinttv(char *string) {
    int len,i;
    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
       glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
    }
}


//
// ceck if process is running
//

bool check_tvguide_process_running(char *processname) {
  int processid=0;
  bool status=false;
  char processcheckstr[1024];
  sprintf(processcheckstr,"pidof -x %s > /dev/null",processname);
  if (0==system("pidof -x tv_grab_dk_dr > /dev/null")) {
    status=true;
     //A process having name PROCESS is running.
  }
  else if (1==system("pidof -x tv_grab_dk_dr > /dev/null")) {
    status=false;
    //A process having name PROCESS is NOT running.
  }
  return(status);
}


// intern function for _xmltv

int get_tvguide_fromweb() {
  char exestring[2048];
  int result=-1;
  // check if active xml_tv graber is running
  if (check_tvguide_process_running((char *) aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr])==false) {
    strcpy(exestring,configbackend_tvgraber);
    strcat(exestring," --days 1 --output ~/tvguide.xml 2> ~/tvguide.log");
    printf("Start tv graber background process %s\n",configbackend_tvgraber);
    result=system(exestring);
    //  if (WIFSIGNALED(result) && (WTERMSIG(result) == SIGINT || WTERMSIG(result) == SIGQUIT)) break;
    printf("Done tv graber background process exit kode %d\n",result);
  } else printf("Graber is already ruuning.\n");
  return(result);
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



void expand_escapes(char* dest, const char* src) {
  char c;
  while (c = *(src++)) {
    switch(c) {
      case '\a':
        *(dest++) = '\\';
        *(dest++) = 'a';
        break;
      case '\b':
        *(dest++) = '\\';
        *(dest++) = 'b';
        break;
      case '\t':
        *(dest++) = '\\';
        *(dest++) = 't';
        break;
      case '\n':
        *(dest++) = '\\';
        *(dest++) = 'n';
        break;
      case '\v':
        *(dest++) = '\\';
        *(dest++) = 'v';
        break;
      case '\f':
        *(dest++) = '\\';
        *(dest++) = 'f';
        break;
      case '\r':
        *(dest++) = '\\';
        *(dest++) = 'r';
        break;
      case '\\':
        *(dest++) = '\\';
        *(dest++) = '\\';
        break;
      case '\"':
        *(dest++) = '\\';
        *(dest++) = '\"';
        break;
        case '\'':
          *(dest++) = '\\';
          *(dest++) = '\"';
          break;
      default:
        *(dest++) = c;
     }
  }

  *dest = '\0'; /* Ensure nul terminator */
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
  unsigned int prg_antal=0;
  char temptxt[1024];

  char sql[8192];
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
  char category[1924];
  char description[4096];
  xmlChar *title;
  bool gettchannel=false;
  xmlChar *desc;
  unsigned long channelid;
  bool fundet=false;
  char *database = (char *) "mythconverg";
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;

  struct stat t_stat;           // file info struct
  struct tm* lastmod;           //

  loading_tv_guide=true;        // set loadtv guide flag to show in show_tv_guide then xml files is passed
  // mysql stuf
  conn=mysql_init(NULL);
  // Connect to database
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, NULL, 0, NULL, 0);
  if (!(conn)) error=1;
  mysql_query(conn,"set NAMES 'utf8'");
  res = mysql_store_result(conn);
  mysql_free_result(res);

  strcpy(description,"");

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
    // if tvguide db not exist create it.
    strcpy(sql,"create table IF NOT EXISTS channel(chanid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,channum varchar(10),freqid varchar(10) ,sourceid int(10) unsigned,callsign varchar(20),name  varchar(64), icon varchar(255), finetune int(11) , videofilters varchar(255), xmltvid varchar(64), recpriority int(10), contrast int(11) DEFAULT 32768, brightness int(11) DEFAULT 32768, colour int(11) DEFAULT 32768, hue int(11) DEFAULT 32768, tvformat varchar(10), visible tinyint(1) DEFAULT 1, outputfilters  varchar(255) ,useonairguide tinyint(1)  DEFAULT 0, mplexid  smallint(6), serviceid  mediumint(8) unsigned, atsc_major_chan int(10) unsigned DEFAULT 0, atsc_minor_chan int(10) unsigned DEFAULT 0, last_record datetime, default_authority varchar(32), commmethod int(11) DEFAULT +1, iptvid smallint(6) unsigned,orderid int(12) unsigned DEFAULT 0)");
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    mysql_free_result(res);
    //
    strcpy(sql,"create table IF NOT EXISTS program(chanid int(10) unsigned NOT NULL,starttime datetime, endtime datetime ,title varchar(128),subtitle varchar(128), description text, category varchar(64), category_type varchar(64), airdate year(4),stars float,previouslyshown tinyint(4), title_pronounce varchar(128), stereo tinyint(1), subtitled tinyint(1),hdtv tinyint(1), closecaptioned tinyint(1), partnumber int(11), parttotal int(11), seriesid  varchar(12), originalairdate date, showtype varchar(30), colorcode varchar(20), syndicatedepisodenumber varchar(20), programid varchar(64), manualid int(10) unsigned, generic tinyint(1), listingsource int(11), first tinyint(1), last tinyint(1) ,audioprop varchar(8),subtitletypes varchar(8),videoprop varchar(8))");
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    mysql_free_result(res);
    //
    strcpy(sql,"create table IF NOT EXISTS programgenres(chanid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,starttime datetime,relevance char(1),genre varchar(30))");
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    mysql_free_result(res);
    //
    strcpy(sql,"create table IF NOT EXISTS programrating(chanid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,starttime datetime,system varchar(8), rating varchar(16))");
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    mysql_free_result(res);
    //
    strcpy(sql,"create table IF NOT EXISTS record(recordid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,type int(10),chanid  int(10) unsigned,starttime time,startdate date,endtime time,enddate date,title varchar(128),subtitle varchar(128),description varchar(16000),season smallint,episode smallint,category varchar(64),profile varchar(128),recpriority int,autoexpire int,maxepisodes int,maxnewest int,startoffset int,endoffset int,recgroup varchar(32),dupmethod int,dupin int,station  varchar(20),seriesid varchar(64),programid varchar(64),inetref varchar(40),search int,autotranscode int, autocommflag  int,autouserjob1 int, autouserjob2 int,autouserjob3 int, autouserjob4 int,autometadata int,findday int,findtime time,findid int,inactive int,parentid int,transcoder int,playgroup varchar(32),prefinput int,next_record datetime,last_record datetime,last_delete datetime,storagegroup varchar(32),avg_delay int,filter int)");
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    mysql_free_result(res);
    // The recorded table, lists programs which have already been recorded (or recorded and then transcoded) and are still available for viewing, translating the Internal Filenames into something safe for human consumption.
    strcpy(sql,"create table IF NOT EXISTS recorded(chanid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,starttime datetime,endtime datetime,title varchar(128),subtitle varchar(128),description varchar(16000),season smallint,episode smallint,category varchar(64),hostname varchar(255),bookmark int default 0,editing int(10) unsigned default 0,cutlist tinyint(1) default 0,autoexpire int(11) default 0,commflagged int(10) unsigned default 0,recgroup varchar(32) default 'Default',recordid int(11),seriesid varchar(64),programid varchar(64),inetref varchar(40),lastmodified timestamp,filesize bigint(20) default 0,stars float default 0.0,previouslyshown tinyint(1) default 0,originalairdate date,preserve tinyint(1) default 0,findid int(11) default 0,deletepending tinyint(1),transcoder int default 0,timestretch float default 1,recpriority int default 0,basename varchar(255),progstart datetime,progend datetime,playgroup varchar(32),profile varchar(32),duplicate tinyint(1) default 0,transcoded tinyint(1) default 0,watched tinyint(4) default 0,storagegroup varchar(32) default 'Default',bookmarkupdate datetime)");
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    mysql_free_result(res);
    //
    strcpy(sql,"create table IF NOT EXISTS Recgrouppassword(recgroup varchar(32),password varchar(10))");
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    mysql_free_result(res);
    //
    strcpy(sql,"create table IF NOT EXISTS Recordingprofiles(id int(10) unsigned,name varchar(128),videocodec varchar(128),audiocodec varchar(128),profilegroup int(10) unsigned)");
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
    strcat(path,filename);                                                      // add filename to xmlfile name

    // get file date
    stat(path, &t_stat);                                                        // get file info like create date
    lastmod=localtime(&(t_stat.st_mtime));                                      // convert to unix time

    // if file change from last run. update tv guide again
    if ((configtvguidelastupdate!=mktime(lastmod)) || (configtvguidelastupdate==0)) {
      // save last updated
      setlastupdate(mktime(lastmod));
      configtvguidelastupdate=mktime(lastmod);
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
                //xmlFree(content);
              }

              // get node childen
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
//                  if (debugmode & 256) printf("From: %20s", starttime);
                  xmlFree(tmpdat);
                }

                tmpdat=xmlGetProp(node,( xmlChar *) "desc");
                if (tmpdat) {
                  strncpy(description,(char *) tmpdat,4095);                             // get desc
                }


                //xmlFree(tmpdat);
                tmpdat=xmlGetProp(node,( xmlChar *) "category");
                if (tmpdat) {
                  if (debugmode & 256) {
                    if (tmpdat) printf("category: %s\n", tmpdat);
                  }
                  if (strcmp("Serier",(char *) tmpdat)==0) strcpy(category,"Serier");
                  else if (strcmp("Natur",(char *) tmpdat)==0) strcpy(category,"Natur");
                  else if (strcmp("Dokumentar",(char *) tmpdat)==0) strcpy(category,"Dokumentar");
                  else if (strcmp("Underholdning",(char *) tmpdat)==0) strcpy(category,"Underholdning");
                  else if (strcmp("Sport",(char *) tmpdat)==0) strcpy(category,"sport");
                  else if (strcmp("Film",(char *) tmpdat)==0) strcpy(category,"Film");
                  else strcpy(category,"None");
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
//                if (debugmode & 256) printf(" ->%20s", endtime);
                }
                //xmlFree(tmpdat);

                xmltvid=xmlGetProp(node1,( xmlChar *) "channel");
                if ((xmltvid) && (gettchannel==false)) {
                  gettchannel=true;
                  strcpy(channelname,(char *) xmltvid);
  //                if (debugmode & 256) printf(" %s",channelname);
                }
                // save channelname to show in update
                strcpy(this->loadinginfotxt,channelname);

                //xmlFree(tmpdat);
//                if (debugmode & 256) printf("\n");
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
              // convert spec chars to esc string in string
              expand_escapes(temptxt,prgtitle);
              strncpy(prgtitle,temptxt,1024-1);
              if (!(do_program_exist(channelid,prgtitle,starttime))) {
                if (strcmp("",(char *) category)==0) strcpy(category,"None");
                // create/update record in program guide table

                // convert spec chars to esc string in string
                expand_escapes(temptxt,prgtitle);
                strncpy(prgtitle,temptxt,1024-1);

                // convert spec chars to esc string in string
                expand_escapes(temptxt,description);
                strncpy(description,temptxt,4096-1);

                sprintf(sql,"REPLACE into program (chanid,starttime,endtime,title,subtitle,description,category) values(%ld,'%s','%s','%s','%s','%s','%s')",channelid,starttime,endtime,prgtitle,"","",category);
                mysql_query(conn,sql);
                res = mysql_store_result(conn);
                mysql_free_result(res);
                prg_antal++;
                if (debugmode & 256) fprintf(stdout,"#%4d of Tvguide records created.... Channel %20s %s->%s %s \n",prg_antal,channelname,starttime,endtime,prgtitle);
              } else {
                if (debugmode & 256) fprintf(stdout,"Tvguide Program exist Channel......         %20s %s->%s %s \n",channelname,starttime,endtime,prgtitle);
              }
            }
            // save rec
          }
        } // for loop end
        fprintf(stdout, "...\n");
        xmlFreeDoc(document);
      } else {
        printf("tvguide.xml not found \n");
      }

    }
  }
  loading_tv_guide=false;
  mysql_close(conn);
  return(!(error));
}


//
// clear tv guide (drop db)
//

void tv_oversigt::cleartvguide() {
  char sql[4096];
  int error=0;
  bool fundet=false;
  char *database = (char *) "mythconverg";
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  // mysql stuf
  conn=mysql_init(NULL);
  // Connect to database
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, NULL, 0, NULL, 0);
  if (!(conn)) error=1;
  if (conn) {
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_free_result(res);
    sprintf(sql,"DROP DATABASE %s",database);
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    if (res) mysql_free_result(res);
    mysql_close(conn);
  }
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
    time_t rawtime;
    struct tm *timelist;
    kanal_antal=0;
    strcpy(mysqllhost,"");
    strcpy(mysqlluser,"");
    strcpy(mysqllpass,"");
    strcpy(loadinginfotxt,"");
    lastupdated=0;
    time(&rawtime);
    // convert clovk to localtime
    timelist=localtime(&rawtime);
    vistvguidekl=timelist->tm_hour;
    if (vistvguidekl>24) vistvguidekl=0;
}


// destructor

tv_oversigt::~tv_oversigt() {
}


void tv_oversigt::reset_tvguide_time() {
  time_t rawtime;
  struct tm *timelist;
  time(&rawtime);
  timelist=localtime(&rawtime);
  vistvguidekl=timelist->tm_hour;
  if (vistvguidekl>24) vistvguidekl=0;
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

                sprintf(sqlselect,"INSERT INTO record values (0,1,%u,'12:00:00','2018-01-02','12:00:00','2018-01-02',\"%s\",\"%s\",\"%s\",11,12,\"%s\",'pro',15,16,17,18,19,20,'Default',22,23,'station','serid','prgid','intref',28,29,30,31,32,33,34,35,36,'12:00:12',38,39,40,41,'playgroup',43,'2017-01-02 12:00:00','2017-01-02 12:00:00','2017-01-02 12:00:00','storegrp',48,49)",row[6],tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn, tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].sub_title,tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].description,row[8]);

                printf("sql record is %s\n",sqlselect);


                //sprintf(sqlselect,"INSERT INTO record values (0,1,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",'Default',0,0,1,0,0,0,'Default',6,15,\"%s\",%u,'',0,0,0,0,0,0,0,0,TIME('%s'),%lu,0,0,0,'Default',0,'0000-00-00 00:00:00','','0000-00-00 00:00:00','Default',100)", row[6], row[1], row[2], row[3],row[4], tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn, tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].sub_title, tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].description, row[7],row[0], ELFHash(tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn) , row[1], ((tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime_unix)/60/60/24)+719528);
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
// bruges ved pil up/down/left/right i tv kanal listen i vis_tv_oversigt
//

int tv_oversigt::findguidetvtidspunkt(int kanalnr,time_t tidspunkt) {
    int prgnr=0;
    bool fundet=false;
    tidspunkt+=60*60;
    while((prgnr<tvkanaler[kanalnr].program_antal()) && (!(fundet))) {
        if ((time_t) tvkanaler[kanalnr].tv_prog_guide[prgnr].starttime_unix<tidspunkt) {
          if ((tvkanaler[kanalnr].tv_prog_guide[prgnr].starttime_unix<tidspunkt) && (tvkanaler[kanalnr].tv_prog_guide[prgnr].endtime_unix<tidspunkt)) prgnr++;
          else fundet=true;
        } else fundet=true;
    }
    if (fundet) {
      while (((tvkanaler[kanalnr].tv_prog_guide[prgnr].starttime_unix<tidspunkt) && (tvkanaler[kanalnr].tv_prog_guide[prgnr].endtime_unix<tidspunkt)) && (prgnr<tvkanaler[kanalnr].program_antal())) {
        prgnr++;
      }
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
    int kanalnr,prgnr;
    int totalantalprogrammer=0;
    int prgtype;
    bool recorded;
    unsigned int huskprgantal=0;
    char sqlselect[512];
    char dagsdato[128];
    char enddate[128];
    time_t rawtime;
    time_t rawtime2;
    struct tm *timeinfo;
    struct tm *timeinfo2;
    struct tm timeinfo3;
    //struct tm timeinfo;
    //struct tm timeinfo2;
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

    // is startid as args ? 0
    if (nystarttid==0) {
        rawtime=rawtime2=time(NULL);				     			// hent nu tid
    } else {
        // hent ny starttid
        rawtime=this->starttid;             // start tid type (time_t unix time)
        rawtime2=this->starttid+(60*60*24); // end   tid type (time_t unix time)
    }

    timeinfo=localtime(&rawtime);			                        		// lav om til local time
    timeinfo2=localtime(&rawtime2);	            		          		  // lav om til local time
    // copy struct
    timeinfo3.tm_mday=timeinfo->tm_mday;
    timeinfo3.tm_mon=timeinfo->tm_mon;
    timeinfo3.tm_year=timeinfo->tm_year;
    timeinfo3.tm_min=timeinfo->tm_min;
    timeinfo3.tm_sec=timeinfo->tm_sec;
    timeinfo3.tm_hour=timeinfo->tm_hour;
    timeinfo3.tm_wday=timeinfo->tm_wday;
    timeinfo3.tm_isdst=timeinfo->tm_isdst;
    timeinfo3.tm_mday+=1;
    mktime(&timeinfo3);

    printf("raw start time %d  \nraw end time %d ",timeinfo->tm_mday,timeinfo2->tm_mday);
    sprintf(dagsdato,"%04d-%02d-%02d 00:00:00",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday);
    sprintf(enddate,"%04d-%02d-%02d 23:59:59",timeinfo3.tm_year+1900,timeinfo3.tm_mon+1,timeinfo3.tm_mday);
    //strftime(dagsdato, 128, "%Y-%m-%d 00:00:00", timeinfo);		        // lav nu tids sting strftime(dagsdato, 128, "%Y-%m-%d %H:%M:%S", timeinfo );
    //strftime(enddate, 128, "%Y-%m-%d 23:59:59", timeinfo2);		        // lav nu tids sting
    this->starttid=rawtime;						                                // gem tider i class
    this->sluttid=rawtime2;						                                //
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
        strcat(sqlselect,"' and starttime>='");
        strcat(sqlselect,dagsdato);
        strcat(sqlselect,"' order by orderid,chanid,abs(channel.channum),starttime");

        if (debugmode & 256) printf("Tv guide sql = %s \n",sqlselect);

        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        kanalnr=0;
        prgnr=0;
        if (res) {
            while (((row = mysql_fetch_row(res)) != NULL) && (prgnr<=maxprogram_antal-1) && (kanalnr<MAXKANAL_ANTAL-1)) {
                if ((prgnr==0) && (strcmp(tmptxt,row[0])!=0)) {
                    tvkanaler[kanalnr].putkanalname(row[0]);
                    tvkanaler[kanalnr].chanid=atoi(row[11]);                      // set chanelid in array
                    strcpy(tmptxt,row[0]);                                        // rember channel name
                    printf("Channel name : %s ",tvkanaler[kanalnr].getkanalname());
                }
                // select by tv_grab_xx nr in array
                if (row[8]) {
                  switch (aktiv_tv_graber.graberaktivnr) {
                      // graber_dk
                    case 0:
                        if (strcmp(row[8],"None")!=0) {
                          if (strcmp("series",row[9])==0) prgtype=1;
                          else if (strcmp("movie",row[9])==0) prgtype=5;
                          else if (strcmp("sport",row[9])==0) prgtype=2;
                          else if (strcmp("cartoon",row[9])==0) prgtype=3;
                          else if (strcmp("animation films",row[9])==0) prgtype=3;
                          else if (strcmp("Animation films",row[9])==0) prgtype=3;
                          else if (strcmp("news",row[9])==0) prgtype=4;
                          else if (strcmp("nature",row[9])==0) prgtype=9;
                          else prgtype=0;
                        }
                        break;
                    case 1:
                      // graber_dk
                      if (strcmp(row[8],"None")!=0) {
                        if (strcmp("series",row[9])==0) prgtype=1;					           	// serie
                        else if (strcmp("dansk underholdning.",row[9])==0) prgtype=1;   //
                        else if (strcmp("sport",row[9])==0) prgtype=2;                  //
                        else if (strncmp("børn",row[9],4)==0) prgtype=3;                //
                        else if (strncmp("dukkefilm",row[9],9)==0) prgtype=3;           //
                        else if (strstr(row[9],"tegnefilm")!=0) prgtype=3;              //
                        else if (strstr(row[9],"animationsfilm")!=0) prgtype=3;         //
                        else if (strcmp("news",row[9])==0) prgtype=4;                   //
                        else if (strcmp("movie",row[9])==0) prgtype=5;                  //
                        else if (strstr(row[9],"dokumentarserie")!=0) prgtype=6;        //
                        else if (strcmp("engelsk madprogram",row[9])==0) prgtype=11;    //
                        else if (strcmp("dansk reportageserie.",row[9])==0) prgtype=7;  //
                        else if (strcmp("amerikansk krimi.",row[9])==0) prgtype=5;      //
                        else if (strcmp("debatprogram.",row[9])==0) prgtype=7;          //
                        else if (strcmp("music",row[9])==0) prgtype=8;                  //
                        else if (strcmp("dyr",row[9])==0) prgtype=9;                    //
                        else if (strcmp("none",row[9])==0) prgtype=0;                   //
                        else prgtype=0;                                                 // default panic
                      } else prgtype=0;                                                 // default panic
                      break;
                    case 2:
                      if (strcmp(row[8],"None")!=0) {
                        if (strcmp("series",row[9])==0) prgtype=1;
                        else if (strcmp("movie",row[9])==0) prgtype=5;
                        else if (strcmp("sport",row[9])==0) prgtype=2;
                        else if (strcmp("cartoon",row[9])==0) prgtype=3;
                        else if (strcmp("animation films",row[9])==0) prgtype=3;
                        else if (strcmp("Animation films",row[9])==0) prgtype=3;
                        else if (strcmp("news",row[9])==0) prgtype=4;
                        else if (strcmp("nature",row[9])==0) prgtype=9;
                        else prgtype=0;
                      }
                      break;
                    case 3:
                      if (strcmp(row[8],"None")!=0) {
                        if (strcmp("series",row[9])==0) prgtype=1;
                        else if (strcmp("movie",row[9])==0) prgtype=5;
                        else if (strcmp("movie",row[9])==0) prgtype=5;
                        else if (strcmp("sport",row[9])==0) prgtype=2;
                        else if (strcmp("cartoon",row[9])==0) prgtype=3;
                        else if (strcmp("animation films",row[9])==0) prgtype=3;
                        else if (strcmp("Animation films",row[9])==0) prgtype=3;
                        else if (strcmp("news",row[9])==0) prgtype=4;
                        else if (strcmp("nature",row[9])==0) prgtype=9;
                        else prgtype=0;
                      }
                      break;
                    case 4:
                      // graber_ar
                      if (strcmp(row[8],"None")!=0) {
                        if (strcmp("series",row[9])==0) prgtype=1;
                        else if (strcmp("movie",row[9])==0) prgtype=5;
                        else if (strcmp("Documentales",row[9])==0) prgtype=7;
                        else if (strcmp("Culturales",row[9])==0) prgtype=7;
                        else if (strcmp("Variedades",row[9])==0) prgtype=0;
                        else if (strcmp("Deportes",row[9])==0) prgtype=2;
                        else if (strcmp("Noticias",row[9])==0) prgtype=4;
                        else if (strcmp("Musicales",row[9])==0) prgtype=8;
                        else if (strcmp("Cine",row[9])==0) prgtype=0;
                        else if (strcmp("Periodístico",row[9])==0) prgtype=4;
                        else if (strcmp("Infantiles",row[9])==0) prgtype=3;         // kids
                        else if (strcmp("Reality Show",row[9])==0) prgtype=0;
                        else prgtype=0;
                      }
                    case 5:
                        // graber_fi
                      if (strcmp(row[8],"None")!=0) {
                        if (strcmp("series",row[9])==0) prgtype=1;
                        else if (strcmp("movie",row[9])==0) prgtype=5;
                        else if (strcmp("Documentales",row[9])==0) prgtype=7;
                        else if (strcmp("Culturales",row[9])==0) prgtype=7;
                        else if (strcmp("Variedades",row[9])==0) prgtype=0;
                        else if (strcmp("Deportes",row[9])==0) prgtype=2;
                        else if (strcmp("Noticias",row[9])==0) prgtype=4;
                        else if (strcmp("Musicales",row[9])==0) prgtype=8;
                        else if (strcmp("Cine",row[9])==0) prgtype=0;
                        else if (strcmp("Periodístico",row[9])==0) prgtype=4;
                        else if (strcmp("Infantiles",row[9])==0) prgtype=3;         // kids
                        else if (strcmp("Reality Show",row[9])==0) prgtype=0;
                        else if (strcmp("elokuvat",row[9])==0) prgtype=0;
                        else if (strcmp("urheilu",row[9])==0) prgtype=0;
                        else prgtype=0;
                      }
                      break;
                    case 6:
                        // graber_tr
                      if (strcmp(row[8],"None")!=0) {
                        if (strcmp("series",row[9])==0) prgtype=1;                          //
                        else if (strcmp("film",row[9])==0) prgtype=5;                       //
                        else if (strcmp("Life Style",row[9])==0) prgtype=7;                 //
                        else if (strcmp("Aile",row[9])==0) prgtype=7;                       // Family
                        else if (strcmp("Aksiyon",row[9])==0) prgtype=0;                    // Action film
                        else if (strcmp("Animasyon",row[9])==0) prgtype=2;                  // Animation
                        else if (strcmp("Belgesel",row[9])==0) prgtype=4;
                        else if (strcmp("Bilim Kurgu",row[9])==0) prgtype=8;                // Science fiction
                        else if (strcmp("Biyografi",row[9])==0) prgtype=0;
                        else if (strcmp("Komedi",row[9])==0) prgtype=4;
                        else if (strcmp("Animasyon",row[9])==0) prgtype=3;                  // kids
                        else if (strcmp("Magazin",row[9])==0) prgtype=0;
                        else if (strcmp("Eğlence",row[9])==0) prgtype=0;                    // Entertainment
                        else if (strcmp("Haberler",row[9])==0) prgtype=4;                   // news
                        else if (strcmp("Dram",row[9])==0) prgtype=1;                       // series
                        else if (strcmp("Korku",row[9])==0) prgtype=0;
                        else if (strcmp("Diğer",row[9])==0) prgtype=0;
                        else if (strcmp("Gerilim",row[9])==0) prgtype=0;
                        else if (strcmp("Yarışma",row[9])==0) prgtype=0;
                        else if (strcmp("Polisiye",row[9])==0) prgtype=0;
                        else if (strcmp("Çocuk",row[9])==0) prgtype=0;
                        else prgtype=0;
                      }
                      break;
                    case 7:
                      // graber_se_tvzon
                      if (strcmp(row[8],"None")!=0) {
                        if (strcmp("series",row[9])==0) prgtype=1;                          // series
                        else if (strcmp("movie",row[9])==0) prgtype=5;                       //
                        else if (strcmp("Adult",row[9])==0) prgtype=7;                 //
                        else if (strcmp("Drama",row[9])==0) prgtype=7;                       // Family
                        else if (strcmp("Crime",row[9])==0) prgtype=0;                    // Action film
                        else if (strcmp("Mystery",row[9])==0) prgtype=2;                  // Animation
                        else if (strcmp("sports",row[9])==0) prgtype=4;
                        else if (strcmp("Sports",row[9])==0) prgtype=8;                // Science fiction
                        else if (strcmp("Skiing",row[9])==0) prgtype=0;
                        else if (strcmp("Soccer",row[9])==0) prgtype=4;
                        else if (strcmp("Fencing",row[9])==0) prgtype=3;                  // kids
                        else if (strcmp("tvshow",row[9])==0) prgtype=0;
                        else if (strcmp("Comedy",row[9])==0) prgtype=0;                    // Entertainment
                        else if (strcmp("Adventure",row[9])==0) prgtype=4;                   // news
                        else if (strcmp("Family",row[9])==0) prgtype=1;                       // series
                        else if (strcmp("Fantasy",row[9])==0) prgtype=0;
                        else if (strcmp("Series",row[9])==0) prgtype=0;
                        else if (strcmp("Reality",row[9])==0) prgtype=0;
                        else if (strcmp("Miniseries",row[9])==0) prgtype=0;
                        else if (strcmp("TV Movie",row[9])==0) prgtype=0;
                        else if (strcmp("Teleshopping",row[9])==0) prgtype=0;
                        else if (strcmp("tvshow",row[9])==0) prgtype=0;
                        else if (strcmp("Sitcom",row[9])==0) prgtype=0;
                        else if (strcmp("Documentary",row[9])==0) prgtype=0;
                        else if (strcmp("Magazine",row[9])==0) prgtype=0;
                        else if (strcmp("news",row[9])==0) prgtype=0;
                        else if (strcmp("Romance",row[9])==0) prgtype=0;
                        else if (strcmp("Sci-Fi",row[9])==0) prgtype=0;
                        else if (strcmp("Action",row[9])==0) prgtype=0;
                        else if (strcmp("Animation",row[9])==0) prgtype=0;
                        else prgtype=0;
                      }
                      break;
                    case 8:
                      // tv_grab_eu_dotmedia dk ver
                      if (strcmp(row[8],"None")!=0) {
                        if (strcmp("series",row[9])==0) prgtype=1;                // series
                        else if (strcmp("movie",row[9])==0) prgtype=5;            //
                        else if (strcmp("Action",row[9])==0) prgtype=5;           //
                        else if (strcmp("Drama",row[9])==0) prgtype=5;            //
                        else if (strcmp("Crime",row[9])==0) prgtype=5;            // Action film
                        else if (strcmp("Mystery",row[9])==0) prgtype=5;          //
                        else if (strcmp("sports",row[9])==0) prgtype=4;           //
                        else if (strcmp("Sports",row[9])==0) prgtype=8;           // Science fiction
                        else if (strcmp("Skiing",row[9])==0) prgtype=0;           //
                        else if (strcmp("Soccer",row[9])==0) prgtype=4;           //
                        else if (strcmp("Fencing",row[9])==0) prgtype=3;          // kids
                        else if (strcmp("tvshow",row[9])==0) prgtype=0;           //
                        else if (strcmp("Comedy",row[9])==0) prgtype=0;           // Entertainment
                        else if (strcmp("Adventure",row[9])==0) prgtype=4;        // news
                        else if (strcmp("Family",row[9])==0) prgtype=1;           // series
                        else if (strcmp("Fantasy",row[9])==0) prgtype=0;          //
                        else if (strcmp("Series",row[9])==0) prgtype=0;           //
                        else if (strcmp("Reality",row[9])==0) prgtype=0;          //
                        else if (strcmp("Miniseries",row[9])==0) prgtype=0;       //
                        else if (strcmp("TV Movie",row[9])==0) prgtype=0;         //
                        else if (strcmp("Teleshopping",row[9])==0) prgtype=0;     //
                        else if (strcmp("tvshow",row[9])==0) prgtype=0;           //
                        else if (strcmp("Sitcom",row[9])==0) prgtype=0;           //
                        else if (strcmp("Documentary",row[9])==0) prgtype=0;      //
                        else if (strcmp("Magazine",row[9])==0) prgtype=0;         //
                        else if (strcmp("news",row[9])==0) prgtype=4;             // news
                        else if (strcmp("Romance",row[9])==0) prgtype=1;          //
                        else if (strcmp("Sci-Fi",row[9])==0) prgtype=5;           //
                        else if (strcmp("Action",row[9])==0) prgtype=5;           //
                        else if (strcmp("Animation",row[9])==0) prgtype=0;        //
                        else prgtype=0;
                      }
                      break;
                    case 10:
                      // tv_grab_pt_meo
                      if (strcmp(row[8],"None")!=0) {
                        if (strcmp("series",row[9])==0) prgtype=1;                          // series
                        else if (strcmp("movie",row[9])==0) prgtype=5;                       //
                        else prgtype=0;
                      }
                      break;
                    default:
                      prgtype=0;
                  }
                } else prgtype=0;
                recorded=tvprgrecorded(row[1],row[3],row[11]);			              // get recorded status from backend
                if (prgnr<maxprogram_antal-1) tvkanaler[kanalnr].tv_prog_guide[prgnr].putprograminfo(row[3],row[1],row[2],row[5],row[6],row[7],row[10],row[4],prgtype,recorded);
                prgnr++;
                totalantalprogrammer++;
                if ((strcmp(tmptxt,row[0])!=0) || (prgnr>=maxprogram_antal-1)) {
                    printf(" Total programs loaded in channel %d \n",prgnr);
                    // if new channel id
                    tvkanaler[kanalnr].set_program_antal(prgnr-1);
                    huskprgantal=prgnr-1;
                    prgnr=0;
                    kanalnr++;								// next tv channel
                }
            }
            // set last channel # of programs in array
            tvkanaler[kanalnr].set_program_antal(huskprgantal);
            // total nr of channels
            this->kanal_antal=kanalnr+1;
            printf("Found nr of tv channels %4d\nFound nr of programs    %4d\n",this->kanal_antal,totalantalprogrammer);
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
    glBindTexture(GL_TEXTURE_2D, _tvbar3);
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
// return prg number from findtime (hours only) on channel selectchanel where progran start time >= findtime
//
/*
int tv_oversigt::find_start_kl_returnpointinarray(int selectchanel,int findtime) {
  int prg_nr=0;
  time_t rawtime;
  struct tm *timelist;
  struct tm mytimelist;
  time(&rawtime);                                                               // get time
  timelist=localtime(&rawtime);                                                 // convert to localtime
  mytimelist.tm_hour=findtime;
  mytimelist.tm_min=0;
  mytimelist.tm_mon=timelist->tm_mon;
  mytimelist.tm_sec=timelist->tm_sec;
  mytimelist.tm_year=timelist->tm_year;
  mytimelist.tm_mday=timelist->tm_mday;
  mytimelist.tm_yday=timelist->tm_yday;
  mytimelist.tm_isdst=timelist->tm_isdst;
  mktime(&mytimelist);
  time_t tt=mktime(&mytimelist);
  while((tvkanaler[selectchanel].tv_prog_guide[prg_nr].starttime_unix<tt) && (prg_nr<tvkanaler[selectchanel].program_antal())) {
    prg_nr++;
  }
  return(prg_nr);
}
*/

// find start time to change the start pos in array depaint on that the time is now

int tv_oversigt::find_start_pointinarray(int selectchanel) {
  struct tm mytimelist;
  struct tm *timelist;
  time_t rawtime;
  int prg_nr=0;
  // hent ur
  time(&rawtime);
  timelist=localtime(&rawtime);
  mytimelist.tm_hour=timelist->tm_hour;
  mytimelist.tm_min=0;
  mytimelist.tm_mon=timelist->tm_mon;
  mytimelist.tm_sec=timelist->tm_sec;
  mytimelist.tm_year=timelist->tm_year;
  mytimelist.tm_mday=timelist->tm_mday;
  mytimelist.tm_yday=timelist->tm_yday;
  mytimelist.tm_isdst=timelist->tm_isdst;
  mktime(&mytimelist);
  time_t tt=mktime(&mytimelist);
  while((tvkanaler[selectchanel].tv_prog_guide[prg_nr].starttime_unix<tt) && (prg_nr<tvkanaler[selectchanel].program_antal())) {
    prg_nr++;
  }
  return(prg_nr);
}

unsigned long tv_oversigt::getprogram_endunixtume(int selectchanel,int selectprg) {
  if (selectchanel<=tv_kanal_antal()) return(tvkanaler[selectchanel].tv_prog_guide[selectprg].endtime_unix);
}

unsigned long tv_oversigt::getprogram_startunixtume(int selectchanel,int selectprg) {
  if (selectchanel<=tv_kanal_antal()) return(tvkanaler[selectchanel].tv_prog_guide[selectprg].starttime_unix);
}


// return pointer to prgname in tvguide

char * tv_oversigt::getprogram_prgname(int selectchanel,int selectprg) {
  return(tvkanaler[selectchanel].tv_prog_guide[selectprg].program_navn);
}




// vis_tv_oversigt
// new
// den som bruges


void tv_oversigt::show_fasttv_oversigt(int selectchanel,int selectprg,bool do_update_xmltv_show) {

  float now_text_color[3]={1.0f,1.0f,0.5f};
  float now_text_clock_color[3]={1.0f,1.0f,0.0f};
  float catalog_text_color[3]={1.0f,1.0f,1.0f};
  float catalog_text_clock_color[3]={0.5f,1.0f,0.0f};
  struct tm *timeinfo;
  struct tm nowtime_h;
  time_t nutid;
  time_t nutidtime;
  time_t rawtime;
  time_t prgtidunix;                                                            // used to calc new length if now rom for it
  struct tm *timelist;
  struct tm mytimelist;
  struct tm *prgtime;
  float selectcolor=1.0f;

  unsigned int kanalomgang=100;                                                 //
  unsigned int kanalomgangofset=100;
  int vis_kanal_antal;

  int n;
  int chanid;
  int kanalnr=0;
  int cstartofset=0;
  int xpos,ypos;
  int xsiz,ysiz;
  int prglength=0;
  int barsize=0;
  float textsize1=18.0f;
  float textsize2=16.0f;
  int starttimeinmin,starttimeintim;
  int yypos=0;
  int prg_nr=0;
  int startyofset;
  int CHANELS_PR_LINE=7;
  char tmptxt[1024];
  char tmptim[1024];
  char tmpmin[1024];
  char tmptxt1[1024];
  time_t prgstarttid,prgendtid;
  starttid=time( NULL );
  nutid=starttid;
  timeinfo=localtime(&starttid);
  switch(screen_size) {
    case 4: CHANELS_PR_LINE=5;
            break;
    default:CHANELS_PR_LINE=7;
            break;
  }
  if (loading_tv_guide) {
    // show loading tv guide
    xsiz=450;
    ysiz=100;
    xpos=(orgwinsizex/2)-xsiz/2;
    ypos=(orgwinsizey/2)-ysiz/2;
    glPushMatrix();
    glTranslatef(10,50, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D,_tvbar3);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
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
  if (selectchanel>(CHANELS_PR_LINE-1)) cstartofset=selectchanel-(CHANELS_PR_LINE-1);
  else cstartofset=0;
  xpos=20;
  ypos=orgwinsizey-200;
  xsiz=(orgwinsizex-50);
  ysiz=150;
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
  aktivfont.selectfont((char *) "Norasi");
  chanid=tvkanaler[1].chanid;
  strcpy(tmptxt,tvkanaler[1].chanel_name);
//  glScalef(70.0, 70.0, 1.0);
//  glcRenderString("TEST");
  glBegin(GL_QUADS); //Begin quadrilateral coordinates
  glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
  glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
  glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
  glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
  glEnd(); //End quadrilateral coordinates
  glScalef(40.0, 40.0, 1.0);
  //glcRenderString(tvkanaler[1].chanel_name);
  glPopMatrix();

  // big top overskrift (tvguide .......)
  glPushMatrix();
  glColor3f(1.0f, 1.0f, 1.0f);
  switch (screen_size) {
    case 4: glTranslatef(xpos+320,orgwinsizey-80, 0.0f);
            break;
    default:glTranslatef(xpos+560,orgwinsizey-80, 0.0f);
            break;
  }
  glScalef(40.0, 40.0,1);
  glDisable(GL_TEXTURE_2D);

  //
  // show time bar in left side
  //
  // hent ur
  time(&rawtime);
  // convert clovk to localtime
  timelist=localtime(&rawtime);
  // vis nu eller kl viskl ?
  //if (debugmode) printf("viskl=%d \n ",viskl);
  //if (viskl==0) mytimelist.tm_hour=timelist->tm_hour; else mytimelist.tm_hour=viskl;
  if (vistvguidekl>24) vistvguidekl=0;
  mytimelist.tm_hour=vistvguidekl;
  mytimelist.tm_min=0;
  mytimelist.tm_mon=timelist->tm_mon;
  mytimelist.tm_sec=timelist->tm_sec;
  mytimelist.tm_year=timelist->tm_year;
  mytimelist.tm_mday=timelist->tm_mday;
  mytimelist.tm_yday=timelist->tm_yday;
  mytimelist.tm_isdst=timelist->tm_isdst;
  //show do_update_xmltv_show

  // check if we get tvguide and show it
  //if ((check_tvguide_process_running((char *) aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr])==false)) do_update_xmltv_show=false;
  switch (configland) {
            // english
    case 0: if (!(do_update_xmltv_show)) sprintf(tmptxt,"TV Guiden %s %02d-%02d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900); else
              if (mytimelist.tm_sec & 2) sprintf(tmptxt,"TV Guiden %s %02d-%02d-%d Updating.",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
              else sprintf(tmptxt,"TV Guiden %s %02d-%02d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
            break;
            // dansk
    case 1: if (!(do_update_xmltv_show)) sprintf(tmptxt,"TV Guiden %s den %02d-%02d-%d %02d:%02d",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900,timelist->tm_hour,timelist->tm_min); else
              if (mytimelist.tm_sec & 2) sprintf(tmptxt,"TV Guiden %s den %02d-%02d-%d %02d:%02d Updating.",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900,timelist->tm_hour,timelist->tm_min);
              else sprintf(tmptxt,"TV Guiden %s den %02d-%02d-%d %02d:%02d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900,timelist->tm_hour,timelist->tm_min);
            break;
    case 2: if (!(do_update_xmltv_show)) sprintf(tmptxt,"TV Guide %s %02d-%02d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900); else
            break;
    case 3: if (!(do_update_xmltv_show)) sprintf(tmptxt,"TV Guide %02s %d-%02d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900); else
              if (mytimelist.tm_sec & 2) sprintf(tmptxt,"TV Guide %02s %d-%02d-%d Updating.",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
              else sprintf(tmptxt,"TV Guide %02s %d-%02d-%d",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
            break;
    case 4: if (!(do_update_xmltv_show)) sprintf(tmptxt,"دليل التلفزيون %s %d-%d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900); else
              if (mytimelist.tm_sec & 2) sprintf(tmptxt,"دليل التلفزيون %s %d-%d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
              else sprintf(tmptxt,"دليل التلفزيون %s %d-%d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
            break;
    default:if (!(do_update_xmltv_show)) sprintf(tmptxt,"TV Guide %s %02d-%02d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900); else
              if (mytimelist.tm_sec & 2) sprintf(tmptxt,"TV Guide %s %02d-%02d-%d  Updating.",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
              else sprintf(tmptxt,"TV Guide %s %02d-%02d-%d",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
            break;
  };
  glcRenderString(tmptxt);
  glPopMatrix();

  // show time line
  n=0;
  while (n<8) {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    switch (screen_size) {
      case 4: glTranslatef(xpos+10,orgwinsizey-230-(n*150), 0.0f);
              break;
      default:
              glTranslatef(xpos+10,(orgwinsizey-230)-(n*150), 0.0f);                      // glTranslatef(xpos+10,(orgwinsizey-230)-(n*300), 0.0f);
              break;
    }
    //glTranslatef(xpos+10,(orgwinsizey-230)-(n*150), 0.0f);                      // glTranslatef(xpos+10,(orgwinsizey-230)-(n*300), 0.0f);
    glScalef(20.0, 20.0,1);
    glDisable(GL_TEXTURE_2D);
    sprintf(tmptxt,"%02d:%02d",mytimelist.tm_hour,mytimelist.tm_min);
    glcRenderString(tmptxt);
    glPopMatrix();
    n++;
    //mytimelist.tm_hour++;
    if ((n==1) || (n==3) || (n==5)) mytimelist.tm_min=30; else {
      mytimelist.tm_hour++;
      mytimelist.tm_min=0;
    }
    mktime(&mytimelist);
  }

  // reset to today after show time line
  //if (viskl==0) mytimelist.tm_hour=timelist->tm_hour; else mytimelist.tm_hour=viskl;
  mytimelist.tm_hour=vistvguidekl;
  mytimelist.tm_min=0;
  mytimelist.tm_mon=timelist->tm_mon;
  mytimelist.tm_sec=timelist->tm_sec;
  mytimelist.tm_year=timelist->tm_year;
  mytimelist.tm_mday=timelist->tm_mday;
  mytimelist.tm_yday=timelist->tm_yday;
  mytimelist.tm_isdst=timelist->tm_isdst;
  mktime(&mytimelist);

  mytimelist.tm_hour=timelist->tm_hour;
  //if (viskl>0) mytimelist.tm_hour=viskl;                                      // timelist->tm_hour;
  mytimelist.tm_hour=vistvguidekl;                                              // timelist->tm_hour;

  kanalnr=0+cstartofset;

  // hent tidspunk nu
  nutid=mktime(&nowtime_h);

  xpos=50+40;
  int do_kanal_nr=0;
  switch (screen_size) {
    case 4: vis_kanal_antal=5;                        // antal kanler som vises
            break;
    default:vis_kanal_antal=7;                        // antal kanler som vises
            break;
  }

  if (kanal_antal<vis_kanal_antal) vis_kanal_antal=kanal_antal;

  //
  // loop for channel
  //

  while ((xpos<orgwinsizex) && (do_kanal_nr<vis_kanal_antal)) {
    startyofset=0;
    glPushMatrix();
    switch (screen_size) {
      case 4: glTranslatef(xpos+11,orgwinsizey-210, 0.0f);
              break;
      default:glTranslatef(xpos+11,860, 0.0f);
              break;
    }
    glScalef(24.0, 24.0, 1.0);
    if (selectchanel==kanalnr) glColor3f(selectcolor,selectcolor,selectcolor); else glColor3f(0.6f, 0.6f, 0.6f);
    chanid=tvkanaler[0].chanid;
    strcpy(tmptxt," ");
    strcat(tmptxt,tvkanaler[kanalnr].chanel_name);
    *(tmptim+14)='\0';
    glcRenderString(tmptxt);
    glPopMatrix();

    int xsiz=210;
    int ysiz=110;

    prg_nr=0;
    barsize=0;
    yypos=0;

    //if (debugmode) printf("Omgang %d \n",do_kanal_nr);

    //printf("kanal nr %10d navn %40s \n",kanalnr,tvkanaler[kanalnr].chanel_name);

    // make time frame to show in sec
    time_t tt=mktime(&mytimelist)+(60*60*3);
    //
    // loop for program
    //

    while((tvkanaler[kanalnr].tv_prog_guide[prg_nr].starttime_unix<tt) && (prg_nr<=tvkanaler[kanalnr].program_antal())) {
      // start pos orgwinsizey-245
      //ypos=orgwinsizey-245-barsize;

      ypos=orgwinsizey-245;
      // hent prg length in min
      prgstarttid=tvkanaler[kanalnr].tv_prog_guide[prg_nr].starttime_unix;              // get time in unixtime
      prgendtid=tvkanaler[kanalnr].tv_prog_guide[prg_nr].endtime_unix;                  // get time in unixtime
      prglength=tvkanaler[kanalnr].tv_prog_guide[prg_nr].program_length_minuter;        // get time in unixtime
      // show program start before over view time start and end tine after view time start
      if ((prgstarttid<=mktime(&mytimelist)) && prgendtid>mktime(&mytimelist)) {
        // hent i minuter og lav det om til pixel (min * 5)
        // calc new length for ysiz
        prglength=20;                                                         // min prg length
        time_t prgtidunix=tvkanaler[kanalnr].tv_prog_guide[prg_nr].endtime_unix;
        prgtime=localtime(&prgtidunix);
        prglength=difftime(mktime(prgtime),mktime(&mytimelist))/60;
        //prglength=tvkanaler[kanalnr].tv_prog_guide[prg_nr].program_length_minuter;
        ysiz=prglength*5;
        glPushMatrix();
        //glTranslatef(xpos,820-yypos, 0.0f);
        glTranslatef(10,10, 0.0f);
//        glColor3f(0.5f,0.5f, 0.5f);		                                          // active program color
        if ((prgstarttid<=time(0)) && (prgendtid>=time(0))) {
          glColor3f(0.5f,0.5f, 0.5f);		    // active program color
        } else {
          switch(tvkanaler[kanalnr].tv_prog_guide[prg_nr].prg_type) {
            case 0:
              glColor3f(prgtypeRGB[0], prgtypeRGB[1], prgtypeRGB[2]);		         // none (default)
              break;
            case 1:
              glColor3f(prgtypeRGB[3], prgtypeRGB[4], prgtypeRGB[5]);      	    // serier
              break;
            case 2:
              glColor3f(prgtypeRGB[6], prgtypeRGB[7], prgtypeRGB[8]);      	    // div
              break;
            case 3:
              glColor3f(prgtypeRGB[9], prgtypeRGB[10], prgtypeRGB[11]);      	  // action
              break;
            case 4:
              glColor3f(prgtypeRGB[12], prgtypeRGB[13], prgtypeRGB[14]);      	// nyheder
              break;
            case 5:
              glColor3f(prgtypeRGB[15], prgtypeRGB[16], prgtypeRGB[17]);		    // komedier
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
              glColor3f(prgtypeRGB[27], prgtypeRGB[28], prgtypeRGB[29]);		    // sifi
              break;
            case 10:
              glColor3f(prgtypeRGB[30], prgtypeRGB[31], prgtypeRGB[32]);		    // ukdentd
              break;
            case 11:
              glColor3f(prgtypeRGB[33], prgtypeRGB[34], prgtypeRGB[35]);		    // rejser
              break;
            case 12:
              glColor3f(prgtypeRGB[36], prgtypeRGB[37], prgtypeRGB[38]);		    //
              break;
            case 13:
              glColor3f(prgtypeRGB[39], prgtypeRGB[40], prgtypeRGB[41]);		    // ukendt
              break;
            case 14:
              glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		    // rejser
              break;
            default:
              glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		    // default
              break;
          }
        }
        if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor,selectcolor,selectcolor);
        //_textureutvbgmask
        if (_textureutvbgmask) {
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D,_textureutvbgmask);
          glLoadName(kanalomgang+prg_nr);                                       // set array pos 100 for each array
          glBegin(GL_QUADS);                                                    // box
        } else {
          glLoadName(kanalomgang+prg_nr);                                       // set array pos 100 for each array
          glBegin(GL_LINE_LOOP);                                                // line
        }
        glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos-ysiz, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos-ysiz, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
        glEnd(); //End quadrilateral coordinates
        glPopMatrix();

        // show program stat + end tid hvis plads
        if (prglength>13) {
          // show start time
          glPushMatrix();
          glDisable(GL_TEXTURE_2D);
          strcpy(tmptxt,tvkanaler[kanalnr].tv_prog_guide[prg_nr].starttime+11);
          *(tmptxt+5)='\0';
          strcat(tmptxt," - ");
          strcat(tmptxt,tvkanaler[kanalnr].tv_prog_guide[prg_nr].endtime+11);
          *(tmptxt+13)='\0';
          glTranslatef(xpos+20,ypos-7, 0.0f);
          glScalef(textsize2, textsize2, 1.0);
          glColor3f(0.5f,0.5f, 0.5f);		// rejser
          if ((prgstarttid<=time(0)) && (prgendtid>=time(0))) glColor3f(now_text_clock_color[0],now_text_clock_color[1], now_text_clock_color[2]); else glColor3f(catalog_text_clock_color[0],catalog_text_clock_color[1], catalog_text_clock_color[2]);	    // active program color
          if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor,selectcolor,selectcolor);
          glcRenderString(tmptxt);
          glPopMatrix();
        }
        // show program name
        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        strcpy(tmptxt,tvkanaler[kanalnr].tv_prog_guide[prg_nr].program_navn);
        *(tmptxt+21)='\0';
        if (prglength>13) glTranslatef(xpos+20,ypos-22, 0.0f); else glTranslatef(xpos+20,ypos-7, 0.0f);
        glScalef(textsize1, textsize1, 1.0f);
        glColor3f(0.5f,0.5f, 0.5f);		                                          // active program color
        if ((prgstarttid<=time(0)) && (prgendtid>=time(0))) glColor3f(now_text_color[0],now_text_color[1], now_text_color[2]); else glColor3f(catalog_text_color[0],catalog_text_color[1], catalog_text_color[2]);    // active program color
        if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor,selectcolor,selectcolor);
        glcRenderString(tmptxt);                                              // print program name
        glPopMatrix();
      } else if (prgstarttid>=mktime(&mytimelist)) {
        // show program start after over view time start
        strncpy(tmpmin,tvkanaler[kanalnr].tv_prog_guide[prg_nr].starttime+14,2);
        strncpy(tmptim,tvkanaler[kanalnr].tv_prog_guide[prg_nr].starttime+11,2);
        *(tmpmin+3)='\0';
        starttimeinmin=atoi(tmpmin);
        starttimeintim=atoi(tmptim)-mytimelist.tm_hour;
        startyofset=((starttimeintim*60)+starttimeinmin);
        ypos=ypos-(startyofset)*5;
        // hent i minuter og lav det om til pixel (min * 5)
        ysiz=(prglength-1)*5;
        glPushMatrix();
        //glTranslatef(xpos,820-yypos, 0.0f);
        glTranslatef(10,10, 0.0f);

        if ((prgstarttid<=time(0)) && (prgendtid>=time(0))) {
          glColor3f(0.5f,0.5f, 0.5f);		    // active program color
        } else {
          switch(tvkanaler[kanalnr].tv_prog_guide[prg_nr].prg_type) {
            case 0:
              glColor3f(prgtypeRGB[0], prgtypeRGB[1], prgtypeRGB[2]);		         // none (default)
              break;
            case 1:
              glColor3f(prgtypeRGB[3], prgtypeRGB[4], prgtypeRGB[5]);      	    // serier
              break;
            case 2:
              glColor3f(prgtypeRGB[6], prgtypeRGB[7], prgtypeRGB[8]);      	    // div
              break;
            case 3:
              glColor3f(prgtypeRGB[9], prgtypeRGB[10], prgtypeRGB[11]);      	  // action
              break;
            case 4:
              glColor3f(prgtypeRGB[12], prgtypeRGB[13], prgtypeRGB[14]);      	// nyheder
              break;
            case 5:
              glColor3f(prgtypeRGB[15], prgtypeRGB[16], prgtypeRGB[17]);		    // komedier
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
              glColor3f(prgtypeRGB[27], prgtypeRGB[28], prgtypeRGB[29]);		    // sifi
              break;
            case 10:
              glColor3f(prgtypeRGB[30], prgtypeRGB[31], prgtypeRGB[32]);		    // ukdentd
              break;
            case 11:
              glColor3f(prgtypeRGB[33], prgtypeRGB[34], prgtypeRGB[35]);		    // rejser
              break;
            case 12:
              glColor3f(prgtypeRGB[36], prgtypeRGB[37], prgtypeRGB[38]);		    //
              break;
            case 13:
              glColor3f(prgtypeRGB[39], prgtypeRGB[40], prgtypeRGB[41]);		    // ukendt
              break;
            case 14:
              glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		    // rejser
              break;
            default:
              glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		    // default
              break;
          }
        }
        // if select program
        if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor,selectcolor,selectcolor);
        if (_textureutvbgmask) {
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D,_textureutvbgmask);
          glLoadName(kanalomgang+prg_nr);
          glBegin(GL_QUADS);                                                   // box
        } else {
          glLoadName(kanalomgang+prg_nr);
          glBegin(GL_LINE_LOOP);                                          // line
        }
        //glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos-ysiz, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos-ysiz, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
        glEnd(); //End quadrilateral coordinates
        glPopMatrix();

        if (prglength>10) {
          glPushMatrix();
          glDisable(GL_TEXTURE_2D);
          //glTranslatef(xpos,820-(yypos+18), 0.0f);
          glTranslatef(xpos+20,ypos-28, 0.0f);
          glScalef(textsize1, textsize1, 1.0);
          if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor,selectcolor,selectcolor);
          strcpy(tmptxt,tvkanaler[kanalnr].tv_prog_guide[prg_nr].program_navn);
          *(tmptxt+21)='\0';
          if ((prgstarttid<=time(0)) && (prgendtid>=time(0))) glColor3f(now_text_color[0],now_text_color[1], now_text_color[2]); else glColor3f(catalog_text_color[0],catalog_text_color[1], catalog_text_color[2]);	    // active program color
          glcRenderString(tmptxt);
          glPopMatrix();
          glPushMatrix();
          strcpy(tmptxt,tvkanaler[kanalnr].tv_prog_guide[prg_nr].starttime+11);
          *(tmptxt+5)='\0';
          strcat(tmptxt," - ");
          strcat(tmptxt,tvkanaler[kanalnr].tv_prog_guide[prg_nr].endtime+11);
          *(tmptxt+13)='\0';
          glDisable(GL_TEXTURE_2D);
          glTranslatef(xpos+20,ypos-8, 0.0f);
          glScalef(textsize2, textsize2, 1.0);
          if ((prgstarttid<=time(0)) && (prgendtid>=time(0))) glColor3f(now_text_clock_color[0],now_text_clock_color[1], now_text_clock_color[2]); else glColor3f(catalog_text_clock_color[0],catalog_text_clock_color[1], catalog_text_clock_color[2]);    // active program color
          if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor,selectcolor,selectcolor);
          glcRenderString(tmptxt);
          glPopMatrix();
        } else {
          glPushMatrix();
          strcpy(tmptxt,tvkanaler[kanalnr].tv_prog_guide[prg_nr].program_navn);
          *(tmptxt+21)='\0';
          glDisable(GL_TEXTURE_2D);
          glTranslatef(xpos+20,ypos-8, 0.0f);
          glScalef(textsize2, textsize2, 1.0);
          if ((prgstarttid<=time(0)) && (prgendtid>=time(0))) glColor3f(0.5f,0.5f, 0.5f);	else glColor3f(catalog_text_color[0],catalog_text_color[1], catalog_text_color[2]);   // active program color
          if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor,selectcolor,selectcolor);
          glcRenderString(tmptxt);
          glPopMatrix();
        }
        barsize=barsize+(prglength*5);
        yypos+=1;
      }
      prg_nr++;                                                                 // next program
    }

    kanalomgang+=kanalomgangofset;                                                         // next channel
    xpos+=220;
    kanalnr++;
    do_kanal_nr++;
  }



  // show clock line over tvguide
  //
  if (!(loading_tv_guide)) {
    time(&rawtime);
    timelist=localtime(&rawtime);
    if (timelist->tm_hour==mytimelist.tm_hour) {
      xpos=35;
      ypos=orgwinsizey-298;
      xsiz=(orgwinsizex-280);
      ysiz=2;
      float timelineofset=(timelist->tm_min*4.5);
      ypos-=timelineofset;

      glPushMatrix();
      glTranslatef(10,50, 0.0f);
      // top
      //glEnable(GL_TEXTURE_2D);
      //glBlendFunc(GL_ONE, GL_ONE);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D,_tvoverskrift);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glColor3f(0.8f, 0.8f, 0.8f);
      //  glScalef(70.0, 70.0, 1.0);
      //  glcRenderString("TEST");
      //glBegin(GL_QUADS); //Begin quadrilateral coordinates
      glBegin(GL_LINE_LOOP);                // line
      glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
      glEnd(); //End quadrilateral coordinates
      glScalef(40.0, 40.0, 1.0);
      //glcRenderString(tvkanaler[1].chanel_name);
      glPopMatrix();
    }
  }
}





//
// viser et prgrams record info.
// har store problemer med formatering (SKAL FIXES)

void tv_oversigt::showandsetprginfo(int kanalnr,int tvprgnr) {
    char tmptxt[200];
    char tmptxt1[17000];
    time_t aktueltid;
    time_t prgtid;
    time(&aktueltid);					// hent hvad klokken er
    struct tm *timeinfo;
    struct tm prgtidinfo;
    static int antalrec=-1;
    int xpos,ypos;
    int xsiz,ysiz;
    timeinfo=localtime(&aktueltid);				// convert to localtime
    // window size
    xsiz=850;
    ysiz=400;
    xpos=(orgwinsizex/2)-xsiz/2;
    ypos=(orgwinsizey/2)-ysiz/2;
    //glPushMatrix();
    glTranslatef(10.0f, 50.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D,_tvbar3);
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(xpos+225-(xsiz/2), ypos-(ysiz/2), 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(xpos+225-(xsiz/2), ypos+ysiz-(ysiz/2), 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xpos+225+xsiz-(xsiz/2), ypos+ysiz-(ysiz/2), 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xpos+225+xsiz-(xsiz/2), ypos-(ysiz/2), 0.0);
    glEnd(); //End quadrilateral coordinates
    // print
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(400,470, 0.0f);
    glScalef(20.0, 20.0,1);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_TEXTURE_2D);
    switch (configland) {
      case 0: sprintf(tmptxt,"Channel  : %-10s",tvkanaler[kanalnr].chanel_name);
              break;
      case 1: sprintf(tmptxt,"Kanal    : %-10s",tvkanaler[kanalnr].chanel_name);
              break;
      case 2: sprintf(tmptxt,"Channel  : %-10s",tvkanaler[kanalnr].chanel_name);
              break;
      case 3: sprintf(tmptxt,"Channel  : %-10s",tvkanaler[kanalnr].chanel_name);
              break;
      case 4: sprintf(tmptxt,"Channel  : %-10s",tvkanaler[kanalnr].chanel_name);
              break;
      default:
              sprintf(tmptxt,"Channel  : %-10s",tvkanaler[kanalnr].chanel_name);
    }
    glcRenderString(tmptxt);

    glTranslatef(-10.0f, -2.0f, 0.0f);
    switch (configland) {
      case 0: sprintf(tmptxt,"Prg name : %-20s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn);
              break;
      case 1: sprintf(tmptxt,"Prg navn : %-20s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn);
              break;
      case 2: sprintf(tmptxt,"Prg name : %-20s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn);
              break;
      case 3: sprintf(tmptxt,"Prg name : %-20s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn);
              break;
      case 4: sprintf(tmptxt,"Prg name : %-20s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn);
              break;
      default:
            sprintf(tmptxt,"Prg name : %-20s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn);
    }
    glcRenderString(tmptxt);

    //glLoadIdentity();
    //glTranslatef(400,470, 0.0f);
    glTranslatef(-15.0f, -2.0f, 0.0f);
    switch (configland) {
      case 0: sprintf(tmptxt,"Start    : %10s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime);
              break;
      case 1: sprintf(tmptxt,"Start    : %10s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime);
              break;
      case 2: sprintf(tmptxt,"début    : %10s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime);
              break;
      case 3: sprintf(tmptxt,"Start    : %10s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime);
              break;
      case 4: sprintf(tmptxt,"Start    : %10s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime);
              break;
      default:sprintf(tmptxt,"Start    : %10s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime);

    }
    glcRenderString(tmptxt);

//   *configlandsprog[]={(char *) "English",(char *) "Dansk",(char *) "France",(char *) "Tysk",(char *) "Arabic"};

    glTranslatef(-15.0f, -2.0f, 0.0f);
    switch (configland) {
      case 0: sprintf(tmptxt,"Length   : %4d min.",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_length_minuter);
              break;
      case 1: sprintf(tmptxt,"Længde   : %4d min.",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_length_minuter);
              break;
      case 2: sprintf(tmptxt,"durée du : %4d min.",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_length_minuter);
              break;
      case 3: sprintf(tmptxt,"Programmlänge : %4d min.",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_length_minuter);
              break;
      case 4: sprintf(tmptxt,"Length   : %4d min.",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_length_minuter);
              break;
      default:
              sprintf(tmptxt,"Length   : %4d min.",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_length_minuter);
    }
    glcRenderString(tmptxt);

    glTranslatef(-10.0f, -2.0f, 0.0f);
    if (tvkanaler[kanalnr].tv_prog_guide[tvprgnr].prg_type<=7)
        sprintf(tmptxt,"Type     : %-10s",prgtypee[tvkanaler[kanalnr].tv_prog_guide[tvprgnr].prg_type]);
    else strcpy(tmptxt,"Type     :           ");
    glcRenderString(tmptxt);

    glTranslatef(-10.0f, -2.0f, 0.0f);
    if (antalrec==-1) antalrec=tvprgrecordedbefore(tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn,tvkanaler[kanalnr].chanid);
    switch (configland) {
      case 0: sprintf(tmptxt,"recorded %d times before.",antalrec);
              break;
      case 1: sprintf(tmptxt,"optaget %d gange før.",antalrec);
              break;
      case 2: sprintf(tmptxt,"recorded %d times before.",antalrec);
              break;
      case 3: sprintf(tmptxt,"recorded %d times before.",antalrec);
              break;
      case 4: sprintf(tmptxt,"recorded %d times before.",antalrec);
              break;
      default: sprintf(tmptxt,"recorded %d times before.",antalrec);

    }
    if (tvkanaler[kanalnr].tv_prog_guide[tvprgnr].recorded) strcat(tmptxt," Set to record");
    glcRenderString(tmptxt);

    glTranslatef(-10.0f, -2.0f, 0.0f);
    switch (configland) {
      case 0: sprintf(tmptxt1,"Description : %s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].description);
              break;
      case 1: sprintf(tmptxt1,"Beskrivelse : %s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].description);
              break;
      case 2: sprintf(tmptxt1,"Description : %s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].description);
              break;
      case 3: sprintf(tmptxt1,"Description : %s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].description);
              break;
      case 4: sprintf(tmptxt1,"Description : %s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].description);
              break;
      default: sprintf(tmptxt1,"Description : %s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].description);
    }
    glcRenderString(tmptxt1);
    if (strptime(tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime,"%Y-%m-%d %H:%M:%S",&prgtidinfo)==NULL) {
        printf("RECORDED PROGRAM DATE FORMAT ERROR can't convert. by strptime\n");
    }
    prgtid=mktime(&prgtidinfo);
    if ((difftime(aktueltid,prgtid)<=0) && (tvprgrecorded(tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime,tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn,tmptxt)==0)) {
      xsiz=5;
      ysiz=5;
      xpos=((orgwinsizex/2)-xsiz/2)-200;
      ypos=((orgwinsizey/2)-ysiz/2)-200;

      glTranslatef(-10.0f, -2.0f, 0.0f);
      xpos=10.0f;
      ypos=-50.0f;
      glTranslatef(10.0f, 50.0f, 0.0f);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D,_tvrecordbutton);
      glEnable(GL_TEXTURE_2D);
      glLoadName(41);                                                           // func Set program to record.
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 0.0); glVertex3f(xpos-(xsiz/2), ypos-(ysiz/2), 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(xpos-(xsiz/2), ypos+ysiz-(ysiz/2), 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz-(xsiz/2), ypos+ysiz-(ysiz/2), 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz-(xsiz/2), ypos-(ysiz/2), 0.0);
      glEnd(); //End quadrilateral coordinates
      // close button
      xpos=15.0f;
      glBindTexture(GL_TEXTURE_2D,_textureclose);            // old _tvrecordcancelbutton
      glLoadName(40);                                        // func close window
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 0.0); glVertex3f(xpos-(xsiz/2), ypos-(ysiz/2), 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(xpos-(xsiz/2), ypos+ysiz-(ysiz/2), 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz-(xsiz/2), ypos+ysiz-(ysiz/2), 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz-(xsiz/2), ypos-(ysiz/2), 0.0);
      glEnd(); //End quadrilateral coordinates
    } else {
      // close button
      xsiz=5;
      ysiz=5;
      xpos=((orgwinsizex/2)-xsiz/2)-200;
      ypos=((orgwinsizey/2)-ysiz/2)-200;
      glTranslatef(-10.0f, -2.0f, 0.0f);
      xpos=15.0f;
      ypos=-50.0f;
      glTranslatef(10.0f, 50.0f, 0.0f);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,_textureclose);            // old _tvrecordcancelbutton
      glLoadName(40);                                        // func close window
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 0.0); glVertex3f(xpos-(xsiz/2), ypos-(ysiz/2), 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(xpos-(xsiz/2), ypos+ysiz-(ysiz/2), 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz-(xsiz/2), ypos+ysiz-(ysiz/2), 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz-(xsiz/2), ypos-(ysiz/2), 0.0);
      glEnd(); //End quadrilateral coordinates
    }
}







//
//*******************************************************************************************************************************//
//

//
// construktor
//


earlyrecorded::earlyrecorded() {
    for(int i=0;i<199;i++) {
        strcpy(this->programinfo[i].name,"ABC");
        strcpy(this->programinfo[i].dato,"2017-01-01");
        strcpy(this->programinfo[i].endtime,"");
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
  // background
  //glLoadIdentity();
  int i=0;
  glEnable(GL_TEXTURE);
  glEnable(GL_BLEND);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturemovieinfobox);                    //_texturemovieinfobox
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),100 , 0.0);
  glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
  glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
  glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,100 , 0.0);
  glEnd();
  glcRenderString(upcommingrec);
  glRasterPos2f(2.0f, 2.0f);
  glcRenderString("fdfdsfd");
  while((i<this->antal) && (i<50)) {
    glcRenderString(this->programinfo[i].name);
  }


/*

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
*/
    // End button 1
}
