//
// Show tv guide
//
#include <mysql.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <GL/glew.h>
#include <GL/glc.h>     // danish ttf support
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <math.h>
#include <ctype.h>
#include <libical/ical.h>
#include <libxml/parser.h>
#include <fmt/format.h>
#include <algorithm>

#include <curl/curl.h>
#include <jsoncpp/json/json.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "renderer.h"
#include "utility.h"
#include "myctrl_tvprg.h"
#include "myth_ttffont.h"
#include "myth_setup.h"
#include "myctrl_readwebfile.h"
#include "readjpg.h"
// #include "myctrl_glprint.h"
#include "myth_config.h"

// extern mFont font12;  // 12px font
// extern mFont font18;  // 18px font
// extern mFont font24;  // 24px font

extern Renderer renderer;

extern Font myfont;
extern Font myfont2;
extern Font myfont_tv_guide_overskrift;
extern Font myfont_mini;
extern config_icons config_menu;
extern channel_list_type channel_list;
extern char localuserhomedir[4096];                                             // user homedir
extern const char *dbname;                                                      // internal database name in mysql (music,movie,radio)
extern FILE *logfile;
extern char debuglogdata[1024];                                // used by log system
extern GLuint setuptvgraberback;
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
extern GLuint tvguide_last_hour_icon;
extern GLuint tvguide_next_hour_icon;
extern GLuint _texturemovieinfobox;
// extern GLuint _tvrecordcancelbutton;
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



                        // bruges ikke af show_tvoversigt



                        void myglprinttv(char *string) {
  int len,i;
  len = (int) strlen(string);
  for (i = 0; i < len; i++) {
    //glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
     glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
  }
}




// ****************************************************************************************
//
// hash func
//
// ****************************************************************************************

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

// ****************************************************************************************
//
// get channel id
// OK
// ****************************************************************************************

unsigned long get_cannel_id(MYSQL *conn,char *channelname) {
  char sql[4096];
  MYSQL_RES *res;
  MYSQL_ROW row;
  unsigned long id=0;
  // mysql stuf
  try {
    if (conn) {
      sprintf(sql,"select chanid from channel where callsign like '%s'",channelname);
      mysql_query(conn,sql);
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) id=atol(row[0]);
      }
      mysql_free_result(res);
    }
  } catch (...) {
    printf("Error connect to mysql.");
  }
  return(id);
}

// ****************************************************************************************
//
// check if channelname exist in db
//
// ****************************************************************************************

bool do_cannel_exist(MYSQL *conn,char *channelname) {  
  std::string channelname_str;
  MYSQL_RES *res;
  MYSQL_ROW row;
  unsigned long id=0;
  try {
    if (conn) {
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      channelname_str = "select chanid from channel where callsign like '";
      channelname_str += channelname;
      channelname_str += "'";
      mysql_query(conn,channelname_str.c_str());
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) id=atol(row[0]);
      }
      mysql_free_result(res);
    }
  } catch (...) {
    printf("Error connect to mysql.");
  }
  if (id==0) return(false); else return(true);
}


// ****************************************************************************************
//
// check if program exist in db
//
// ****************************************************************************************

bool do_program_exist(MYSQL *conn,int pchanid,char *ptitle,char *pstarttime) {
  std::string sql_str;
  MYSQL_RES *res;
  MYSQL_ROW row;
  unsigned long id=0;
  try {
    if (conn) {
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      sql_str = fmt::format("select chanid from program where chanid={} and starttime like '{}' limit 1",pchanid,pstarttime);
      mysql_query(conn,sql_str.c_str());
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) id=atol(row[0]);
      }
      mysql_free_result(res);
    }
  } catch (...) {
    printf("Error connect to mysql.");
  }
  if (id==0) {
    return(false);
  } else {
    return(true);
  }
}


// ****************************************************************************************
//
// get first line from string having \n in
//
// ****************************************************************************************

void getfirstlinefromstring(char *resl,char *line) {
  int n=0;
  while((line[n]!='\n') && (line[n]!='\0') && (n<strlen(line))) {
    resl[n]=line[n];
    n++;
  }
  resl[n]='\0';
}


// ****************************************************************************************
//
//
//
// ****************************************************************************************

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



// ****************************************************************************************
//
// Save channel (tvguide) data
//
// ****************************************************************************************

int tv_oversigt::saveparsexmltvdb() {
  int n=0;
  FILE *dbfil;
  char writefilename[1024];
  //getuserhomedir(writefilename);
  strcpy(writefilename,localuserhomedir);

  strcat(writefilename,"/");
  strcat(writefilename,tvguidedbfilename);
  dbfil=fopen(writefilename,"w");                                             //tvguidedb.dat
  if (dbfil) {
    while(n<this->kanal_antal) {
      fwrite(&tvkanaler[n],sizeof(tv_oversigt_pr_kanal),1,dbfil);
      n++;
    }
    fclose(dbfil);
  } else {
    write_logfile(logfile,(char *) "Error write tvguidedb.dat to disk.");
  }
  return(1);
}


// ****************************************************************************************
//
// Load channel (tvguide) data
//
// ****************************************************************************************

int tv_oversigt::loadparsexmltvdb() {
  int n=0;
  FILE *dbfil;
  char writefilename[1024];
  //getuserhomedir(writefilename);
  strcpy(writefilename,localuserhomedir);
  strcat(writefilename,"/");
  strcat(writefilename,tvguidedbfilename);
  dbfil=fopen(writefilename,"r");                                             // tvguidedb.dat
  if (dbfil) {
    while(!(feof(dbfil))) {
      printf("Loading channel # %d \n",n);
      fread(&tvkanaler[n],sizeof(tv_oversigt_pr_kanal),1,dbfil);
      n++;
    }
    fclose(dbfil);
  } else {
    write_logfile(logfile,(char *) "Error loading tvguidedb.dat from disk.");
  }
  return(1);
}


// ****************************************************************************************
//
// Funktion to draw lines of text in screen.
//
// ****************************************************************************************

void drawLinesOfText(const std::string& text, float x, float y, float scale,int maxWidth,int maxlines,int color,bool center) {
  std::istringstream stream(text);
  std::string word;
  std::string currentLine;
  std::string formattext;
  float yoffset=0.0f;
  int linecount=0;
  while (stream >> word) {
    // Check if adding the word exceeds the maximum width then print the line
    if (currentLine.length() + word.length() + 1 > maxWidth) {
      if (currentLine.length()>0) {
        if (center) formattext = fmt::format("{:^{}s}",currentLine,maxWidth);
        else formattext = fmt::format("{}",currentLine);
        renderer.AddText(&myfont, x, y+yoffset  ,formattext,1,1,1,1);
        // drawText(font12,formattext.c_str(), x, y + yoffset, scale, color);
        currentLine = word; // Start a new line with the current word
        linecount++;
        yoffset-=18.0f;
      } else {
        currentLine = word; // Start a new line with the current word
      }
    } else {
      if (!currentLine.empty()) {
        currentLine += " "; // Add a space before the next word
      }
      currentLine += word; // Add the word to the current line
    }
    if (linecount>maxlines) break;
  }
    // Print any remaining text in the current line
  if (linecount<=maxlines) {
    if (!currentLine.empty()) {
      if (currentLine.length()>maxWidth) currentLine.resize(maxWidth);
      if (center) formattext = fmt::format("{:^{}s}",currentLine,maxWidth);
      else formattext = fmt::format("^{}",currentLine,maxWidth);
      renderer.AddText(&myfont, x, y+yoffset  ,formattext,1,1,1,1);
      // drawText(font12,formattext.c_str(), x, y + yoffset, scale, color);
    }
  }
}



// new version of dr program guide api. 2024-07-31


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents,size*nmemb);
    return size*nmemb;
}


std::string MariaDateTime(std::string s) {
  if (s.size() >= 19) {
    s[10] = ' ';   // T -> space
    s.resize(19);  // fjern +02:00
  }
  return s;
}



std::string Get_time_min(std::string s) {
  if (s.size() >= 19) {
    s.erase(0,11);
    s.resize(5);
  }
  return s;
}


time_t StringToUnix(const std::string &datetime) {
    std::tm tm = {};
    std::istringstream ss(datetime.substr(0, 19)); // Ignorer .000
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return mktime(&tm);   // Fortolker tiden som lokal tid
}

// ****************************************************************************************
//
// Henter kanal ikoner fra dr.dk json api. Gemmer i dr_kanal
//
// ****************************************************************************************

int hent_tv_kanal_icons() {
  std::string url = "https://prod95-cdn.dr-massive.com/api/channels/195869?channel_group=all&device=web_browser&ff=idp%2Cldp%2Crpt&geoLocation=dk&isDeviceAbroad=false&lang=da&page=3&segments=drtv%2Coptedin&sub=Anonymous2";
  CURL *curl = curl_easy_init();
  if (!curl)
      return 1;
  std::string response;
  curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
  curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallback);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response);
  struct curl_slist *headers=NULL;
  headers=curl_slist_append(headers,"User-Agent: Mozilla/5.0");
  headers=curl_slist_append(headers,"Accept: application/json");
  curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);
  CURLcode res=curl_easy_perform(curl);
  if(res!=CURLE_OK) {
    std::cout << curl_easy_strerror(res) << std::endl;
    return 1;
  }
  Json::Value root;
  Json::CharReaderBuilder builder;
  std::stringstream ss(response);
  std::string errs;
  if(!Json::parseFromStream(builder,ss,&root,&errs)) {
      std::cout << errs << std::endl;
      return 1;
  }
  std::ofstream out("dr_kanal_icons.json");
  out << root;
  out.close();
  std::cout << "Gemt som dr_kanal_icons.json" << std::endl;

  /*
  for (const auto &channel : root) {
  }
  */
  return(1);
}

std::tm AddDays(std::tm date, int days) {
    std::mktime(&date);  // normaliser
    date.tm_mday += days;
    std::mktime(&date);
    return date;
}


// **************************************************************************************************
//
// create db if not exist
//
// **************************************************************************************************


int tv_oversigt::createdb_if_not_exist() {
  // check if db exist
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  std::string sql;
  bool fundet=false;
  conn=mysql_init(NULL);
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, NULL, 0, NULL, 0);
    // if tvguide db not exist create it.
    sql = "create table IF NOT EXISTS mythtvcontroller.channel(chanid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,channum varchar(10),freqid varchar(10) ,sourceid int(10) unsigned,callsign varchar(20),name  varchar(64), icon varchar(255), finetune int(11) , videofilters varchar(255), xmltvid varchar(64), recpriority int(10), contrast int(11) DEFAULT 32768, brightness int(11) DEFAULT 32768, colour int(11) DEFAULT 32768, hue int(11) DEFAULT 32768, tvformat varchar(10), visible tinyint(1) DEFAULT 1, outputfilters varchar(255), useonairguide tinyint(1) DEFAULT 0, mplexid smallint(6), serviceid mediumint(8) unsigned, atsc_major_chan int(10) unsigned DEFAULT 0, atsc_minor_chan int(10) unsigned DEFAULT 0, last_record datetime, default_authority varchar(32), commmethod int(11) DEFAULT +1, iptvid smallint(6) unsigned,orderid int(12) unsigned DEFAULT 0,iconfile varchar(200))";
    mysql_query(conn,sql.c_str());
    res = mysql_store_result(conn);
    mysql_free_result(res);
    //
    sql = "create table IF NOT EXISTS mythtvcontroller.program(chanid int(10) unsigned NOT NULL,starttime datetime, endtime datetime ,title varchar(128),subtitle varchar(128), description text, category varchar(64),preview_image text)";
    mysql_query(conn,sql.c_str());
    res = mysql_store_result(conn);
    mysql_free_result(res);
    //
    sql = "create table IF NOT EXISTS mythtvcontroller.programgenres(chanid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,starttime datetime,relevance char(1),genre varchar(30))";
    mysql_query(conn,sql.c_str());
    res = mysql_store_result(conn);
    mysql_free_result(res);
    //
    sql = "create table IF NOT EXISTS mythtvcontroller.programrating(chanid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,starttime datetime,system varchar(8), rating varchar(16))";
    mysql_query(conn,sql.c_str());
    res = mysql_store_result(conn);
    mysql_free_result(res);
    //
    sql = "create table IF NOT EXISTS mythtvcontroller.record(recordid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,type int(10),chanid  int(10) unsigned,starttime time,startdate date,endtime time,enddate date,title varchar(128),subtitle varchar(128),description varchar(16000),season smallint,episode smallint,category varchar(64),profile varchar(128),recpriority int,autoexpire int,maxepisodes int,maxnewest int,startoffset int,endoffset int,recgroup varchar(32),dupmethod int,dupin int,station  varchar(20),seriesid varchar(64),programid varchar(64),inetref varchar(40),search int,autotranscode int, autocommflag  int,autouserjob1 int, autouserjob2 int,autouserjob3 int, autouserjob4 int,autometadata int,findday int,findtime time,findid int,inactive int,parentid int,transcoder int,playgroup varchar(32),prefinput int,next_record datetime,last_record datetime,last_delete datetime,storagegroup varchar(32),avg_delay int,filter int)";
    mysql_query(conn,sql.c_str());
    res = mysql_store_result(conn);
    mysql_free_result(res);
    // The recorded table, lists programs which have already been recorded (or recorded and then transcoded) and are still available for viewing, translating the Internal Filenames into something safe for human consumption.
    sql = "create table IF NOT EXISTS mythtvcontroller.recorded(chanid int(10) unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,starttime datetime,endtime datetime,title varchar(128),subtitle varchar(128),description varchar(16000),season smallint,episode smallint,category varchar(64),hostname varchar(255),bookmark int default 0,editing int(10) unsigned default 0,cutlist tinyint(1) default 0,autoexpire int(11) default 0,commflagged int(10) unsigned default 0,recgroup varchar(32) default 'Default',recordid int(11),seriesid varchar(64),programid varchar(64),inetref varchar(40),lastmodified timestamp,filesize bigint(20) default 0,stars float default 0.0,previouslyshown tinyint(1) default 0,originalairdate date,preserve tinyint(1) default 0,findid int(11) default 0,deletepending tinyint(1),transcoder int default 0,timestretch float default 1,recpriority int default 0,basename varchar(255),progstart datetime,progend datetime,playgroup varchar(32),profile varchar(32),duplicate tinyint(1) default 0,transcoded tinyint(1) default 0,watched tinyint(4) default 0,storagegroup varchar(32) default 'Default',bookmarkupdate datetime)";
    mysql_query(conn,sql.c_str());
    res = mysql_store_result(conn);
    mysql_free_result(res);
    //
    sql = "create table IF NOT EXISTS mythtvcontroller.Recgrouppassword(recgroup varchar(32),password varchar(10))";
    mysql_query(conn,sql.c_str());
    res = mysql_store_result(conn);
    mysql_free_result(res);
    //
    sql = "create table IF NOT EXISTS mythtvcontroller.Recordingprofiles(id int(10) unsigned,name varchar(128),videocodec varchar(128),audiocodec varchar(128),profilegroup int(10) unsigned)";
    mysql_query(conn,sql.c_str());
    res = mysql_store_result(conn);
    mysql_free_result(res);
    // crete index
    sql = "CREATE UNIQUE INDEX program_starttime_IDX USING BTREE ON mythtvcontroller.program (starttime,title)";
    mysql_query(conn,sql.c_str());

    sql = "CREATE UNIQUE INDEX channel_name_IDX USING BTREE ON mythtvcontroller.channel (name)";
    mysql_query(conn,sql.c_str());
    return(1);
  }
  return(0);
}


std::string escape(MYSQL* conn, const std::string& input) {
  std::vector<char> buf(input.length() * 2 + 1);
  unsigned long len = mysql_real_escape_string(conn, buf.data(), input.c_str(), input.length());
  return std::string(buf.data(), len);
}

// *******************************************************************************************
//
// Henter tv guide samt help function
//
// *******************************************************************************************

bool check_tv_guide_date(MYSQL *conn,time_t tidspunkt) {
  long antal=0;
  bool fundet=false;
  std::string sql;
  MYSQL_RES *mres;
  MYSQL_ROW mrow;
  std::string startdato;
  time_t nu = tidspunkt;
  std::tm tm = *std::localtime(&nu);
  tm.tm_hour = tm.tm_hour - 2;
  tm.tm_min = 0;
  std::ostringstream ss;
  ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  startdato=ss.str();
  sql = fmt::format("SELECT count(c.name) FROM mythtvcontroller.program inner join mythtvcontroller.channel c on mythtvcontroller.program.title = c.name where starttime>='{}' order by c.orderid,title",startdato);
  if (mysql_query(conn,sql.c_str())==0) {
    mres = mysql_store_result(conn);
    while ((mrow = mysql_fetch_row(mres)) != NULL) {
      antal=std::stoi(mrow[0]);
      if (antal>0) fundet=true; // dato data fundet
    }
  }
  mres = mysql_store_result(conn);
  mysql_free_result(mres);
  return(fundet);
}

// *******************************************************************************************
//
// Henter tv guide fra dr.dk json api. Gemmer i mysql db. Return 0 on success, 1 on error
//
// *******************************************************************************************

int tv_oversigt::get_dr_proguide(int ofset_from_today) {
    DRProgram new_program;
    std::string sql;
    std::string temptxt;
    std::string datostring;
    MYSQL *mconn;
    MYSQL_RES *mres;
    MYSQL_ROW mrow;
    createdb_if_not_exist();
    std::time_t now = std::time(nullptr);
    std::tm today = *std::localtime(&now);
    today=AddDays(today, ofset_from_today + (-1));
    datostring=fmt::format("{:04d}-{:02d}-{:02d}", today.tm_year + 1900, today.tm_mon + 1, today.tm_mday);
    std::cout << "Henter tvguide dato " << datostring << std::endl;
    // clear old
    tvkanaler.clear();

    std::string url =fmt::format("https://prod95-cdn.dr-massive.com/api/schedules?"
        "channels=20875,20876,20892,20966,21546,22221,22463,192099,237449,22410,21302,21468,21514,21593,21980,299482"
        "&date={}"
        "&device=web_browser"
        "&duration=24"
        "&ff=idp,ldp,rpt"
        "&geoLocation=dk"
        "&hour=22"        
        "&isDeviceAbroad=false"
        "&lang=da"
        "&segments=drtv,optedin"
        "&sub=Anonymous2",datostring);

    // Connect to database
    mconn=mysql_init(NULL);
    if (mconn) {
      mysql_real_connect(mconn, configmysqlhost,configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0);
    } else {
      std::cout << "Error connecting to database: " << mysql_error(mconn) << std::endl;
      return 1;
    }
    // hent kun hvis data ikke findes i db
    if (check_tv_guide_date(mconn,now+(dags_ofset*60*60))==false) {
      // open curl
      CURL *curl = curl_easy_init();
      if (!curl)
          return 1;
      std::string response;
      curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
      curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
      curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
      curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
      curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallback);
      curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response);
      struct curl_slist *headers=NULL;
      headers=curl_slist_append(headers,"User-Agent: Mozilla/5.0");
      headers=curl_slist_append(headers,"Accept: application/json");
      curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);
      CURLcode res=curl_easy_perform(curl);
      if(res!=CURLE_OK) {
        std::cout << curl_easy_strerror(res) << std::endl;
        return 1;
      }
      Json::Value root;
      Json::CharReaderBuilder builder;
      std::stringstream ss(response);
      std::string errs;
      if(!Json::parseFromStream(builder,ss,&root,&errs)) {
          std::cout << errs << std::endl;
          return 1;
      }
      std::ofstream out("dr_programguide.json");
      out << root;
      out.close();
      std::cout << "Gemt som dr_programguide.json" << std::endl;
      // root er array med kanaler
      int ii=0;
      int channel_nr=1;
      tvkanaler.reserve(10);
      for (const auto &channel : root) {
        tv_oversigt_pr_kanal new_kanal;
        std::string channelId = channel["channelId"].asString();
        std::cout << "KANAL ID: " << channelId << std::endl;
        new_kanal.chanid = std::stoi(channelId);
        // create new tv kanal
        tvkanaler.push_back(new_kanal);
        bool gemkanal=false;
        std::string husk_ny_kanal="";
        std::string nykanal="";
        try {
          for (const auto &program : channel["schedules"]) {
            const auto &item = program["item"];
            new_program.broadcastChannel= item["broadcastChannel"].asString();
            new_program.title           = item["title"].asString();
            new_program.preview_image   = item["images"]["tile"].asString();
            // change image size in url
            
            // preview image
            std::string findstring = "Width=1920&Height=1080";
            std::string replacestring = "Width=230&Height=130";
            size_t pos = new_program.preview_image.find(findstring);
            if (pos != std::string::npos) {
              new_program.preview_image.replace(pos, findstring.length(), replacestring);
            }
            
            new_program.description     = item["description"].asString();
            std::replace(new_program.description.begin(), new_program.description.end(), '\n', ' ');
            new_program.start           = program["startTimeInDefaultTimeZone"].asString();
            new_program.end             = program["endTimeInDefaultTimeZone"].asString();
            new_program.starttime_unix  = StringToUnix(new_program.start);
            new_program.endtime_unix    = StringToUnix(new_program.end);
            new_program.program_length_minuter = (int)((new_program.endtime_unix - new_program.starttime_unix) / 60);
            new_program.id              = item["id"].asString();
            new_program.aktive          = false;
            bool har_keyword=false;
            for (const auto& keyword : program["item"]["keywords"]) {
              if (har_keyword==false) {
                std::string tekst = keyword.asString();
                new_program.keywords = tekst;
                har_keyword=true;
              }
            }
            tvkanaler.at(ii).programs.push_back(new_program);
            tvkanaler.at(ii).kanal_name   = item["broadcastChannel"].asString();

            tvkanaler.at(ii).channel_icon_name = "images/tv_icons/";
            tvkanaler.at(ii).channel_icon_name = tvkanaler.at(ii).channel_icon_name + new_program.broadcastChannel;
            tvkanaler.at(ii).channel_icon_name = tvkanaler.at(ii).channel_icon_name + ".png";
            // std::cout << "Added program " << new_program.title << std::endl;

            sql = fmt::format("INSERT INTO mythtvcontroller.program (title, chanid,starttime,endtime,subtitle,description,category,preview_image) VALUES ('{}',{},'{}','{}','{}','{}','{}','{}')",
              escape(mconn,new_program.broadcastChannel), 
              ii, 
              MariaDateTime(new_program.start),
              MariaDateTime(new_program.end),
              escape(mconn,new_program.title), 
              escape(mconn,new_program.description), 
              escape(mconn,new_program.keywords),
              escape(mconn,new_program.preview_image));
            if (mysql_query(mconn,sql.c_str())!=0) {
              // debuglogdata = "Error inserting program: ";
              // write_logfile(logfile,(char *) debuglogdata);
                // std::cout << "Error inserting program: " << mysql_error(mconn) << std::endl;
                // std::cout << "SQL: " << sql << std::endl << std::endl;
            }
            // create channel in db if not exist
            husk_ny_kanal=nykanal;
            nykanal=tvkanaler.at(ii).kanal_name;
            if (husk_ny_kanal != nykanal) gemkanal=true; else gemkanal=false;
            if (gemkanal) {
              sql = fmt::format("INSERT IGNORE INTO mythtvcontroller.channel (chanid, channum, freqid, sourceid, callsign, name, icon, finetune, videofilters, xmltvid, recpriority, contrast, brightness, colour, hue, tvformat, visible, outputfilters, useonairguide, mplexid, serviceid, atsc_major_chan, atsc_minor_chan, last_record, default_authority, commmethod, iptvid, orderid , iconfile) VALUES(0, '', '', 0,'', '{}', '{}', 0, '', '', 0, 32768, 32768, 32768, 32768, NULL, 1, NULL, 0, NULL, NULL, 0, 0, NULL, NULL, 1, NULL, {}, NULL)",tvkanaler.at(ii).kanal_name, tvkanaler.at(ii).kanal_name,channel_nr);
              if (mysql_query(mconn,sql.c_str())!=0) {
                std::cout << "Error inserting program: " << mysql_error(mconn) << std::endl;
              }
              channel_nr++;
            }
          }
        } catch (const std::exception& e) {
            std::cerr << "Error processing channel " << channelId << ": " << e.what() << std::endl;
        }
        ii++;
      }
      // if ((mconn) && (mres)) mysql_free_result(mres);
      mysql_close(mconn);
      curl_slist_free_all(headers);
      curl_easy_cleanup(curl);
    }
    return 0;
}



// update sorted channel list in db to show in view

int tv_oversigt::update_guide() {
  std::string sql;
  MYSQL *mconn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  int i=0;
  // Connect to database
  mconn=mysql_init(NULL);
  if (mconn) {
    mysql_real_connect(mconn, configmysqlhost,configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0);
  } else {
    std::cout << "Error connecting to database: " << mysql_error(mconn) << std::endl;
    return 1;
  }
  if (mconn) {
    int i=0;
    while( i < channel_list.channel_list.size()) {
      sql = fmt::format("update channel set orderid = {} where name like '{}'",i, channel_list.channel_list[i].name);
      if (mysql_query(mconn,sql.c_str())!=0) {
        std::cout << "Error inserting program: " << mysql_error(mconn) << std::endl;
      }
      i++;
    }
    mysql_close(mconn);
    return(1);
  }
  return(0);
  
}


// *******************************************************************************************
//
// opdate tv guide vector. get data from class var visdato_unixtime
//
// *******************************************************************************************


int tv_oversigt::opdatere_tv_oversigt(char *mysqlhost,char *mysqluser,char *mysqlpass) {
  std::string sql;
  MYSQL *mconn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  tv_oversigt_pr_kanal ny_kanal;
  DRProgram nyt_program;
  // Connect to database
  mconn=mysql_init(NULL);
  if (mconn) {
    mysql_real_connect(mconn, configmysqlhost,configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0);
  } else {
    std::cout << "Error connecting to database: " << mysql_error(mconn) << std::endl;
    return 1;
  }
  time_t nu = visdato_unixtime;
  std::tm tm = *std::localtime(&nu);
  tm.tm_hour = tm.tm_hour - 2;
  tm.tm_min = 0;
  std::ostringstream ss;
  ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  std::string startdato=ss.str();
  std::string last_kanal_name = "";
  if (mconn) {
    tvkanaler.clear();
    kanal_antal=0;
    // sql = fmt::format("SELECT title,subtitle,chanid,starttime,endtime,description,UNIX_TIMESTAMP(starttime),UNIX_TIMESTAMP(endtime),preview_image,category FROM mythtvcontroller.program where starttime>='{}' order by title",startdato);
    sql = fmt::format("SELECT c.name,subtitle,program.chanid,starttime,endtime,description,UNIX_TIMESTAMP(starttime),UNIX_TIMESTAMP(endtime),preview_image,category FROM mythtvcontroller.program inner join mythtvcontroller.channel c on mythtvcontroller.program.title = c.name where starttime>='{}' order by c.orderid,title",startdato);
    mysql_query(mconn,sql.c_str());
    res = mysql_store_result(mconn);
    if (res) {
      int omgang=0;
      while ((row = mysql_fetch_row(res)) != NULL) {
        nyt_program.broadcastChannel=row[0];
        nyt_program.title=row[1];
        nyt_program.start=row[3];
        nyt_program.end=row[4];
        nyt_program.starttime_unix=std::stoul(row[6]);
        nyt_program.endtime_unix=std::stoul(row[7]);
        nyt_program.program_length_minuter = (int)((nyt_program.endtime_unix - nyt_program.starttime_unix) / 60);
        nyt_program.description=row[5];
        nyt_program.preview_image=row[8];
        nyt_program.keywords=row[9];
        last_kanal_name=ny_kanal.kanal_name;
        // if last kanal har andet navn opret en ny kanal;
        if (row[0] != last_kanal_name) {
          if (omgang>0) {
            ny_kanal.channel_icon_name = "images/tv_icons/";
            ny_kanal.channel_icon_name = ny_kanal.channel_icon_name + last_kanal_name;
            ny_kanal.channel_icon_name = ny_kanal.channel_icon_name + ".png";
            tvkanaler.push_back(ny_kanal);
            kanal_antal++;
            ny_kanal.programs.clear();
          }
          ny_kanal.kanal_name=row[0];
        }
        ny_kanal.programs.push_back(nyt_program);
        omgang++;

        printf("Total programe antal = %d\n",omgang);

      }
      if (omgang>0) {
        tvkanaler.push_back(ny_kanal);
        kanal_antal++;
      }
    }
    mysql_close(mconn);
  }
  return(1);
}


// ****************************************************************************************
//
// clear tv guide (drop db)
//
// ****************************************************************************************

void tv_oversigt::cleartvguide() {
  int error=0;
  bool fundet=false;
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  std::string sqlstr;
  // mysql stuf
  conn=mysql_init(NULL);
  // Connect to database
  if (conn) {
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, NULL, 0, NULL, 0)) {
      printf("Mysql error (drop tables).\n");
      mysql_error(conn);
      error=1;
    }
    if (error==0) {      
      sqlstr = "delete from mythtvcontroller.channel where chanid like '%'";
      mysql_query(conn,sqlstr.c_str());
      mysql_free_result(res);
      sqlstr = "delete from mythtvcontroller.program where chanid like '%'";
      mysql_query(conn,sqlstr.c_str());
      mysql_free_result(res);
      sqlstr = "delete from mythtvcontroller.programgenres where chanid like '%'";
      mysql_query(conn,sqlstr.c_str());
      mysql_free_result(res);
      sqlstr = "delete from mythtvcontroller.programrating where chanid like '%'";
      mysql_query(conn,sqlstr.c_str());
      mysql_free_result(res);
      mysql_close(conn);
    }
  }
}


// ****************************************************************************************
//
// Constructor
//
// ****************************************************************************************

tv_oversigt_pr_kanal::tv_oversigt_pr_kanal() {
    programantal=0;
    chanid=0;
    // strcpy(chanel_name,"");
}


// ****************************************************************************************
//
// Destructor
//
// ****************************************************************************************

tv_oversigt_pr_kanal::~tv_oversigt_pr_kanal() {
  for(int i=0;i<programs.size();i++) {
    if (programs[i].preview_image_texture) glDeleteTextures(1,&programs[i].preview_image_texture);
  }
}




// ****************************************************************************************
//
// Constructor tv_oversigt class
//
// ****************************************************************************************

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
    vis_kanal_antal=8;                                                        // default nr of channels to display (will change if no room to show all the channels)
    // get time now
    time(&rawtime);
    // convert clovk to localtime
    timelist=localtime(&rawtime);
    vistvguidekl=timelist->tm_hour;
    if (vistvguidekl>24) vistvguidekl=0;
    visdato_unixtime=time(nullptr);                           // setr now time
}



// ****************************************************************************************
//
// Destructor tv_oversigt class
//
// ****************************************************************************************

tv_oversigt::~tv_oversigt() {
  for(int i=0;i<tvkanaler.size();i++) {
    if (tvkanaler[i].channel_icon) glDeleteTextures(1,&tvkanaler[i].channel_icon);
  }
}


// ****************************************************************************************
//
// Reset tvguide time to localtime
//
// ****************************************************************************************

void tv_oversigt::reset_tvguide_time() {
  time_t rawtime;
  struct tm *timelist;
  time(&rawtime);
  timelist=localtime(&rawtime);
  vistvguidekl=timelist->tm_hour;
  if (vistvguidekl>24) vistvguidekl=0;
}

// ****************************************************************************************
//
// Clean tvprogram oversigt
//
// ****************************************************************************************

int tv_oversigt::cleanchannels() {
  tvkanaler.clear();
  return(1);
}


// ****************************************************************************************
//
// Checker om et program er optaget tidligere og retunere antal
//
// ****************************************************************************************

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


// ****************************************************************************************
//
// Checker om et program er optaget retunere type
//
// ****************************************************************************************

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

// ****************************************************************************************
//
// Fjerner et program som skal optages
//
// ****************************************************************************************

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

// ****************************************************************************************
//
// Indsæt into table record to schetule new tv recording
//
// ****************************************************************************************

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
    if (strptime(tvkanaler[tvvalgtrecordnr].programs[tvsubvalgtrecordnr].start.c_str(),"%Y-%m-%d %H:%M:%S",&prgtidinfo)==NULL) {
      printf("DO INSERT RECORDED PROGRAM DATE FORMAT ERROR can't convert. by strptime\n");
    }
    // lav tv proram starttid om til time_t format
    prgtid=mktime(&prgtidinfo);
    if ((difftime(aktueltid,prgtid)<=0) && (sqlselect)) {
      sprintf(sqlselect,"SELECT channel.name as channelname, TIME(starttime) as starttime,DATE(starttime) as startdate,TIME(endtime) as endtime,DATE(endtime) as enddate,NOW() as datenu, program.chanid, program.category from program left join channel on program.chanid=channel.chanid where program.title='%s' and program.starttime='%s' and program.endtime='%s'",tvkanaler[tvvalgtrecordnr].programs[tvsubvalgtrecordnr].title.c_str(),tvkanaler[tvvalgtrecordnr].programs[tvsubvalgtrecordnr].start.c_str(),tvkanaler[tvvalgtrecordnr].programs[tvsubvalgtrecordnr].end.c_str());
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
          sprintf(sqlselect,"INSERT INTO record values (0,1,%s,'12:00:00','2018-01-02','12:00:00','2018-01-02',\"%s\",\"%s\",\"%s\",11,12,\"%s\",'pro',15,16,17,18,19,20,'Default',22,23,'station','serid','prgid','intref',28,29,30,31,32,33,34,35,36,'12:00:12',38,39,40,41,'playgroup',43,'2017-01-02 12:00:00','2017-01-02 12:00:00','2017-01-02 12:00:00','storegrp',48,49)",row[6],tvkanaler[tvvalgtrecordnr].programs[tvsubvalgtrecordnr].title.c_str(), tvkanaler[tvvalgtrecordnr].programs[tvsubvalgtrecordnr].title.c_str(),tvkanaler[tvvalgtrecordnr].programs[tvsubvalgtrecordnr].description.c_str(),row[8]);
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



// ****************************************************************************************
//
// retunere tvprg starttid in unix start time
//
// ****************************************************************************************

time_t tv_oversigt::hentprgstartklint(int kanalnr,int prgnr) {
    if ((kanalnr>=0) && (kanalnr<kanal_antal)) return(tvkanaler[kanalnr].programs[prgnr].starttime_unix); else return(0);
}


// ****************************************************************************************
//
// load kanal icons
// used internal by opdatere_tv_oversigt
//
// ****************************************************************************************

void tv_oversigt::opdatere_tv_oversigt_kanal_icons() {
  char file[2048];
  char tmp[2048];
  GLuint icon=0;
  for (int i=0;i<kanal_antal;i++) {
    if (!(tvkanaler[i].get_kanal_icon())) {
      strcpy(file,"images/tv_icons/");
      // tvkanaler[i].get_kanal_icon_file(tmp);
      strcat(file,tmp);
//      icon=loadTexture(file);
      tvkanaler[i].set_kanal_icon(icon);
    }
  }
}


// ****************************************************************************************
//
// update tv channel status (view/hide) fro
//
// ****************************************************************************************

int tv_oversigt::set_channel_state(channel_list_struct *channel_list) {
  char sqlselect[2048];
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  int nr=0;
  // mysql stuf
  char *database = (char *) "mythtvcontroller";
  conn=mysql_init(NULL);
  // Connect to database
  if (mysql_real_connect(conn, this->mysqllhost,this->mysqlluser, this->mysqllpass, database, 0, NULL, 0)) {
    while ((strcmp(channel_list[nr].name,"")!=0) && (nr<199)) {
      strcpy(sqlselect,"UPDATE channel set channel.visible=");
      if (channel_list[nr].selected) strcat(sqlselect,"1");
      else strcat(sqlselect,"0");
      strcat(sqlselect," where name like '");
      strcat(sqlselect,channel_list[nr].name);
      strcat(sqlselect,"' limit 1");
      mysql_query(conn,sqlselect);
      write_logfile(logfile,(char *) sqlselect);                                                // write to log file
      res = mysql_store_result(conn);
      nr++;
    }
  } else nr=-1; // set error flag
  if (nr==-1) write_logfile(logfile,(char *) "Error connect to mysql server.");
  return(1);
}

// ****************************************************************************************
//
// Outputs a string wrapped to N columns
//
// ****************************************************************************************

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


// ****************************************************************************************
//
// find start time to change the start pos in array depaint on that the time is now
//
// ****************************************************************************************

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
  while((tvkanaler[selectchanel].programs[prg_nr].starttime_unix<tt) && (prg_nr<tvkanaler[selectchanel].program_antal())) {
    prg_nr++;
  }
  return(prg_nr);
}


// ****************************************************************************************
//
//
//
// ****************************************************************************************

unsigned long tv_oversigt::getprogram_endunixtume(int selectchanel,int selectprg) {
  if (selectchanel<=tv_kanal_antal()) return(tvkanaler[selectchanel].programs[selectprg].endtime_unix);
  else return(0);
}



// ****************************************************************************************
//
//
//
// ****************************************************************************************


unsigned long tv_oversigt::getprogram_startunixtume(int selectchanel,int selectprg) {
  if (selectchanel<=tv_kanal_antal()) return(tvkanaler[selectchanel].programs[selectprg].starttime_unix);
  else return(0);
}

// ****************************************************************************************
//
// return pointer to prgname in tvguide
//
// ****************************************************************************************

std::string tv_oversigt::getprogram_prgname(int selectchanel,int selectprg) {
  return(tvkanaler[selectchanel].programs[selectprg].title);
}

// ****************************************************************************************
//
// set program to record flag
//
// ****************************************************************************************

void tv_oversigt::set_program_torecord(int selectchanel,int selectprg) {
  if (selectchanel<=tv_kanal_antal()) tvkanaler[selectchanel].programs[selectprg].set_to_record=true;
}


// ****************************************************************************************
//
// Find tv guide kanal med samme eller senere tidspunkt som tidspunkt og retunre hviken element i array som har den start tid
// hvis ingen findes gå til start dvs array element 0
// bruges ved pil up/down/left/right i tv kanal listen i vis_tv_oversigt
//
// ****************************************************************************************

int tv_oversigt::findguidetvtidspunkt(int kanalnr,time_t tidspunkt) {
    int prgnr=0;
    bool fundet=false;
    tidspunkt+=60*60;
    while((prgnr<tvkanaler[kanalnr].program_antal()) && (!(fundet))) {
        if ((time_t) tvkanaler[kanalnr].programs[prgnr].starttime_unix<tidspunkt) {
          if ((tvkanaler[kanalnr].programs[prgnr].starttime_unix<tidspunkt) && (tvkanaler[kanalnr].programs[prgnr].endtime_unix<tidspunkt)) prgnr++;
          else fundet=true;
        } else fundet=true;
    }
    if (fundet) {
      while (((tvkanaler[kanalnr].programs[prgnr].starttime_unix<tidspunkt) && (tvkanaler[kanalnr].programs[prgnr].endtime_unix<tidspunkt)) && (prgnr<tvkanaler[kanalnr].program_antal())) {
        prgnr++;
      }
    }
    if (fundet) return(prgnr); else return(0);
}



// ****************************************************************************************
//
// Show tv oversigt
//
// ****************************************************************************************

void tv_oversigt::show_tv_oversigt(int selectchanel,int selectprg,bool do_update_xmltv_show) {
  float tfade;
  const float channelX     = 10.0f;
  const float channelWidth = 200.0f;
  const float programX     = 240.0f;
  // Beholder din oprindelige placering og tidsskala.
  const float guideTop        = static_cast<float>(startY) - 20.0f;
  const float pixelsPerSecond = 0.2f;
  const float rowH = static_cast<float>(rowHeight);
  const float viewportH = static_cast<float>(viewHeight);
  if (rowH <= 0.0f || viewportH <= 0.0f)
    return;
  const float itemH = std::min(58.0f, rowH);
  const int channelCount = static_cast<int>(tvkanaler.size());
  // ---------------------------------------------------------
  // Tid: tag kopier, da localtime() bruger en fælles buffer.
  // ---------------------------------------------------------
  starttid = std::time(nullptr);
   // const time_t dateSource = visdato_unixtime != 0 ? visdato_unixtime : starttid;
  time_t dateSource;
  if (visdato_unixtime != 0) {
      dateSource = visdato_unixtime;
  } else {
      dateSource = starttid;
  }
  const std::tm* local = std::localtime(&dateSource);
  if (local == nullptr)
    return;
  std::tm guideTime = *local;
  // Samme starttid bruges til både tidslinje og programmer.
  guideTime.tm_hour = vistvguidekl;
  guideTime.tm_min = 0;
  guideTime.tm_sec = 0;
  guideTime.tm_isdst = -1;
  const time_t guideStart = std::mktime(&guideTime);
  if (guideStart == static_cast<time_t>(-1)) 
    return;
  const time_t guideEnd = guideStart + (3 * 60 * 60);
  // ---------------------------------------------------------
  // Overskrift
  // ---------------------------------------------------------
  renderer.AddTextureRect(27, _tvoverskrift,1, 1, 1920, 170,1, 1, 1, 1);
  const std::string heading =
    configland == 1 ? "TV Guiden" :
    configland == 4 ? "دليل التلفزيون" :
                      "TV Guide";
  std::string title = fmt::format("{} {} {:02d}-{:02d}-{:04d} {:02d}:{:02d}",heading,ugedage[guideTime.tm_wday],guideTime.tm_mday,guideTime.tm_mon + 1,guideTime.tm_year + 1900,guideTime.tm_hour,guideTime.tm_min);
  if (do_update_xmltv_show && (starttid % 4 < 2))
    title += " Updating.";
  renderer.AddText(&myfont_tv_guide_overskrift,350, 110, title,1, 1, 1, 1);
  // ---------------------------------------------------------
  // Tidslinje: hver halve time
  // ---------------------------------------------------------
  for (int n = 0; n <= 6; ++n) {
    const time_t tickTime = guideStart + n * 30 * 60;
    const std::tm* tickLocal = std::localtime(&tickTime);
     if (tickLocal == nullptr)
      continue;
    const std::tm tick = *tickLocal;
    const std::string clockText = fmt::format("{:02d}:{:02d}",tick.tm_hour,tick.tm_min);
    const float x = programX + n * 30.0f * 60.0f * pixelsPerSecond;
    renderer.AddText(&myfont2,x, 240, clockText,1, 1, 1, 1);
  }
  // ---------------------------------------------------------
  // Kinetic scroll: kun én opdatering pr. frame.
  //
  // scrollVel antages at være pixels pr. frame.
  // ---------------------------------------------------------
  scrollPos += scrollVel;
  scrollVel *= friction;
  if (std::fabs(scrollVel) < 0.01f)
    scrollVel = 0.0f;
  const float contentHeight = channelCount * rowH;
  const float maxScroll = std::max(0.0f, contentHeight - viewportH);
  if (scrollPos < 0.0f) {
    scrollPos = 0.0f;
    scrollVel = 0.0f;
  }
  else if (scrollPos > maxScroll) {
    scrollPos = maxScroll;
    scrollVel = 0.0f;
  }
  const int firstRow = static_cast<int>(scrollPos / rowH);
  const float subOffset = scrollPos - firstRow * rowH;
  const int rowsToDraw = static_cast<int>(std::ceil((viewportH + subOffset) / rowH));
  vis_kanal_antal = std::max(1,static_cast<int>(viewportH / rowH));
  // ---------------------------------------------------------
  // Kanaler
  // ---------------------------------------------------------
  for (int visibleRow = 0; visibleRow < rowsToDraw; ++visibleRow) {
    const int index = firstRow + visibleRow;
    if (index >= channelCount)
      break;
    auto& channel = tvkanaler[index];
    const float y = guideTop + visibleRow * rowH - subOffset;
    const bool selectedChannel = (index == selectchanel);
    // -----------------------------------------------------
    // Kanalikon
    // -----------------------------------------------------
    if (channel.channel_icon == 0 && !channel.channel_icon_name.empty()) {
      const GLuint texture = loadTexture((char*)channel.channel_icon_name.c_str());
      if (texture != 0) channel.channel_icon = texture;
    }
    renderer.AddTextureRect(0, _textureutvbgmask,channelX, y,channelWidth, itemH,1, 1, 1,selectedChannel ? 0.8f : fade);
    renderer.AddText(&myfont,40, y + 20,channel.kanal_name,1, 1, 1, fade);
    if (channel.channel_icon != 0) {
      const float iconSize = std::min(56.0f, itemH);
      renderer.AddTextureRect(0, channel.channel_icon,150, y + (itemH - iconSize) * 0.5f,iconSize, iconSize,1, 1, 1, 0.8f);
    }
    // back time icon
    renderer.AddRoundedTextureRect(28, tvguide_last_hour_icon,215, y,20, itemH,6, 1.0f, 1.0f, 1.0f, 0.8f);
    // next time icon
    renderer.AddRoundedTextureRect(29, tvguide_next_hour_icon,1900, y,20, itemH,6, 1.0f, 1.0f, 1.0f, 0.8f);

    // -----------------------------------------------------
    // Programmer
    // -----------------------------------------------------
    // Beholder dit start_ofset, men undgår negative indekser.
    std::size_t firstProgram=0;
    if (start_ofset>0) {
      firstProgram = static_cast<std::size_t>(start_ofset);
    } else {
      firstProgram = 0;
    } 
    int visibleProgram = 0;
    // Samme kanal får samme ID-base under scroll.
    const unsigned int channelIdBase = 100u + static_cast<unsigned int>(index) * 100u;
    for (std::size_t p = firstProgram; p < channel.programs.size(); ++p) {
      auto& program = channel.programs[p];
      program.aktive = false;
      const time_t programStart = program.starttime_unix;
      const time_t programEnd   = program.endtime_unix;
      if (programEnd <= programStart)
        continue;
      // Programmet overlapper ikke det viste tidsrum.
      if (programEnd <= guideStart || programStart >= guideEnd)
        continue;
      const time_t visibleStart = std::max(programStart, guideStart);
      const time_t visibleEnd = std::min(programEnd, guideEnd);
      const float x = programX + static_cast<float>(visibleStart - guideStart) * pixelsPerSecond;
      float width = std::max(1.0f,static_cast<float>(visibleEnd - visibleStart) * pixelsPerSecond - 4.0f);
      // -------------------------------------------------
      // Kategorifarve
      // -------------------------------------------------
      float r = 1.0f;
      float g = 1.0f;
      float b = 1.0f;
      const auto& keywords = program.keywords;
      if (keywords == "editorialptaxtop_serier") {
        r = 0.0f; 
        g = 1.0f; 
        b = 0.0f;
      } else if (keywords == "editorialptaxtop_dokumentar") {
        r = 0.0f; 
        g = 0.0f; 
        b = 1.0f;
      } else if (keywords == "editorialptaxtop_nyheder") {
        r = 0.0f; 
        g = 0.5f; 
        b = 1.0f;
      } else if (keywords == "editorialptaxtop_børn/fakta") {
        r = 0.5f; 
        g = 0.0f; 
        b = 1.0f;
      } else if (keywords == "editorialptaxtop_børn/fiktion") {
        r = 0.5f; 
        g = 0.5f; 
        b = 1.0f;
      } else if (keywords == "editorialptaxtop_film") {
        r = 1.0f; 
        g = 0.5f; 
        b = 0.5f;
      } else if (keywords == "editorialptaxtop_børn/animation") {
        r = 0.5f; 
        g = 1.0f; 
        b = 0.2f;
      } else if (keywords == "editorialptaxtop_tv-program") {
        r = 0.7f; 
        g = 0.2f; 
        b = 0.8f;
      }
      if (vis_tv_guide) tfade=1.0f;
      else tfade=fade;
      const bool selected = selectedChannel && visibleProgram == selectprg;
      const unsigned int programId = channelIdBase + static_cast<unsigned int>(p - firstProgram);
      program.aktive = selected;
      program.program_idnr = programId;
      
      if (x+width>1896) {
        width=1896-x;
      }

      renderer.AddRoundedTextureRect(programId,0,x,y,width,itemH,10, r, g, b,fade);
      const std::string timeText = fmt::format("{} - {} {} ",Get_time_min(program.start),Get_time_min(program.end),programId);
      renderer.AddText(&myfont_mini,x + 8, y + 18,timeText,1, 1, 1, tfade);
      const std::string titleText = fmt::format("{} ",program.title);
      renderer.AddText(&myfont,x + 10, y + 50,titleText,selected ? 0.2f  : 1.0f,selected ? 0.21f : 1.0f,selected ? 0.20f : 1.0f,tfade);
      if (selected) {
        renderer.AddThickLine(x,y,width+x,y,2.0f,1.0f,1.0f,1.0f,fade);
        renderer.AddThickLine(x,y+itemH,width+x,y+itemH,2.0f,1.0f,1.0f,1.0f,fade);
        renderer.AddThickLine(x,y ,x,y+itemH,2.0f,1.0f,1.0f,1.0f,fade);
        renderer.AddThickLine(x+width,y,x + width,y+itemH,2.0f,1.0f,1.0f,1.0f,fade);
      }
      ++visibleProgram;
    }
  }
}

// ****************************************************************************************
//
// viser et prgrams info.
//
// ****************************************************************************************

void tv_oversigt::showandsetprginfo() {
  std::string temprgtxt;
  time_t prgtid;
  time_t aktueltid;
  struct tm *timeinfo;
  int xpos,ypos;
  int antalrec=0;
  xpos=150+20;
  ypos=180+20;
  // windows background
  renderer.AddTextureRect(27,_tvbar3, 100, 100, 850, 400,1,1,1,1);
  switch (configland) {
    case 0: temprgtxt = fmt::format("Channel");
            break;
    case 1: temprgtxt = fmt::format("Kanal");
            break;
    case 2: temprgtxt = fmt::format("Channel");
            break;
    case 3: temprgtxt = fmt::format("Channel");
            break;
    case 4: temprgtxt = fmt::format("Channel");
            break;
    default:
            temprgtxt = fmt::format("Channel");
  }
  renderer.AddText(&myfont,xpos, ypos+(1*18),temprgtxt,1,1,1,1);
  temprgtxt=fmt::format("{}", tvkanaler[vis_kanal_nr].programs[vis_program_nr].broadcastChannel);
  renderer.AddText(&myfont,xpos+120, ypos+(1*18),temprgtxt,1,1,1,1);
  switch (configland) {
    case 0: temprgtxt = fmt::format("Prg name");
            break;
    case 1: temprgtxt = fmt::format("Prg navn");
            break;
    case 2: temprgtxt = fmt::format("Prg name");
            break;
    case 3: temprgtxt = fmt::format("Prg name");
            break;
    case 4: temprgtxt = fmt::format("Prg name");
            break;
    default:    
            temprgtxt = fmt::format("Prg name ");
  }
  renderer.AddText(&myfont,xpos, ypos+(2*18),temprgtxt,1,1,1,1);
  temprgtxt=fmt::format("{}", tvkanaler[vis_kanal_nr].programs[vis_program_nr].title);
  renderer.AddText(&myfont,xpos+120, ypos+(2*18),temprgtxt,1,1,1,1);
  switch (configland) {
    case 0: temprgtxt = fmt::format("Start");
            break;
    case 1: temprgtxt = fmt::format("Start");
            break;
    case 2: temprgtxt = fmt::format("début");
            break;
    case 3: temprgtxt = fmt::format("Start");
            break;
    case 4: temprgtxt = fmt::format("Start");
            break;
    default:temprgtxt = fmt::format("Start");
  }
  renderer.AddText(&myfont,xpos, ypos+(3*18),temprgtxt,1,1,1,1);
  temprgtxt=fmt::format("{}", tvkanaler[vis_kanal_nr].programs[vis_program_nr].start);
  renderer.AddText(&myfont,xpos+120, ypos+(3*18),temprgtxt,1,1,1,1);
  switch (configland) {
    case 0: temprgtxt = fmt::format("Length");
            break;
    case 1: temprgtxt = fmt::format("Længde");
            break;
    case 2: temprgtxt = fmt::format("durée");
            break;
    case 3: temprgtxt = fmt::format("Programmlänge");
            break;
    case 4: temprgtxt = fmt::format("Length");
            break;
    default:
            temprgtxt = fmt::format("Length");
  }
  renderer.AddText(&myfont,xpos, ypos+(4*18),temprgtxt,1,1,1,1);
  temprgtxt=fmt::format("{} min", tvkanaler[vis_kanal_nr].programs[vis_program_nr].program_length_minuter);
  renderer.AddText(&myfont,xpos+120, ypos+(4*18),temprgtxt,1,1,1,1);
  switch (configland) {
    case 0: temprgtxt = fmt::format("Recorded");
            break;
    case 1: temprgtxt = fmt::format("Optaget");
            break;
    case 2: temprgtxt = fmt::format("Recorded");
            break;
    case 3: temprgtxt = fmt::format("Recorded");
            break;
    case 4: temprgtxt = fmt::format("Recorded");
            break;
    default:temprgtxt = fmt::format("Recorded");

  }
  renderer.AddText(&myfont,xpos, ypos+(5*18),temprgtxt,1,1,1,1);
  if (tvkanaler[vis_kanal_nr].programs[vis_program_nr].recorded) 
    renderer.AddText(&myfont,xpos+120, ypos+(5*18),"Yes",1,1,1,1);
  else 
    renderer.AddText(&myfont,xpos+120, ypos+(5*18),"No",1,1,1,1);
  switch (configland) {
    case 0: temprgtxt = fmt::format("Description : ");
            break;
    case 1: temprgtxt = fmt::format("Description : ");
            break;
    case 2: temprgtxt = fmt::format("Description : ");
            break;
    case 3: temprgtxt = fmt::format("Description : ");    
            break;
    case 4: temprgtxt = fmt::format("Description : ");
            break;
    default:temprgtxt = fmt::format("Description : ");
  }
  renderer.AddText(&myfont,xpos, ypos+(7*18),temprgtxt,1,1,1,1);
  temprgtxt=fmt::format("{}", tvkanaler[vis_kanal_nr].programs[vis_program_nr].description);
  drawLinesOfText(temprgtxt, xpos, ypos+(13*18), 1,60 ,5,1,false); 
  // show program info image
  GLuint program_texture=0;
  if ((program_texture==0) && (tvkanaler[vis_kanal_nr].programs[vis_program_nr].preview_image_texture==0)) {
    downloadImage(tvkanaler[vis_kanal_nr].programs[vis_program_nr].preview_image,"images/tv_icons/1.png");
    program_texture=loadTexture((char *) "images/tv_icons/1.png");
    tvkanaler[vis_kanal_nr].programs[vis_program_nr].preview_image_texture=program_texture;
  }
  if (tvkanaler[vis_kanal_nr].programs[vis_program_nr].preview_image_texture) 
    renderer.AddTextureRect(0,tvkanaler[vis_kanal_nr].programs[vis_program_nr].preview_image_texture, xpos+530, ypos-10, 230, 130,1,1,1,1);
  prgtid=tvkanaler[vis_kanal_nr].programs[vis_program_nr].starttime_unix;
  aktueltid=std::time(nullptr);
  timeinfo=localtime(&aktueltid);				// convert to localtime
  if (difftime(aktueltid,prgtid)<=0) {
    // std::cout << "record button " << "\n";
    renderer.AddTextureRect(41,_tvrecordbutton, 124, 120, 81, 81,1,1,1,1);     // 41 = record 
  }
}



// ****************************************************************************************
//
// construktor
//
// ****************************************************************************************

earlyrecorded::earlyrecorded() {
    for(int i=0;i<199;i++) {
        strcpy(this->programinfo[i].name,"ABC");
        strcpy(this->programinfo[i].dato,"2017-01-01");
        strcpy(this->programinfo[i].endtime,"");
    }
}

// ****************************************************************************************
//
// load recorded history not programs
//
// ****************************************************************************************

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


// ****************************************************************************************
//
// viser liste over record(historie) som har været optaget en gang.
// dvs det er ikke optagelser som ligger
//
// ****************************************************************************************

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
    // glBindTexture(GL_TEXTURE_2D, _tvrecordcancelbutton);
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




// ****************************************************************************************
//
// load recorded history not programs
//
// ****************************************************************************************



// ****************************************************************************************
//
// file reader
//
// ****************************************************************************************

char *read_stream(char *stream,size_t size, void *d) {
    char *c=fgets(stream,size,(FILE *) d);
    return c;
}

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


// ****************************************************************************************
//
// viser liste over tv programmer som skal optages.
//
// do not work
//
// ****************************************************************************************

void earlyrecorded::showtvreclist() {
  // background
  //glLoadIdentity();
  int i=0;
  static GLuint setuptvgraberback=0;
  glLoadIdentity();
  glEnable(GL_TEXTURE);
  glEnable(GL_BLEND);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glColor3f(1.0f, 1.0f, 1.0f);
  if (setuptvgraberback==0) setuptvgraberback  = loadTexture((char *) "/opt/mythtv-controller/tema3/images/setuptvgraberback.png");
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setuptvgraberback);                          // _texturemovieinfobox
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
    // glBindTexture(GL_TEXTURE_2D, _tvrecordcancelbutton);
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
