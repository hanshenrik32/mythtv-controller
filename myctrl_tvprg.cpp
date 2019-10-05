#include <mysql.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <GL/glut.h>    // Header File For The GLUT Library
#include <GL/gl.h>      // Header File For The OpenGL32 Library
#include <GL/glu.h>     // Header File For The GLu32 Library
#include <GL/glx.h>     // Header file fot the glx libraries.
#include <GL/glc.h>     // danish ttf support
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <math.h>
#include <ctype.h>
#include <ical.h>
#include <libxml/parser.h>
#include "utility.h"
#include "myctrl_tvprg.h"
#include "myth_ttffont.h"
#include "myth_setup.h"
#include "myctrl_readwebfile.h"
#include "readjpg.h"

extern bool tv_guide_firsttime_update;
extern float configdefaulttvguidefontsize;                                     // font size in tvguide
extern GLuint setupnetworkwlanback;
extern bool ask_tv_record;
extern tv_graber_config aktiv_tv_graber;                                       // xmltv graber config
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
extern GLuint _tvoldprgrecordedbutton;
extern GLuint _tvnewprgrecordedbutton;
extern GLuint _tvoverskrift;
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

float prgtypeRGB[]={    0.7f,0.7f,0.7f,               // 0 - none
                        0.6f,0.6f,1.0f,               // 1 - children
                        0.0f,0.8f,0.0f,               // 2 - sport
                        0.6f,0.6f,0.8f,               // 3 - cartoons
                        0.5f,0.9f,0.0f,               // 4 - news
                        1.0f,0.4f,1.0f,               // 5 - movies
                        0.5f,0.9f,0.0f,               // 6 - natur
                        0.5f,0.8f,0.6f,               // 7 - Documentary
                        0.7f,0.1f,0.1f,               // 8 - Entertainment
                        1.0f,0.6f,0.0f,               // 9 - Sci-Fi
                        0.1f,0.2f,0.1f,               // 10 - Series
                        0.8f,0.2f,0.8f};              // 11 - Adult


// bruges ikke af show_tvoversigt

void myglprinttv(char *string) {
  int len,i;
  len = (int) strlen(string);
  for (i = 0; i < len; i++) {
    //glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
     glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
  }
}


//
// check if process is running
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


//
// intern function for _xmltv
// used to cmd/download the tvguide.xml file to xmlparser
// use 2 days for tv_grab_eu_dotmedia it is fast as hell

int get_tvguide_fromweb() {
  char exestring[2048];
  int result=-1;
  // check if active xml_tv graber is running
  if (check_tvguide_process_running((char *) aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr])==false) {
    strcpy(exestring,configbackend_tvgraber);
    if ((aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr],"tv_grab_eu_dotmedia")==0) strcat(exestring," --days 2 --output ~/tvguide.xml 2> ~/tvguide.log");
    else strcat(exestring," --days 2 --output ~/tvguide.xml 2> ~/tvguide.log");
    if (debugmode & 256) printf("Start tv graber background process %s\n command :%s\n",configbackend_tvgraber,exestring);
    result=system(exestring);   // do it
    if (debugmode & 256) printf("Done tv graber background process exit kode %d\n",result);
  } else printf("Graber is already ruuning.\n");
  return(result);
}


//
// hash func
//

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
  char *database = (char *) "mythtvcontroller";
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
  char *database = (char *) "mythtvcontroller";
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  unsigned long id=0;
  // mysql stuf
  conn=mysql_init(NULL);
  if (conn) {
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
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
  char *database = (char *) "mythtvcontroller";
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  unsigned long id=0;
  // mysql stuf
  conn=mysql_init(NULL);
  if (conn) {
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
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
//
//

int tv_oversigt::saveparsexmltvdb() {
  int n=0;
  FILE *dbfil;
  dbfil=fopen(tvguidedbfilename,"w");
  if (dbfil) {
    while(n<this->kanal_antal) {
      fwrite(&tvkanaler[n],sizeof(tv_oversigt_pr_kanal),1,dbfil);
      n++;
    }
    fclose(dbfil);
  }
}


//
//
//

int tv_oversigt::loadparsexmltvdb() {
  int n=0;
  FILE *dbfil;
  dbfil=fopen(tvguidedbfilename,"r");
  if (dbfil) {
    while(!(feof(dbfil))) {
      fread(&tvkanaler[n],sizeof(tv_oversigt_pr_kanal),1,dbfil);
      n++;
    }
    fclose(dbfil);
  }
}


//
// read tv guide from file. And update tvguide db (create if not exist)
// return >0 on error

int tv_oversigt::parsexmltv(const char *filename) {
  char *token;
  bool prgtype;
  xmlChar *content;
  xmlChar *content1;
  char userhomedir[200];
  char path[1024];
  char result[1024];
  unsigned int prg_antal=0;
  char temptxt[1024];
  char downloadfile[1024];
  char sql[32738];
  char *s;
  int error=0;
  bool cidfundet=false;
  xmlChar *tmpdat;
  xmlDoc *document;
  xmlNode *root, *first_child, *node, *node1 ,*subnode;
  xmlChar *xmltvid;
  xmlChar *xmltvicon_url;
  char iconfile[1024];
  char starttime[64];
  bool getstart=false;
  char endtime[64];
  bool getend=false;
  char prgtitle[1024];
  char channelname[1024];
  char channelidname[1024];
  char category[1924];
  char description[4096];
  char realfilename[1024];
  xmlChar *title;
  bool gettchannel=false;
  xmlChar *desc;
  unsigned long channelid;
  bool fundet=false;
  char *database = (char *) "mythtvcontroller";
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  struct stat t_stat;           // file info struct
  struct tm* lastmod;           //
  loading_tv_guide=true;        // set loadtv guide flag to show in show_tv_guide then xml files is passed
  // mysql stuf
  // Connect to database
  conn=mysql_init(NULL);
  if (conn) {
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
      strcpy(sql,"create table IF NOT EXISTS channel(chanid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,channum varchar(10),freqid varchar(10) ,sourceid int(10) unsigned,callsign varchar(20),name  varchar(64), icon varchar(255), finetune int(11) , videofilters varchar(255), xmltvid varchar(64), recpriority int(10), contrast int(11) DEFAULT 32768, brightness int(11) DEFAULT 32768, colour int(11) DEFAULT 32768, hue int(11) DEFAULT 32768, tvformat varchar(10), visible tinyint(1) DEFAULT 1, outputfilters varchar(255), useonairguide tinyint(1) DEFAULT 0, mplexid smallint(6), serviceid mediumint(8) unsigned, atsc_major_chan int(10) unsigned DEFAULT 0, atsc_minor_chan int(10) unsigned DEFAULT 0, last_record datetime, default_authority varchar(32), commmethod int(11) DEFAULT +1, iptvid smallint(6) unsigned,orderid int(12) unsigned DEFAULT 0,iconfile varchar(200))");
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
  }
  if (conn) {
    if (stat("images/tv_icons/", &t_stat)!=0) {
      mkdir("images/tv_icons",0777);
    }
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
                if (strcmp((char *) subnode->name,"icon")==0) {
                  // get icon source http link from xmlfile
                  xmltvicon_url=xmlGetProp(subnode,( xmlChar *) "src");
                  if (xmltvicon_url) {
                    strcpy(iconfile,(char *) xmltvicon_url);
                    // get file name from url (realfilename)
                    get_webfilename(realfilename,iconfile);
                    // set downloadfile path
                    strcpy(downloadfile,"images/tv_icons/");
                    strcat(downloadfile,realfilename);
                    // download tv channel iconfile
                    // if not exist in images/tv_icons directory
                    if (!(file_exists(downloadfile))) {
                      get_webfile(iconfile,downloadfile);
                    }
                  }
                }
                subnode=subnode->next;
              }
              cidfundet=do_cannel_exist(channelidname);
              if (cidfundet==0) {
                sprintf(sql,"insert into channel (chanid,callsign,name,xmltvid,iconfile) values(0,'%s','%s','%s','%s')",channelidname,s,xmltvid,realfilename);
                mysql_query(conn,sql);
                res = mysql_store_result(conn);
              }
            }
            // create tv programs in guide from xmltag programme
            if (strcmp((char *) node->name,"programme")==0) {
              content = xmlNodeGetContent(node);
              if (content) {
                strcpy(category,"");
                prgtype=false;
                strcpy(result,(char *) content);
                s=trimwhitespace(result);
                getfirstlinefromstring(prgtitle,s);
                token=strtok((char *) content,"\n");
                while(token) {
                  switch (aktiv_tv_graber.graberaktivnr) {
                    // none
                    case 0:
                      break;
                    // 1 tv_grab_na_dd (username/pass required)
                    // can auto config by --configure
                    case 1:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    // 2 tv_grab_nl Grab TV listings for Netherlands
                    // can auto config by --configure
                    case 2:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    // 3 tv_grab_es_laguiatv TV listings for Spain
                    // can auto config by --configure
                    // graber have no prg type in xml file so
                    case 3:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_il Grab TV listings for Israel
                    //
                    // DO NOT WORK
                    case 4:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentales",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Realidad",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Aventuras",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Acción",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Deportes",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Infantiles",(char *) token)==0)) {
                        strcpy(category,"children");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Series",(char *) token)==0)) {
                        strcpy(category,"Serie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Venta y Publicidad",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentales",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Noticias",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Películas",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Culturales",(char *) token)==0)) {
                        strcpy(category,"Cultural");   // kultur programmer
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Musicales",(char *) token)==0)) {
                        strcpy(category,"Music");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Series",(char *) token)==0)) {
                        strcpy(category,"Serie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Variedades",(char *) token)==0)) {
                        strcpy(category,"Other");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    // tv_grab_na_tvmedia
                    // account at API Key found on your account dashboard page (https://www.xmltvlistings.com/account/)
                    //
                    case 5:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    // tv_grab_dtv_la Grab TV listings for Direct TV Latin America
                    // can not test from denmark Access Denied
                    case 6:
                    // tv_grab_fi Grab TV listings for Finland
                    //
                    case 7:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentales",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Culturales",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    urheilu",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    elokuvat",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Infantiles",(char *) token)==0)) {
                        strcpy(category,"children");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    // tv_grab_eu_dotmedia Europe ver OK
                    // need config by --gui
                    // country's avable in xmlgraber is this list
                    //
                    // Austria,coratia
                    // Denmark,Finland
                    // germany,Gungary
                    // Latvia, Lithuania
                    // netherlands, Norway
                    // Poland, Sweden
                    // Switzerland
                    // all radio stations
                    // All tv channels
                    case 8:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_se_swedb sweden
                    //
                    case 9:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    //  tv_grab_pt_meo for Portugal
                    //
                    case 10:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_fr for france
                    //
                    case 11:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_uk_bleb uk
                    //
                    case 12:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_huro Hungary or Romania ok
                    //
                    case 13:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Talk",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Children",(char *) token)==0)) {
                        strcpy(category,"Children");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Educational",(char *) token)==0)) {
                        strcpy(category,"Children");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_ch_search ok
                    //
                    case 14:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    gemischte Themen",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Kriminalfilm",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sport",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Dokumentation",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Menschen",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nachrichten",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Unterhaltung",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Show",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Film",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Krimireihe",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Krimi",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Infomagazin",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    TV-Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_it ok
                    //
                    case 15:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    motori",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    atletica",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    ciclismo",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    SPORT",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    mondo e tendenze",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    fiction",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    cinema",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    film",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    documentario",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    intrattenimento",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    natura",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    animazione",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_is OK
                    //
                    case 16:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    children",(char *) token)==0)) {
                        strcpy(category,"children");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_fi_sv
                    //
                    case 17:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_na_dtv
                    //
                    case 18:
                    if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                      strcpy(category,"Series");
  //                    prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                      strcpy(category,"Fantasy");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                      strcpy(category,"Dokumentar");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                      strcpy(category,"Reality");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                      strcpy(category,"Adventure");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                      strcpy(category,"Action");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                      strcpy(category,"Thriller");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                      strcpy(category,"Romance");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                      strcpy(category,"Mystery");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                      strcpy(category,"Sport");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                      strcpy(category,"Sport");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                      strcpy(category,"Comedy");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                      strcpy(category,"Tvshow");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                      strcpy(category,"War");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                      strcpy(category,"Documentary");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                      strcpy(category,"Lifestyle");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                      strcpy(category,"News");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                      strcpy(category,"Entertainment");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                      strcpy(category,"Movie");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                      strcpy(category,"Nature");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                      strcpy(category,"Gameshow");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                      strcpy(category,"Animation");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                      strcpy(category,"Animation");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                      strcpy(category,"Drama");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                      strcpy(category,"Adult");
                      prgtype=true;
                    } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                      // description
                      prgtype=true;
                    }
                    break;
                    //
                    // tv_grab_tr Grab TV listings for Turkey
                    //
                    case 19:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Diğer",(char *) token)==0)) {
                        strcpy(category,"Other");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Korku",(char *) token)==0)) {
                        strcpy(category,"Thriller");  // or Fear
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Yarışma",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Çocuk",(char *) token)==0)) {
                        strcpy(category,"children");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Haberler",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Polisiye",(char *) token)==0)) {
                        strcpy(category,"Crime");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    film",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Bilim Kurgu",(char *) token)==0)) {
                        strcpy(category,"Sci-Fi");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Magazin",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Aile",(char *) token)==0)) {
                        strcpy(category,"Family");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Dram",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Eğlence",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_eu_egon TV listings for German
                    //
                    case 20:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_dk_dr TV listings for DK
                    //
                    case 22:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sport",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Dokumentar",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Livsstil",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nyheder &amp; aktualitet",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Underholdning",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Film",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_se_tvzon TV listings for Sweden OK
                    //
                    case 23:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    movie",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Horror",(char *) token)==0)) {
                        strcpy(category,"Horror");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    series",(char *) token)==0)) {
                        strcpy(category,"series");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    //
                    // tv_grab_ar TV listings for Argentina OK
                    //
                    case 24:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Deportes",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Infantiles",(char *) token)==0)) {
                        strcpy(category,"Children");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Venta y Publicidad",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentales",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Periodístico",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Noticias",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Cine",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Series",(char *) token)==0)) {
                        strcpy(category,"Series");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    // 25 tv_grab_fr_kazer
                    case 25:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    // 26 tv_grab_uk_tvguide
                    case 26:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    // 27 tv_grab_zz_sdjson
                    case 27:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                    // > 27
                    default:
                      if ((prgtype==false) && (strcmp("series",(char *) token)==0)) {
                        strcpy(category,"Series");
    //                    prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Fantasy",(char *) token)==0)) {
                        strcpy(category,"Fantasy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sci-Fi",(char *) token)==0)) {
                        strcpy(category,"Dokumentar");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Reality",(char *) token)==0)) {
                        strcpy(category,"Reality");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adventure",(char *) token)==0)) {
                        strcpy(category,"Adventure");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Action",(char *) token)==0)) {
                        strcpy(category,"Action");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Thriller",(char *) token)==0)) {
                        strcpy(category,"Thriller");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Romance",(char *) token)==0)) {
                        strcpy(category,"Romance");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Mystery",(char *) token)==0)) {
                        strcpy(category,"Mystery");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    sports",(char *) token)==0)) {
                        strcpy(category,"Sport");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Comedy",(char *) token)==0)) {
                        strcpy(category,"Comedy");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    tvshow",(char *) token)==0)) {
                        strcpy(category,"Tvshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    War",(char *) token)==0)) {
                        strcpy(category,"War");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Documentary",(char *) token)==0)) {
                        strcpy(category,"Documentary");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Lifestyle",(char *) token)==0)) {
                        strcpy(category,"Lifestyle");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    News",(char *) token)==0)) {
                        strcpy(category,"News");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Entertainment",(char *) token)==0)) {
                        strcpy(category,"Entertainment");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Movies",(char *) token)==0)) {
                        strcpy(category,"Movie");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Nature",(char *) token)==0)) {
                        strcpy(category,"Nature");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Gameshow",(char *) token)==0)) {
                        strcpy(category,"Gameshow");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Animation",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Family",(char *) token)==0)) {
                        strcpy(category,"Animation");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Drama",(char *) token)==0)) {
                        strcpy(category,"Drama");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("    Adult",(char *) token)==0)) {
                        strcpy(category,"Adult");
                        prgtype=true;
                      } else if ((prgtype==false) && (strcmp("  ",(char *) token)==0)) {
                        // description
                        prgtype=true;
                      }
                      break;
                  }
                  token=strtok(NULL,"'\n");
                }
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
//                if (debugmode & 256) fprintf(stdout,"#%4d of Tvguide records created.... Channel %20s %s->%s %s \n",prg_antal,channelname,starttime,endtime,prgtitle);
              } else {
//                if (debugmode & 256) fprintf(stdout,"Tvguide Program exist Channel......         %20s %s->%s %s \n",channelname,starttime,endtime,prgtitle);
              }
            }
            // save rec
          }
        } // for loop end
        fprintf(stdout, "...\n");
        xmlFreeDoc(document);
      } else {
        if (debugmode & 256) printf("tvguide.xml not found \n");
      }

    }
  }
  loading_tv_guide=false;
  mysql_close(conn);
  return(error);
}


//
// clear tv guide (drop db)
//

void tv_oversigt::cleartvguide() {
  char sql[4096];
  int error=0;
  bool fundet=false;
  char *database = (char *) "mythtvcontroller";
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
    settorecord=false;
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

// constructor tv_oversigt class

tv_oversigt::tv_oversigt() {
    time_t rawtime;
    struct tm *timelist;
    kanal_antal=0;
    strcpy(mysqllhost,"");
    strcpy(mysqlluser,"");
    strcpy(mysqllpass,"");
    strcpy(loadinginfotxt,"");
    lastupdated=0;
    vistvguidecolors=true;
    vis_kanal_antal=8;                                                        // nr of channels to display
    // get time now
    time(&rawtime);
    // convert clovk to localtime
    timelist=localtime(&rawtime);
    vistvguidekl=timelist->tm_hour;
    if (vistvguidekl>24) vistvguidekl=0;
}


// destructor tv_oversigt class

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
    char *database = (char *) "mythtvcontroller";
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
    char *database = (char *) "mythtvcontroller";
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
    char *database = (char *) "mythtvcontroller";
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
    char *database = (char *) "mythtvcontroller";
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
          // sprintf(sqlselect,"INSERT INTO record values (0,1,%s,'12:00:00','2018-01-02','12:00:00','2018-01-02',\"%s\",\"%s\",\"%s\",11,12,\"%s\",'pro',15,16,17,18,19,20,'Default',22,23,'station','serid','prgid','intref',28,29,30,31,32,33,34,35,36,'12:00:12',38,39,40,41,'playgroup',43,'2017-01-02 12:00:00','2017-01-02 12:00:00','2017-01-02 12:00:00','storegrp',48,49)",row[6],tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn, tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].sub_title,tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].description,row[8]);
          sprintf(sqlselect,"INSERT INTO record values (0,1,%s,'12:00:00','2018-01-02','12:00:00','2018-01-02',\"%s\",\"%s\",\"%s\",11,12,\"%s\",'pro',15,16,17,18,19,20,'Default',22,23,'station','serid','prgid','intref',28,29,30,31,32,33,34,35,36,'12:00:12',38,39,40,41,'playgroup',43,'2017-01-02 12:00:00','2017-01-02 12:00:00','2017-01-02 12:00:00','storegrp',48,49)",row[6],tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn, tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].sub_title,tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].description,row[8]);
          if (debugmode & 256) printf("sql record is %s\n",sqlselect);
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



// load kanal icons

void tv_oversigt::opdatere_tv_oversigt_kanal_icons() {
  char file[2048];
  char tmp[2048];
  GLuint icon=0;
  for (int i=0;i<kanal_antal;i++) {
    if (!(tvkanaler[i].get_kanal_icon())) {
      strcpy(file,"images/tv_icons/");
      tvkanaler[i].get_kanal_icon_file(tmp);
      strcat(file,tmp);
//      icon=loadTexture(file);
      tvkanaler[i].set_kanal_icon(icon);
    }
  }
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
    char *database = (char *) "mythtvcontroller";
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
    sprintf(dagsdato,"%04d-%02d-%02d 00:00:00",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday);
    sprintf(enddate,"%04d-%02d-%02d 23:59:59",timeinfo3.tm_year+1900,timeinfo3.tm_mon+1,timeinfo3.tm_mday);
    //strftime(dagsdato, 128, "%Y-%m-%d 00:00:00", timeinfo);		        // lav nu tids sting strftime(dagsdato, 128, "%Y-%m-%d %H:%M:%S", timeinfo );
    //strftime(enddate, 128, "%Y-%m-%d 23:59:59", timeinfo2);		        // lav nu tids sting
    this->starttid=rawtime;						                                // gem tider i class
    this->sluttid=rawtime2;						                                //
    if (debugmode & 256) printf("\nGet/update Tvguide.\n");
    if (debugmode & 256) printf("Tvguide from %-19s to %-19s \n",dagsdato,enddate);
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
            if (debugmode & 256) printf("Antal channels/tvguide %s \n",row[0]);
          }
        }
        // do select from db
        strcpy(sqlselect,"SELECT channel.name,channel.iconfile,program.starttime,program.endtime,title,subtitle,TIMESTAMPDIFF(MINUTE,starttime,endtime),UNIX_TIMESTAMP(program.starttime),UNIX_TIMESTAMP(program.endtime),category,category_type,description,program.chanid FROM program left join channel on program.chanid=channel.chanid where channel.visible=1 and endtime<='");
        strcat(sqlselect,enddate);
        strcat(sqlselect,"' and starttime>='");
        strcat(sqlselect,dagsdato);
        strcat(sqlselect,"' order by orderid,chanid,abs(channel.channum),starttime");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        kanalnr=0;
        prgnr=0;
        if (res) {
            while (((row = mysql_fetch_row(res)) != NULL) && (prgnr<=maxprogram_antal-1) && (kanalnr<MAXKANAL_ANTAL-1)) {
                if ((prgnr==0) && (strcmp(tmptxt,row[0])!=0)) {
                    tvkanaler[kanalnr].putkanalname(row[0]);
                    tvkanaler[kanalnr].chanid=atoi(row[12]);                      // set chanelid in array
                    // load tv kanal icon
                    if (tvkanaler[kanalnr].get_kanal_icon()==0) {
                      GLuint icon=0;
                      char tmp[2000];
                      strcpy(tmp,"images/tv_icons/");
                      strcat(tmp,row[1]);
//                      if (file_exists(tmp)) icon=loadTexture((char *) tmp);
                      tvkanaler[kanalnr].set_kanal_icon_file(row[1]);
                      //tvkanaler[kanalnr].set_kanal_icon(icon);
                    }
                    strcpy(tmptxt,row[0]);                                        // rember channel name
                    if (debugmode & 256) printf("Channel name : %-20s ",tvkanaler[kanalnr].getkanalname());
                }
                // select by tv_grab_xx nr in array
                if (row[8]) {
                  switch (aktiv_tv_graber.graberaktivnr) {
                    case 0:
                        break;
                      // graber_dk
                      // Grab TV listings for Denamrk
                    case 2:                                                     // tv_grab_nl
                    case 4:                                                     // tv_grab_il
                    case 5:                                                     // tv_grab_na_tvmedia
                    case 7:                                                     // tv_grab_fi
                    case 8:                                                     // tv_grab_eu_dotmedia
                    case 10:                                                    // tv_grab_pt_meo
                    case 11:                                                    // tv_grab_fr
                    case 12:                                                    // tv_grab_uk_bleb
                    case 13:                                                    // tv_grab_huro
                    case 14:                                                    // tv_grab_ch_search
                    case 15:                                                    // tv_grab_it
                    case 16:                                                    // tv_grab_is
                    case 17:                                                    // tv_grab_fi_sv
                    case 18:                                                    // tv_grab_na_dtv
                    case 19:                                                    // tv_grab_tr
                    case 20:                                                    // tv_grab_eu_egon
                    case 21:                                                    // tv_grab_dk_dr
                    case 22:                                                    // tv_grab_se_tvzon
                    case 23:                                                    // tv_grab_se_tvzon
                    case 25:                                                    // tv_grab_fr_kazer
                    case 26:                                                    // tv_grab_uk_tvguide
                    case 27:                                                    // tv_grab_zz_sdjson
                      // 2 tv_grab_nl Grab TV Netherlands ver OK
                      // 5 tv_grab_fi Grab TV Finland ver OK
                      // 7 tv_grab_se_tvzon Sweden ver OK
                      // 8 tv_grab_eu_dotmedia Europe ver OK
                      // 9 tv_grab_se_swedb Sweden ver OK
                      // 18 tv_grab_tr Tyrkiye ver OK
                      if (strcmp(row[9],"None")!=0) {
                        if (strcmp("series",row[9])==0) prgtype=10;               // series
                        else if (strcmp("Movie",row[9])==0) prgtype=5;            // movie type
                        else if (strcmp("Action",row[9])==0) prgtype=5;           // movie type
                        else if (strcmp("Drama",row[9])==0) prgtype=5;            //
                        else if (strcmp("Crime",row[9])==0) prgtype=5;            // Action film
                        else if (strcmp("Mystery",row[9])==0) prgtype=5;          // movie type
                        else if (strcmp("sports",row[9])==0) prgtype=2;           //
                        else if (strcmp("Sports",row[9])==0) prgtype=2;           //
                        else if (strcmp("Soccer",row[9])==0) prgtype=2;           //
                        else if (strcmp("Fencing",row[9])==0) prgtype=3;          // children
                        else if (strcmp("Tvshow",row[9])==0) prgtype=8;           //
                        else if (strcmp("Comedy",row[9])==0) prgtype=8;           // Entertainment
                        else if (strcmp("Adventure",row[9])==0) prgtype=5;        //
                        else if (strcmp("Family",row[9])==0) prgtype=1;           // series
                        else if (strcmp("Fantasy",row[9])==0) prgtype=5;          // movie type
                        else if (strcmp("Reality",row[9])==0) prgtype=8;          //
                        else if (strcmp("Miniseries",row[9])==0) prgtype=0;       //
                        else if (strcmp("TV Movie",row[9])==0) prgtype=8;         //
                        else if (strcmp("Teleshopping",row[9])==0) prgtype=0;     //
                        else if (strcmp("tvshow",row[9])==0) prgtype=8;           //
                        else if (strcmp("Magazine",row[9])==0) prgtype=4;         //
                        else if (strcmp("News",row[9])==0) prgtype=4;             // news
                        else if (strcmp("Romance",row[9])==0) prgtype=5;          // movie type
                        else if (strcmp("Sci-Fi",row[9])==0) prgtype=9;           //
                        else if (strcmp("Action",row[9])==0) prgtype=5;           // movie type
                        else if (strcmp("Animation",row[9])==0) prgtype=3;        //
                        else if (strcmp("Documentary",row[9])==0) prgtype=7;      // Documentary
                        else if (strcmp("Lifestyle",row[9])==0) prgtype=8;        //
                        else if (strcmp("Entertainment",row[9])==0) prgtype=7;    //
                        else if (strcmp("Nature",row[9])==0) prgtype=6;           //
                        else if (strcmp("War",row[9])==0) prgtype=5;              // movie type
                        else if (strcmp("Animation",row[9])==0) prgtype=5;        // movie type
                        else if (strcmp("children",row[9])==0) prgtype=3;         // children
                        else if (strcmp("Adult",row[9])==0) prgtype=11;           // Adult tv
                        else prgtype=0;
                      }
                      break;
                    case 24:
                      // graber_ar Grab TV listings for Argentina
                      // Province avable in config mode is
                      // Buenos,capital federal
                      // chaco,cordoba
                      // entre rios,formosa
                      // la pampa,misones
                      // neuquen,rio negro
                      // salta,santa fe
                      //
                      if (strcmp(row[9],"None")!=0) {
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
                        else if (strcmp("Infantiles",row[9])==0) prgtype=3;         // children
                        else if (strcmp("Reality Show",row[9])==0) prgtype=8;
                        else prgtype=0;
                      }
                    default:
                      prgtype=0;
                  }
                } else prgtype=0;
                recorded=tvprgrecorded(row[2],row[4],row[12]);			              // get recorded status from backend
                if (prgnr<maxprogram_antal-1) tvkanaler[kanalnr].tv_prog_guide[prgnr].putprograminfo(row[4],row[2],row[3],row[6],row[7],row[8],row[11],row[5],prgtype,recorded);
                prgnr++;
                totalantalprogrammer++;
                if ((strcmp(tmptxt,row[0])!=0) || (prgnr>=maxprogram_antal-1)) {
                  if (debugmode & 256) printf(" Programs in channel : %3d \n",prgnr);
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
            if (debugmode & 256) printf("\nFound nr of tv channels %4d\nFound nr of programs    %4d\n",this->kanal_antal,totalantalprogrammer);
        }
        mysql_close(conn);
    }
    opdatere_tv_oversigt_kanal_icons();
    loading_tv_guide=false;
}






//
// Outputs a string wrapped to N columns
//

void WordWrap(char *str, int N) {
  int i,j,k,pos;
  i = pos = 0;
  char word[16000];
  while(1) {
    j = 0;
    while(!isspace(str[i])) {                                                  // Find next word
      if( str[i] == '\0' )
        break;
      word[j] = str[i];
      i++;
      j++;
    }
    word[j] = '\0'; // The value of j is the word length
    if( j > N ) { // Word length is greater than column length // print char-by-char
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
      if(pos + j > N) { // Word doesn't fit in line
        putchar('\n');
        pos = 0;
      }
      printf("%s", word );
      pos += j;
    }
    if(pos < N-1) {// print space if not last column
      putchar(' ');
      pos++;
    }
    if( str[i] == '\0' ) break;
    i++; // Skip space
  } // end of while(1)
}



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


// set program to record flag

void tv_oversigt::set_program_torecord(int selectchanel,int selectprg) {
  if (selectchanel<=tv_kanal_antal()) tvkanaler[selectchanel].tv_prog_guide[selectprg].settorecord=true;
}




// vis_tv_oversigt
// new
// den som bruges


void tv_oversigt::show_fasttv_oversigt(int selectchanel,int selectprg,bool do_update_xmltv_show) {
  float selectcolor[3]={1.0f,1.0f,1.0f};
  float timenow_color[3]={0.8f,0.8f,0.8f};
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
  unsigned int kanalomgang=100;                                                 //
  unsigned int kanalomgangofset=100;
  int n;
  int chanid;
  int kanalnr=0;
  int cstartofset=0;
  int xpos,ypos;
  int xsiz,ysiz;
  int prglength=0;
  int barsize=0;
  //float textsize1=20.0f;                                                        // default text size
  float textsize2=16.0f;                                                        // default text size (NOT IN USE)
  int starttimeinmin,starttimeintim;
  int yypos=0;
  int prg_nr=0;
  int startyofset;
  char tmptxt[1024];
  char tmptim[1024];
  char tmpmin[1024];
  char tmptxt1[1024];
  bool grayaktivprg=false;                                                      // show aktiv program in gray color
  static GLuint index;
  static bool firsttime=true;                                                   // first time update flag
  time_t prgstarttid,prgendtid;
  static int last_sec=0;
  starttid=time(NULL);
  nutid=starttid;
  timeinfo=localtime(&starttid);
  if (selectchanel>(this->vis_kanal_antal-1)) cstartofset=selectchanel-(this->vis_kanal_antal-1);
  else cstartofset=0;
  xpos=20;
  ypos=orgwinsizey-200;
  xsiz=(orgwinsizex-50);
  ysiz=150;
  glPushMatrix();
  glTranslatef(10,50, 0.0f);
  // top bar
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_tvoverskrift);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(27);     // 40
  glColor3f(1.0f, 1.0f, 1.0f);
  aktivfont.selectfont((char *) "Norasi");
  chanid=tvkanaler[1].chanid;
  strcpy(tmptxt,tvkanaler[1].chanel_name);
  glBegin(GL_QUADS); //Begin quadrilateral coordinates
  glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
  glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
  glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
  glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
  glEnd(); //End quadrilateral coordinates
  glScalef(40.0, 40.0, 1.0);
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
    case 3: if (!(do_update_xmltv_show)) sprintf(tmptxt,"TV Guide %s %d-%02d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900); else
              if (mytimelist.tm_sec & 2) sprintf(tmptxt,"TV Guide %s %d-%02d-%d Updating.",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
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
  while (n<6) {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    switch (screen_size) {
      case 4: glTranslatef(xpos+10,orgwinsizey-230-(n*150), 0.0f);
              break;
      default:
              glTranslatef(xpos+10,(orgwinsizey-230)-(n*150), 0.0f);                      // glTranslatef(xpos+10,(orgwinsizey-230)-(n*300), 0.0f);
              break;
    }
    // show clock
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
  mktime(&mytimelist);
  kanalnr=0+cstartofset;
  // hent tidspunk nu
  nutid=mktime(&nowtime_h);
  xpos=50+40;
  int do_kanal_nr=0;
  switch (screen_size) {
    case 4: this->vis_kanal_antal=5;                        // antal kanler som vises 768*1024
            break;
    case 3: this->vis_kanal_antal=8;                        // antal kanler som vises 1080p
            break;
    default:this->vis_kanal_antal=7;                        // antal kanler som vises
            break;
  }
  if (kanal_antal<this->vis_kanal_antal) this->vis_kanal_antal=kanal_antal;
  //
  // loop for channel
  //
  int xx=xpos-50;
  while ((xpos<orgwinsizex) && (do_kanal_nr<this->vis_kanal_antal)) {
    startyofset=0;
    glPushMatrix();
    switch (screen_size) {
      case 4: glTranslatef(xpos+11,orgwinsizey-210, 0.0f);
              break;
      default:glTranslatef(xpos+11,860, 0.0f);
              break;
    }
    // box show the icon for the tv channel loaded from xmltv file if exist in file
    // loaded other place
    if (tvkanaler[kanalnr].get_kanal_icon()) {
      // show channel icon
      // icon size
      ysiz=72;
      xsiz=98;
      glColor3f(1.0f, 1.0f, 1.0f);		                                      // default
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,tvkanaler[kanalnr].get_kanal_icon());
      glBegin(GL_QUADS);                                                    // box
      glTexCoord2f(0.0, 0.0); glVertex3f(xx, 70, 0.0f);
      glTexCoord2f(0.0, 1.0); glVertex3f(xx, 70-ysiz, 0.0f);
      glTexCoord2f(1.0, 1.0); glVertex3f(xx+xsiz, 70-ysiz, 0.0f);
      glTexCoord2f(1.0, 0.0); glVertex3f(xx+xsiz, 70, 0.0f);
      glEnd();
      glDisable(GL_TEXTURE_2D);
    } else {
      // show channel name
      glScalef(24.0, 24.0, 1.0);
      if (selectchanel==kanalnr) glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]); else glColor3f(0.6f, 0.6f, 0.6f);
      chanid=tvkanaler[0].chanid;
      strcpy(tmptxt," ");
      strcat(tmptxt,tvkanaler[kanalnr].chanel_name);
      *(tmptim+14)='\0';
      glcRenderString(tmptxt);
    }
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
    // mytime = overskrift tid
    // tt = tid som går fra maxtid nederst på skærmen starttid + 2 timer
    while((tvkanaler[kanalnr].tv_prog_guide[prg_nr].starttime_unix<tt) && (prg_nr<=tvkanaler[kanalnr].program_antal())) {
      // start pos orgwinsizey-245
      //ypos=orgwinsizey-245-barsize;
      ypos=orgwinsizey-245;
      // hent prg length in min
      prgstarttid=tvkanaler[kanalnr].tv_prog_guide[prg_nr].starttime_unix;              // get time in unixtime
      prgendtid=tvkanaler[kanalnr].tv_prog_guide[prg_nr].endtime_unix;                  // get time in unixtime
      prglength=tvkanaler[kanalnr].tv_prog_guide[prg_nr].program_length_minuter;        // get time in unixtime
      //printf("tt=%d %s progrm start tid %d \n",tt,ctime(&tt),prgstarttid);
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
        if ((prgstarttid<=time(0)) && (prgendtid>=time(0)) && (grayaktivprg)) {
          glColor3f(timenow_color[0],timenow_color[1], timenow_color[2]);		    // active program color
        } else {
          // show tvguide in colors ?
          if (vistvguidecolors) {
            switch(tvkanaler[kanalnr].tv_prog_guide[prg_nr].prg_type) {
              case 0:
                glColor3f(prgtypeRGB[0], prgtypeRGB[1], prgtypeRGB[2]);		         // none
                break;
              case 1:
                glColor3f(prgtypeRGB[3], prgtypeRGB[4], prgtypeRGB[5]);      	    // children
                break;
              case 2:
                glColor3f(prgtypeRGB[6], prgtypeRGB[7], prgtypeRGB[8]);      	    // sport
                break;
              case 3:
                glColor3f(prgtypeRGB[9], prgtypeRGB[10], prgtypeRGB[11]);      	  // cartoon
                break;
              case 4:
                glColor3f(prgtypeRGB[12], prgtypeRGB[13], prgtypeRGB[14]);      	// news
                break;
              case 5:
                glColor3f(prgtypeRGB[15], prgtypeRGB[16], prgtypeRGB[17]);		    // movie
                break;
              case 6:
                glColor3f(prgtypeRGB[18], prgtypeRGB[19], prgtypeRGB[20]);      	// natur
                break;
              case 7:
                glColor3f(prgtypeRGB[21], prgtypeRGB[22], prgtypeRGB[23]);      	// Documentary
                break;
              case 8:
                glColor3f(prgtypeRGB[24], prgtypeRGB[25], prgtypeRGB[26]);      	// Entertainment
                break;
              case 9:
                glColor3f(prgtypeRGB[27], prgtypeRGB[28], prgtypeRGB[29]);		    // Sci-Fi
                break;
              case 10:
                glColor3f(prgtypeRGB[30], prgtypeRGB[31], prgtypeRGB[32]);		    // serier
                break;
              default:
                glColor3f(1.0f, 1.0f, 1.0f);		    // default
                break;
            }
          } else glColor3f(0.5f, 0.5f, 0.5f);		    // show tvguide no colors no use default
        }
        if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor[0],selectcolor[1],selectcolor[0]);
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
          if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
          glcRenderString(tmptxt);
          glPopMatrix();
        }
        // show program name
        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        strcpy(tmptxt,tvkanaler[kanalnr].tv_prog_guide[prg_nr].program_navn);
        *(tmptxt+21)='\0';
        if (prglength>13) glTranslatef(xpos+20,ypos-22, 0.0f); else glTranslatef(xpos+20,ypos-7, 0.0f);
        glScalef(configdefaulttvguidefontsize, configdefaulttvguidefontsize, 1.0);
        //glScalef(textsize1, textsize1, 1.0f);
        glColor3f(0.7f,0.7f, 0.7f);		                                          // active program color
        // default color
        if ((prgstarttid<=time(0)) && (prgendtid>=time(0))) glColor3f(now_text_color[0],now_text_color[1], now_text_color[2]); else glColor3f(catalog_text_color[0],catalog_text_color[1], catalog_text_color[2]);    // active program color
        // select color
        if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
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
        if ((prgstarttid<=time(0)) && (prgendtid>=time(0)) && (grayaktivprg)) {
          glColor3f(timenow_color[0],timenow_color[1], timenow_color[2]);		    // active program color
        } else {
          // show tvguide in colors ?
          if (vistvguidecolors) {
            switch(tvkanaler[kanalnr].tv_prog_guide[prg_nr].prg_type) {
              case 0:
                glColor3f(prgtypeRGB[0], prgtypeRGB[1], prgtypeRGB[2]);		         // none
                break;
              case 1:
                glColor3f(prgtypeRGB[3], prgtypeRGB[4], prgtypeRGB[5]);      	    // children
                break;
              case 2:
                glColor3f(prgtypeRGB[6], prgtypeRGB[7], prgtypeRGB[8]);      	    // sport
                break;
              case 3:
                glColor3f(prgtypeRGB[9], prgtypeRGB[10], prgtypeRGB[11]);      	  // cartoon
                break;
              case 4:
                glColor3f(prgtypeRGB[12], prgtypeRGB[13], prgtypeRGB[14]);      	// news
                break;
              case 5:
                glColor3f(prgtypeRGB[15], prgtypeRGB[16], prgtypeRGB[17]);		    // movie
                break;
              case 6:
                glColor3f(prgtypeRGB[18], prgtypeRGB[19], prgtypeRGB[20]);      	// natur
                break;
              case 7:
                glColor3f(prgtypeRGB[21], prgtypeRGB[22], prgtypeRGB[23]);      	// Documentary
                break;
              case 8:
                glColor3f(prgtypeRGB[24], prgtypeRGB[25], prgtypeRGB[26]);      	// Entertainment
                break;
              case 9:
                glColor3f(prgtypeRGB[27], prgtypeRGB[28], prgtypeRGB[29]);		    // Sci-Fi
                break;
              case 10:
                glColor3f(prgtypeRGB[30], prgtypeRGB[31], prgtypeRGB[32]);		    // serier
                break;
              default:
                glColor3f(1.0f, 1.0f, 1.0f);		    // default
                break;
            }
          } else glColor3f(0.5f, 0.5f, 0.5f);		    // show tvguide in colors no use default
        }
        // if select program
        if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
        if (_textureutvbgmask) {
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D,_textureutvbgmask);
          glLoadName(kanalomgang+prg_nr);
          glBegin(GL_QUADS);                                              // box
        } else {
          glLoadName(kanalomgang+prg_nr);
          glBegin(GL_LINE_LOOP);                                          // line
        }
        glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos-ysiz, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos-ysiz, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
        glEnd();
        glPopMatrix();
        if (prglength>10) {
          glPushMatrix();
          glDisable(GL_TEXTURE_2D);
          glTranslatef(xpos+20,ypos-28, 0.0f);
          glScalef(configdefaulttvguidefontsize, configdefaulttvguidefontsize, 1.0);
          if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
          strcpy(tmptxt,tvkanaler[kanalnr].tv_prog_guide[prg_nr].program_navn);
          *(tmptxt+21)='\0';
          if ((prgstarttid<=time(0)) && (prgendtid>=time(0))) glColor3f(now_text_color[0],now_text_color[1], now_text_color[2]); else glColor3f(catalog_text_color[0],catalog_text_color[1], catalog_text_color[2]);	    // active program color
          glcRenderStyle(GLC_TRIANGLE);
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
          if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
          glcRenderString(tmptxt);
          if (tvkanaler[kanalnr].tv_prog_guide[prg_nr].settorecord) {
            glColor3f(1.0f,0.0f,0.0f);
            glcRenderString(" R");
            ask_tv_record=false;
          }
          glPopMatrix();
        } else {
          glPushMatrix();
          strcpy(tmptxt,tvkanaler[kanalnr].tv_prog_guide[prg_nr].program_navn);
          *(tmptxt+21)='\0';
          glDisable(GL_TEXTURE_2D);
          glTranslatef(xpos+20,ypos-8, 0.0f);
          glScalef(textsize2, textsize2, 1.0);
          if ((prgstarttid<=time(0)) && (prgendtid>=time(0))) glColor3f(0.5f,0.5f, 0.5f);	else glColor3f(catalog_text_color[0],catalog_text_color[1], catalog_text_color[2]);   // active program color
          if ((selectchanel==kanalnr) && (selectprg==prg_nr)) glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
          glcRenderString(tmptxt);
          if (tvkanaler[kanalnr].tv_prog_guide[prg_nr].settorecord) {
            glColor3f(1.0f,0.0f,0.0f);
            glcRenderString(" R");
            ask_tv_record=false;
          }
          glPopMatrix();
        }
        barsize=barsize+(prglength*5);
        yypos+=1;
      }
      prg_nr++;                                                                 // next program
    }
    kanalomgang+=kanalomgangofset;                                              // next channel
    xpos+=220;
    kanalnr++;
    do_kanal_nr++;
  }
  //
  // show clock line over tvguide gfx
  //
  if (!(loading_tv_guide)) {
    time(&rawtime);
    timelist=localtime(&rawtime);
    if ((timelist->tm_hour==mytimelist.tm_hour) || (timelist->tm_hour==mytimelist.tm_hour+1)) {
      xpos=35;
      ypos=orgwinsizey-298;
      xsiz=(orgwinsizex-280);
      if (this->vis_kanal_antal>7) xsiz+=190;
      ysiz=2;
      float timelineofset=(timelist->tm_min*4.5);
      ypos-=timelineofset;
      glPushMatrix();
      glTranslatef(10,50, 0.0f);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D,_tvoverskrift);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glColor3f(0.8f, 0.8f, 0.8f);
      glBegin(GL_LINE_LOOP);
      glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
      glEnd();
      glPopMatrix();
    }
  }
}




const char *prgtypee[]={"Unknown",
                        "children",
                        "Sport",
                        "Cartoon",
                        "News",
                        "Movie",
                        "Nature",
                        "Documentary",
                        "Entertainment",
                        "Sci-Fi",
                        "Serie",
                        "Adult"};



//
// viser et prgrams record info.
// har store problemer med formatering (SKAL FIXES)

void tv_oversigt::showandsetprginfo(int tvvalgtrecordnr,int tvsubvalgtrecordnr) {
  char temptxt[8192];
  char temprgtxt[8192];
  struct tm prgtidinfo;
  time_t prgtid;
  time_t aktueltid;

  struct tm *timeinfo;

  int xpos,ypos,xsiz,ysiz;
  int antalrec=0;
  // windows background
  glPushMatrix();
  glBindTexture(GL_TEXTURE_2D, _tvbar3);
  glEnable(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glColor3f(1.0f, 1.0f, 1.0f);
  //glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glLoadName(81);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f((orgwinsizex/3)+0, (orgwinsizey/4)+0, 0.0);
  glTexCoord2f(0, 1); glVertex3f((orgwinsizex/3)+0, (orgwinsizey/4)+400, 0.0);
  glTexCoord2f(1, 1); glVertex3f((orgwinsizex/3)+850, (orgwinsizey/4)+400, 0.0);
  glTexCoord2f(1, 0); glVertex3f((orgwinsizex/3)+850, (orgwinsizey/4)+0, 0.0);
  glEnd();
  glPopMatrix();

  glPushMatrix();
  switch (configland) {
    case 0: snprintf(temprgtxt,65,"Channel  : %-10s",tvkanaler[tvvalgtrecordnr].chanel_name);
            break;
    case 1: snprintf(temprgtxt,65,"Kanal    : %-10s",tvkanaler[tvvalgtrecordnr].chanel_name);
            break;
    case 2: snprintf(temprgtxt,65,"Channel  : %-10s",tvkanaler[tvvalgtrecordnr].chanel_name);
            break;
    case 3: snprintf(temprgtxt,65,"Channel  : %-10s",tvkanaler[tvvalgtrecordnr].chanel_name);
            break;
    case 4: snprintf(temprgtxt,65,"Channel  : %-10s",tvkanaler[tvvalgtrecordnr].chanel_name);
            break;
    default:
            sprintf(temprgtxt,"Channel  : %-10s",tvkanaler[tvvalgtrecordnr].chanel_name);
  }
  glTranslatef(700,575, 0.0f);
  glScalef(20.0, 20.0,1);
  glBlendFunc(GL_ONE, GL_ONE);
  glDisable(GL_TEXTURE_2D);
  glcRenderString(temprgtxt);
  glPopMatrix();

  glPushMatrix();
  switch (configland) {
    case 0: snprintf(temprgtxt,65,"Prg name : %-20s",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn);
            break;
    case 1: snprintf(temprgtxt,65,"Prg navn : %-20s",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn);
            break;
    case 2: snprintf(temprgtxt,65,"Prg name : %-20s",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn);
            break;
    case 3: snprintf(temprgtxt,65,"Prg name : %-20s",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn);
            break;
    case 4: snprintf(temprgtxt,65,"Prg name : %-20s",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn);
            break;
    default:
          snprintf(temprgtxt,65,"Prg name : %-20s",65,tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn);
  }
  glTranslatef(700,525, 0.0f);
  glScalef(20.0, 20.0,1);
  glBlendFunc(GL_ONE, GL_ONE);
  glDisable(GL_TEXTURE_2D);
  glcRenderString(temprgtxt);
  glPopMatrix();

  glPushMatrix();
  switch (configland) {
    case 0: snprintf(temprgtxt,65,"Start    : %10s",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime);
            break;
    case 1: snprintf(temprgtxt,65,"Start    : %10s",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime);
            break;
    case 2: snprintf(temprgtxt,65,"début    : %10s",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime);
            break;
    case 3: snprintf(temprgtxt,65,"Start    : %10s",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime);
            break;
    case 4: snprintf(temprgtxt,65,"Start    : %10s",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime);
            break;
    default:snprintf(temprgtxt,65,"Start    : %10s",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime);

  }
  glTranslatef(700,500, 0.0f);
  glScalef(20.0, 20.0,1);
  glBlendFunc(GL_ONE, GL_ONE);
  glDisable(GL_TEXTURE_2D);
  glcRenderString(temprgtxt);
  glPopMatrix();


  glPushMatrix();
  switch (configland) {
    case 0: snprintf(temprgtxt,65,"Length   : %d min.",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_length_minuter);
            break;
    case 1: snprintf(temprgtxt,65,"Længde   : %d min.",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_length_minuter);
            break;
    case 2: snprintf(temprgtxt,65,"durée du : %d min.",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_length_minuter);
            break;
    case 3: snprintf(temprgtxt,65,"Programmlänge : %d min.",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_length_minuter);
            break;
    case 4: snprintf(temprgtxt,65,"Length   : %d min.",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_length_minuter);
            break;
    default:
            snprintf(temprgtxt,65,"Length   : %d min.",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_length_minuter);
  }
  glTranslatef(700,475, 0.0f);
  glScalef(20.0, 20.0,1);
  glBlendFunc(GL_ONE, GL_ONE);
  glDisable(GL_TEXTURE_2D);
  glcRenderString(temprgtxt);
  glPopMatrix();


  glPushMatrix();
  if (antalrec==-1) antalrec=tvprgrecordedbefore(tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn,tvkanaler[tvvalgtrecordnr].chanid);
  switch (configland) {
    case 0: snprintf(temprgtxt,65,"Recorded : %d times before.",antalrec);
            break;
    case 1: snprintf(temprgtxt,65,"Optaget  : %d gange før.",antalrec);
            break;
    case 2: snprintf(temprgtxt,65,"Recorded : %d times before.",antalrec);
            break;
    case 3: snprintf(temprgtxt,65,"Recorded : %d times before.",antalrec);
            break;
    case 4: snprintf(temprgtxt,65,"Recorded : %d times before.",antalrec);
            break;
    default: snprintf(temprgtxt,65,"Recorded : %d times before.",antalrec);

  }
  if (tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].recorded) strcat(temprgtxt," Set to record");
  glTranslatef(700,450, 0.0f);
  glScalef(20.0, 20.0,1);
  glBlendFunc(GL_ONE, GL_ONE);
  glDisable(GL_TEXTURE_2D);
  glcRenderString(temprgtxt);
  glPopMatrix();



  glPushMatrix();
  if (tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].prg_type<=10)
  sprintf(temprgtxt,"Type     : %-10s",prgtypee[tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].prg_type]);
  else sprintf(temprgtxt,"Type     : %d nr  ",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].prg_type);
  glTranslatef(700,425, 0.0f);
  glScalef(20.0, 20.0,1);
  glBlendFunc(GL_ONE, GL_ONE);
  glDisable(GL_TEXTURE_2D);
  glcRenderString(temprgtxt);
  glPopMatrix();

  glPushMatrix();
  switch (configland) {
    case 0: sprintf(temprgtxt,"Description : ");
            break;
    case 1: sprintf(temprgtxt,"Beskrivelse : ");
            break;
    case 2: sprintf(temprgtxt,"Description : ");
            break;
    case 3: sprintf(temprgtxt,"Description : ");
            break;
    case 4: sprintf(temprgtxt,"Description : ");
            break;
    default: sprintf(temprgtxt,"Description :");
  }
  glTranslatef(700,375, 0.0f);
  glScalef(20.0, 20.0,1);
  glBlendFunc(GL_ONE, GL_ONE);
  glDisable(GL_TEXTURE_2D);
  glcRenderString(temprgtxt);
  glPopMatrix();

  // show description
  glPushMatrix();
  snprintf(temprgtxt,65,"%s",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].description);
  glTranslatef(700,350, 0.0f);
  glScalef(20.0, 20.0,1);
  glBlendFunc(GL_ONE, GL_ONE);
  glDisable(GL_TEXTURE_2D);
  glcRenderString(temprgtxt);
  glPopMatrix();


  if (strptime(tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime,"%Y-%m-%d %H:%M:%S",&prgtidinfo)==NULL) {
      printf("RECORDED PROGRAM DATE FORMAT ERROR can't convert. by strptime\n");
  }
  //
  // can we record program
  // if we can show icon for record
  //
  prgtid=mktime(&prgtidinfo);
  time(&aktueltid);					            // hent hvad klokken er
  timeinfo=localtime(&aktueltid);				// convert to localtime
  if ((difftime(aktueltid,prgtid)<=0) && (tvprgrecorded(tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime,tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn,temptxt)==0)) {

    glPushMatrix();
    // close button
    xsiz=100;
    ysiz=100;
    xpos=((orgwinsizex/2)-xsiz/2)-200;
    ypos=((orgwinsizey/2)-ysiz/2)-200;
    xpos=10.0f;
    ypos=-50.0f;
    glTranslatef(1400,400, 0.0f);
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
    glPopMatrix();
  } else {
    glPushMatrix();
    // close button
    xsiz=100;
    ysiz=100;
    xpos=((orgwinsizex/2)-xsiz/2)-200;
    ypos=((orgwinsizey/2)-ysiz/2)-200;
    glTranslatef(-10.0f, -2.0f, 0.0f);
    xpos=15.0f;
    ypos=-50.0f;
    glTranslatef(1400,400, 0.0f);
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
    glPopMatrix();
  }

  //glPopMatrix();
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
    char *database = (char *) "mythtvcontroller";
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
    //glBindTexture(GL_TEXTURE_2D, _tvoldrecorded);
    glBindTexture(GL_TEXTURE_2D, 0);
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
