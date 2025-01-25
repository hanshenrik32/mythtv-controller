//
// Show movie overview
//
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glc.h>                     // glc true type font system
#include <mysql.h>                      // mysql stuf
#include <dirent.h>                     // dir functions
#include <linux/limits.h>
#include <sys/stat.h>                   // stats func for file/dir info struct
#include <string>
#include <ctime>
#define _UNICODE
#include <iostream>
#include <MediaInfo/MediaInfo.h>
#include <sstream>
#include <fmt/format.h>

#include "utility.h"
#include "myctrl_movie.h"
#include "readjpg.h"
#include "myth_vlcplayer.h"
#include "myctrl_music.h"
#include "myctrl_glprint.h"

using namespace std;
using namespace MediaInfoLib;

extern FILE *logfile;
extern char debuglogdata[1024];                                // used by log system

extern float configdefaultmoviefontsize;                       // font size
extern char configmoviepath[256];                              //
extern char configdefaultmoviepath[256];
extern char configbackend[];
extern const char *dbname;
extern char configmysqluser[256];                              // /mythtv/mysql access info
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern int configmythtvver;
extern int configxbmcver;
extern int screen_size;
extern unsigned int filmoversigt_antal;
extern int film_key_selected;
extern int film_select_iconnr;
extern GLuint _defaultdvdcover;
extern GLuint _defaultdvdcover2;
extern bool global_use_internal_music_loader_system;
const float textcolor_movie_oversigt[3]={0.8f,0.8f,0.8f};
extern GLint cur_avail_mem_kb;
extern unsigned int filmoversigt_antal;
extern GLuint _textureIdloading,_textureIdloading1;
extern bool vis_uv_meter;                                 // uv meter er igang med at blive vist
extern int movie_oversigt_loaded_nr;
extern bool movie_oversigt_gfx_loading;

// window info
extern int orgwinsizey;
extern int orgwinsizex;

extern GLuint _textureId9_askbox;



// ****************************************************************************************
//
// constructor
//
// ****************************************************************************************

film_oversigt_type::film_oversigt_type() {
    film_id=0;				// filmid i mysql
    film_title=new char[128];
    film_subtitle=new char[1024];	      // film info (sub title)
    film_filename=new char[256];	      // path to file to play
    film_coverfile=new char[256];	      // path to created (by convert) 3d gfx file
    film_adddate=new char[25];		      // format (åååå-mm-dd hh:mm:ss)
    rating=new char[100];		            // imdb rating
    format=new char[100];		            // movie format
    textureId=0;		                  	// texture id for 3D cover hvis der findes en cover til filmen
    frontcover=0;			                  // normal cover
    length=0;	                    			// film length
    year=0;				                      // aar som den udkom
    userrating=0;			                  // bruger rating
    film_imdbnr=new char[20];           //
    category_name=new char[128];	      // from mythtv film type (tal = database)
    genre=new char[200];		            //
    if (genre) strcpy(genre,"");
    if (format) strcpy(format,"");
    for(int n=0;n<20;n++) strcpy(cast[n],"");
    cover3d=false;
}

// ****************************************************************************************
// destructor
// ****************************************************************************************

film_oversigt_type::~film_oversigt_type() {
    delete [] film_title;
    delete [] film_subtitle;
    delete [] film_filename;
    delete [] film_coverfile;
    delete [] film_adddate;
    delete [] rating;
    delete [] film_imdbnr;
    delete [] category_name;
    delete [] genre;
    delete [] format;
}

// ****************************************************************************************
//
// gem texture filname
//
// ****************************************************************************************

void film_oversigt_type::settextureidfile(char *filename) {
    if (textureId==0) textureId=loadTexture ((char *) filename);
}


// ****************************************************************************************
//
// ****************************************************************************************

void film_oversigt_type::swap_film(film_oversigt_type *film1,film_oversigt_type *film2) {
    film_oversigt_type tempfilm;
    int i;
    if ((film1) && (film2)) {
        // gem org
        tempfilm.textureId = film1->textureId;
        tempfilm.film_id   = film1->film_id;
        tempfilm.length    = film1->length;
        tempfilm.year      = film1->year;
        tempfilm.userrating= film1->userrating;
        strcpy(tempfilm.rating , film1->rating);
        strcpy(tempfilm.film_imdbnr , film1->film_imdbnr);
        strcpy(tempfilm.film_title , film1->film_title);
        strcpy(tempfilm.film_coverfile , film1->film_coverfile);
        strcpy(tempfilm.film_filename , film1->film_filename);
        strcpy(tempfilm.film_adddate , film1->film_adddate);
        tempfilm.cover3d   =film1->cover3d;
        strcpy(tempfilm.film_subtitle , film1->film_subtitle);
        strcpy(tempfilm.category_name, film1->category_name);
        strcpy(tempfilm.genre , film1->genre);
        strcpy(tempfilm.format , film1->format);
        for(i=0;i<castlinieantal;i++) {
          strcpy(tempfilm.cast[i],film1->cast[i]);
        }
        // copy 2 to 1
        film1->textureId=film2->textureId;
        film1->film_id=film2->film_id;
        film1->length=film2->length;
        film1->year=film2->year;
        film1->userrating=film2->userrating;
        strcpy(film1->rating,film2->rating);
        strcpy(film1->film_imdbnr,film2->film_imdbnr);
        strcpy(film1->film_title,film2->film_title);
        strcpy(film1->film_coverfile,film2->film_coverfile);
        strcpy(film1->film_filename,film2->film_filename);
        strcpy(film1->film_adddate,film2->film_adddate);
        film1->cover3d=film2->cover3d;
        strcpy(film1->film_subtitle,film2->film_subtitle);
        strcpy(film1->category_name,film2->category_name);
        strcpy(film1->genre,film2->genre);
        strcpy(film1->format,film2->format);
        for(i=0;i<castlinieantal;i++) {
          strcpy(film1->cast[i],film2->cast[i]);
        }
        // copy 1(gemt) til 2
        film2->textureId=tempfilm.textureId;
        film2->film_id=tempfilm.film_id;
        film2->length=tempfilm.length;
        film2->year=tempfilm.year;
        film2->userrating=tempfilm.userrating;
        strcpy(film2->rating,tempfilm.rating);
        strcpy(film2->film_imdbnr,tempfilm.film_imdbnr);
        strcpy(film2->film_title,tempfilm.film_title);
        strcpy(film2->film_coverfile,tempfilm.film_coverfile);
        strcpy(film2->film_filename,tempfilm.film_filename);
        strcpy(film2->film_adddate,tempfilm.film_adddate);
        film2->cover3d=tempfilm.cover3d;
        strcpy(film2->film_subtitle,tempfilm.film_subtitle);
        strcpy(film2->category_name,tempfilm.category_name);
        strcpy(film2->genre,tempfilm.genre);
        strcpy(film2->format,tempfilm.format);
        for(i=0;i<castlinieantal;i++) {
          strcpy(film2->cast[i],tempfilm.cast[i]);
        }
    }
}



// ****************************************************************************************
// reset film info
//
// ****************************************************************************************

void film_oversigt_type::resetfilm() {
     // reset film oversigt
     strcpy(film_coverfile,"");
     strcpy(category_name,"");
     strcpy(format,"");
     strcpy(genre,"");
     textureId=0;                    // texture id for 3D cover hvis der findes en cover til filmen
}


// ****************************************************************************************
//
// get media info and update movie info
//
// ****************************************************************************************

bool film_oversigt_type::get_media_info_from_file(char *moviepath) {
  MediaInfo MI;
  unsigned long ll;
  string movfile;
  movfile=moviepath;
  std::wstring wstr=std::wstring(movfile.begin(),movfile.end());
  std::wstring path(wstr);
  if ((MI.Open(path)) && (movfile.substr(movfile.find_last_of(".") + 1) != "iso")) {
    std::wstring codec = MI.Get(Stream_General, 0, L"Format");
    std::wstring Duration = MI.Get(Stream_General, 0, L"Duration");             // in ms Duration
    std::wstring Title = MI.Get(Stream_General, 0, L"Title");                   // in ms Duration
    std::wstring Bitrate = MI.Get(Stream_General, 0, L"BitRate");               // in bitrate
    std::wstring Framerate = MI.Get(Stream_General, 0, L"FrameRate");           // in bitrate
    std::wstring Lenght = MI.Get(Stream_General, 0, L"Length");                 // in bitrate
    std::wstring Width = MI.Get(Stream_Video, 0, L"Width");
    std::wstring High = MI.Get(Stream_Video, 0, L"Height");
    std::wstring Format = MI.Get(Stream_General, 0, L"Format");   
    std::wstring Filsize = MI.Get(Stream_General, 0, L"FileSize");
    if (Framerate.size()>0) setFramerate(stoul(Framerate));
    if (Duration.size()>0) setfilmlength((stoul(Duration))*0.001/60);  // calc to min
    if (Bitrate.size()>0) setBitrate(stoul(Bitrate));
    if (Width.size()>0) setWidth(stoul(Width));
    if (High.size()>0) setHigh(stoul(High));
    string s( Format.begin(), Format.end());
    if (Format.size()>0) setFormat((char *) s.c_str());
    string fs(  Filsize.begin(),  Filsize.end());
    if (Filsize.size()>0) Flesize=atol(fs.c_str());
    MI.Close();
    return(1);
  } else return(0);
}



// *********************************************************************************************************************************************


// ****************************************************************************************
//
// constructor
//
// ****************************************************************************************

film_oversigt_typem::film_oversigt_typem(unsigned int antal) {
    filmoversigt=new film_oversigt_type[antal];
    filmoversigtsize=antal;
    film_oversigt_loaded=false;
    film_is_playing=false;
    film_is_pause=false;
    volume=100;
}

// ****************************************************************************************
//
// destructor
//
// ****************************************************************************************

film_oversigt_typem::~film_oversigt_typem() {
    if (filmoversigt) delete [] filmoversigt;
    filmoversigtsize=0;
}


// ****************************************************************************************
//
// reset all movies in array
//
// ****************************************************************************************

void film_oversigt_typem::resetallefilm() {
     for(unsigned int i=0;i<filmoversigtsize-1;i++) {
       filmoversigt[i].resetfilm();			// back cover
    }

}


// ****************************************************************************************
//
// default player
// stop playing movie
//
// ****************************************************************************************

void film_oversigt_typem::stopmovie() {
  if ((vlc_in_playing()) && (film_is_playing)) vlc_controller::stopmedia();
  film_is_playing=false;
}


// ****************************************************************************************
//
// vlc stop player
//
// ****************************************************************************************


void film_oversigt_typem::softstopmovie() {
  if ((vlc_in_playing()) && (film_is_playing)) vlc_controller::stopmedia();
  film_is_playing=false;
}



// ****************************************************************************************
//
// Play streams from path and update db abount nr of play
// 
//
// ****************************************************************************************

int film_oversigt_typem::playmovie(int nr) {
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  std::string updatedbplayed;
  char *database = (char *) "mythtvcontroller";
  conn=mysql_init(NULL);
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    updatedbplayed = fmt::v8::format("update videometadata set playcount=playcount+1 where filename like '{}'",this->filmoversigt[nr].getfilmfilename());
    mysql_query(conn,updatedbplayed.c_str());
    res = mysql_store_result(conn);
    mysql_close(conn);
  }
  film_is_playing=true;                                       // set play flag
  vlc_controller::playmedia(this->filmoversigt[nr].getfilmfilename());
  return(1);
}

// ****************************************************************************************
//
// pause movie
//
// ****************************************************************************************

void film_oversigt_typem::pausemovie() {
  vlc_controller::pause(1);
}

// ****************************************************************************************
//
// get position
//
// ****************************************************************************************

float film_oversigt_typem::getmovieposition() {
  return(vlc_controller::get_position());
}


// ****************************************************************************************
//
// ****************************************************************************************


void film_oversigt_typem::next_movie_chapther() {
  vlc_controller::pnext_chapter();
}


// ****************************************************************************************
//
// ****************************************************************************************

void film_oversigt_typem::prevous_movie_chapther() {
  vlc_controller::plast_chapter();
}


// ****************************************************************************************
//
// ****************************************************************************************

void film_oversigt_typem::volumeup() {
  if (volume<100) volume+=10;
  vlc_controller::volume_up(volume);
}


// ****************************************************************************************
//
// ****************************************************************************************

void film_oversigt_typem::volumedown() {
  if (volume>0) volume-=10;
  vlc_controller::volume_down(volume);
}


// ****************************************************************************************
//
// ****************************************************************************************

void film_oversigt_typem::setcolume(int volume) {
  //if ((volume>=0) && (volume<=100)) this->volume=volume;
  vlc_controller::setvolume(volume);
}


// sort movies after type

//type=0 by movie name
//type=1 by id
//type=2 by add date
//default by movie name

// ****************************************************************************************
//
// ****************************************************************************************

void film_oversigt_typem::sortfilm(int type) {
  bool swap=false;
  unsigned int i;
  unsigned int antal=get_film_antal();
  switch (type) {
      case 0:
        do {
          swap=false;
          for(i=0;i<antal-1;i++) {
            if (strcmp(filmoversigt[i].getfilmtitle(),filmoversigt[i+1].getfilmtitle())>0) {
              filmoversigt->swap_film(&filmoversigt[i],&filmoversigt[i+1]);
              swap=true;
            }
          }
        } while(swap);
        break;
      case 1:
        do {
          swap=false;
          for(i=0;i<antal-1;i++) {
            if (filmoversigt[i].getfilmnr()>filmoversigt[i+1].getfilmnr()) {
              filmoversigt->swap_film(&filmoversigt[i],&filmoversigt[i+1]);
              swap=true;
            }
          }
        } while(swap);
        break;
      case 2:
        do {
            swap=false;
            for(i=0;i<antal-1;i++) {
              if (strcmp(filmoversigt[i].getfilm_adddate(),filmoversigt[i+1].getfilm_adddate())>0) {
                filmoversigt->swap_film(&filmoversigt[i],&filmoversigt[i+1]);
                swap=true;
              }
            }
        } while(swap);
        break;
      default:
        do {
          swap=false;
          for(i=0;i<antal-1;i++) {
            if (strcmp(filmoversigt[i].getfilmtitle(),filmoversigt[i+1].getfilmtitle())>0) {
              filmoversigt->swap_film(&filmoversigt[i],&filmoversigt[i+1]);
              swap=true;
            }
          }
        } while(swap);
    }
}



// ****************************************************************************************
//
// load dvd gfx files covers in
//
// ****************************************************************************************


int film_oversigt_typem::load_film_dvcovers() {
    unsigned int i=0;					// hent alle 3d film covers
    char tmpfilename[200];
    while (i<FILM_OVERSIGT_TYPE_SIZE) {
      strcpy(tmpfilename,this->filmoversigt[i].getfilmcoverfile());
      if ((file_exists(tmpfilename)) && (this->filmoversigt[i].gettextureid()==0)) {
        this->filmoversigt[i].settextureidfile(tmpfilename);
      }
      i++;
    }
    return(1);
}





// ****************************************************************************************
//
// ****************************************************************************************

void hentgenre(struct film_oversigt_type *film,unsigned int refnr) {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    std::string sqlselect;
    unsigned int i;
    // mysql stuf
    // sprintf(sqlselect,"select idvideo,videogenre.genre from videometadatagenre left join videogenre on videometadatagenre.idgenre=videogenre.intid where idvideo=%d",refnr);
    sqlselect = fmt::v8::format("select idvideo,videogenre.genre from videometadatagenre left join videogenre on videometadatagenre.idgenre=videogenre.intid where idvideo={}",refnr);
    char *database = (char *) "mythconverg";
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect.c_str());
    res = mysql_store_result(conn);
    i=0;
    if (res) {
      while ((res) && (((row = mysql_fetch_row(res)) != NULL) && (i<FILM_OVERSIGT_TYPE_SIZE))) {
        if (strlen(film->genre)<160) {
          if (i>0) strcat(film->genre,",");
          strcat(film->genre,row[1]);
          i++;
        }
      }
      mysql_close(conn);
    }
}


// ****************************************************************************************
//
// ****************************************************************************************

void hentcast(struct film_oversigt_type *film, unsigned int refnr) {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    std::string sqlselect;
    int i=0;
    // mysql stuf
    // sprintf(sqlselect,"select videocast.cast from videometadatacast left join videocast on videometadatacast.idcast=videocast.intid where idvideo=%d",refnr);
    sqlselect = fmt::v8::format("select videocast.cast from videometadatacast left join videocast on videometadatacast.idcast=videocast.intid where idvideo={}",refnr);
    // mysql stuf
    char *database = (char *) "mythconverg";
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect.c_str());
    res = mysql_store_result(conn);
    if (res) {
      while ((res) && (((row = mysql_fetch_row(res)) != NULL) && (i<19))) {
        strcpy(film->cast[i],row[0]);		// hent crew
        i++;
      }
      mysql_close(conn);
    }
}



// ****************************************************************************************
//
// hent antal elementer i directory dirname
//
// ****************************************************************************************

int countEntriesInDir(const char* dirname) {
    int n=0;
    dirent* d;
    DIR* dir = opendir(dirname);
    if (dir == NULL) return 0;
    while((d = readdir(dir))!=NULL) n++;
    closedir(dir);
    return n;
}


// ****************************************************************************************
//
// overloaded function in .h file
// hent film oversigt
// create if not exist (mythtv/internal)
//
// ****************************************************************************************

int film_oversigt_typem::opdatere_film_oversigt(void) {
    char convert_command[2000];
    char convert_command1[2000];
    char sqlselect[4000];
    std::string ssqlselect;
    std::string ssqlselect1;
    char sqlselect1[4000];
    char mainsqlselect[2000];
    std::string mainsqlselect1;
    char temptxt[2000];    
    unsigned int recnr=0;
    unsigned int i;
    int filmantal=0;
    FILE *filhandle;
    char resl[200];
    char database[200];
    bool allokay=false;
    bool dbexist=false;
    DIR *dirp=NULL;
    DIR *subdirp=NULL;
    struct dirent *moviefil=NULL;
    struct dirent *submoviefil=NULL;
    char *ext;
    char filename[256];
    char userhomedir[2000];
    char movietitle[200];
    string moviepath;                             // movie path
    // char moviepathcheck[2000];
    string moviepathcheck;
    char statfilename[2000];
    bool fundet;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_RES *res1;
    MYSQL_ROW row;
    MYSQL_ROW row1;
    struct stat statbuffer;
    bool film_ok=false;
    bool film_cover_ok=false;
    int movieyear=2000;
    float movieuserrating=1.0f;
    int movielength=0;
    unsigned int del_rec_nr;
    bool is_db_updated_then_do_clean_up=false;
    long delrecid;
    unsigned int filepathsize;
    char *file_to_check_path;
    string coverfile;
    string dato;
    string thismoviepathdir;
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);

    // mysql stuf
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    strcpy(database,dbname);
    int checkdirexist=0;
    write_logfile(logfile,(char *) "Opdatere Film oversigt fra db :");
    sprintf(mainsqlselect,"SELECT videometadata.intid,title,filename,coverfile,length,year,rating,userrating,plot,inetref,videocategory.category from videometadata left join videocategory on videometadata.category=videocategory.intid and browse=1 order by category,title limit %d",FILM_OVERSIGT_TYPE_SIZE-1);
//    sprintf(sqlselect,"SELECT videometadata.intid,title,filename,coverfile,length,year,rating,userrating,plot,inetref,videocategory.category,videogenre.genre from videogenre,videometadatagenre,videometadata left join videocategory on videometadata.category=videocategory.intid where videometadatagenre.idvideo=videometadata.intid and browse=1 group by idvideo order by category,title limit %d",FILM_OVERSIGT_TYPE_SIZE-1);
    conn=mysql_init(NULL);
    if (conn) {
      allokay=true;
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      // test musik table exist
      // sprintf(sqlselect,"select table_schema as database_name,count(*) as tables from information_schema.tables where table_type = 'BASE TABLE' and table_schema not in ('information_schema', 'sys', 'performance_schema', 'mysql') group by table_schema order by table_schema");
      ssqlselect = "select table_schema as database_name,count(*) as tables from information_schema.tables where table_type = 'BASE TABLE' and table_schema not in ('information_schema', 'sys', 'performance_schema', 'mysql') group by table_schema order by table_schema";
      mysql_query(conn,ssqlselect.c_str());
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          if (atoi(row[1])==32) dbexist=true; else dbexist=false;
        }
      } else dbexist=false;
      // create databases/tables if not exist
      // needed by movie loader
      if (!(dbexist)) {
        ssqlselect = "create table IF NOT EXISTS videometadata(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, title varchar(120), subtitle text, tagline varchar(255), director varchar(128), studio varchar(128), plot text, rating varchar(128), inetref  varchar(255), collectionref int, homepage text,year int, releasedate date, userrating float, length int, playcount int, season int, episode int,showlevel int, filename text,hash varchar(128), coverfile text, childid int, browse int, watched int, processed int, playcommand varchar(255), category int, trailer text,host text, screenshot text, banner text, fanart text,insertdate timestamp, contenttype int, bitrate int , width int , high int, fsize int, fformat varchar(150))";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "create table IF NOT EXISTS videocategory(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, category varchar(128))";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "create table IF NOT EXISTS videogenre(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, genre varchar(128))";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "create table IF NOT EXISTS videocountry(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, country varchar(128))";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "create table IF NOT EXISTS videocollection(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, title varchar(256), contenttype int, plot text,network varchar(128), collectionref varchar(128), certification varchar(128), genre varchar(128),releasedate date, language varchar(10),status varchar(64), rating float, ratingcount int, runtime int, banner text,fanart text,coverart text)";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "create table IF NOT EXISTS videopathinfo(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, path text, contenttype int, collectionref int,recurse  int)";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "create table IF NOT EXISTS videotypes(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, extension varchar(128),playcommand varchar(255), f_ignore int,  use_default int)";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "insert into videotypes values (0,'txt','',1,0)";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "insert into videotypes values (0,'log','',1,0)";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "insert into videotypes values (0,'mpg','',0,0)";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "insert into videotypes values (0,'avi','',0,1)";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "insert into videotypes values (0,'vob','',0,0)";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "insert into videotypes values (0,'mpeg','',0,0)";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "insert into videotypes values (0,'VIDEO_TS','',0,0)";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "insert into videotypes values (0,'iso','',0,0)";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
        ssqlselect = "insert into videotypes values (0,'img','',0,0)";
        mysql_query(conn,ssqlselect.c_str());
        res = mysql_store_result(conn);
      }
      //configdefaultmoviepath
      dirp=opendir(configmoviepath);                                                          // "~/.kodi/userdata/Database/");
      //dirp=opendir(configdefaultmoviepath);                                                          // "~/.kodi/userdata/Database/");
      if (dirp==NULL) {
        printf("No %s dir found \nOpen dir error \n",userhomedir);
        //exit(0);
      }
      // loop dir and update movie db
      // and find kodi db version
      if (!(dbexist)) {      
        conn=mysql_init(NULL);
        if ((conn) && (dirp)) {
          mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass,database, 0, NULL, 0);
          // loop files in dirp
          while (moviefil = readdir(dirp)) {
            t = std::time(0);   // get time now
            now = std::localtime(&t);
            if ((strcmp(moviefil->d_name,".")!=0) && (strcmp(moviefil->d_name,"..")!=0)) {          
              // save path for later use in db insert
              thismoviepathdir = configmoviepath;            
              thismoviepathdir = thismoviepathdir + "/";
              thismoviepathdir = thismoviepathdir + moviefil->d_name;
              ext = strrchr(moviefil->d_name, '.');
              if (ext) {
                strcpy(filename,moviefil->d_name);
              }
              // get title from dir/file name
              strcpy(movietitle,filename);
              strcpy(statfilename,configmoviepath);
              strcat(statfilename,"/");
              strcat(statfilename,moviefil->d_name);
              if (stat(statfilename,&statbuffer)==-1) {
                perror("stat");
                exit(EXIT_FAILURE);
              }
              // if dir
              if ((statbuffer.st_mode & S_IFMT)==S_IFDIR) {
                // it a dir opendir and find files
                subdirp=opendir(statfilename);
                film_ok=false;
                while (submoviefil = readdir(subdirp)) {
                  ext = strrchr(submoviefil->d_name, '.');
                  if (ext) {
                    film_ok=false;
                    if (strcmp(ext,".avi")==0) film_ok=true;
                    if (strcmp(ext,".mp4")==0) film_ok=true;
                    if (strcmp(ext,".mkv")==0) film_ok=true;
                    if (strcmp(ext,".mov")==0) film_ok=true;
                    if (strcmp(ext,".wmv")==0) film_ok=true;
                    if (strcmp(ext,".mpeg")==0) film_ok=true;
                    if (strcmp(ext,".ogv")==0) film_ok=true;
                    if (strcmp(ext,".iso")==0) film_ok=true;
                    if (strcmp(ext,".ISO")==0) film_ok=true;
                    if (strcmp(ext,".ISO")==0) film_ok=true;
                    if (film_ok) {
                      // name title
                      strcpy(movietitle,submoviefil->d_name);
                      ext = strrchr(movietitle, '.');
                      if (ext) {
                        *ext='\0';
                      }
                      // check for '\n' in the end of the string and remove it
                      if (movietitle[strlen(movietitle)-2]=='\n') movietitle[strlen(movietitle)-2]=0;
                      
                      moviepath=moviefil->d_name;
                      moviepath=moviepath + "/";
                      moviepath=submoviefil->d_name;          
                      moviepathcheck=configmoviepath;
                      if (moviepathcheck.back()!='/') moviepathcheck=moviepathcheck + "/";

                      moviepathcheck = moviepathcheck + moviefil->d_name;                     // dir name 
                      moviepathcheck = moviepathcheck + "/";
                      moviepathcheck = moviepathcheck + submoviefil->d_name;                     // get full filename
                      // get cover file from movie file name
                      coverfile=configmoviepath;
                      if (coverfile.back()!='/') coverfile=coverfile + "/";
                      coverfile=coverfile + moviefil->d_name;                        // path
                      coverfile=coverfile + "/";
                      coverfile=coverfile + submoviefil->d_name;                      // add file name
                      size_t lastindex = coverfile.find_last_of("."); 
                      string tmpcovername = coverfile.substr(0, lastindex); 
                      coverfile=tmpcovername;
                      coverfile=coverfile + ".jpg";
                      if (!(file_exists(coverfile.c_str()))) {
                        // cover file name do not exist do make name as cover.jpg else no cover
                        coverfile=configmoviepath;
                        if (coverfile.back()!='/') coverfile=coverfile + "/";
                        coverfile=coverfile + moviefil->d_name;                        // path
                        coverfile=coverfile + "/";
                        coverfile=coverfile + "cover.jpg";
                        if (!(file_exists(coverfile.c_str()))) {
                          // sprintf(sqlselect,"No cover file for %s ",coverfile.c_str());
                          // printf("%s\n",sqlselect);
                          coverfile="";
                        }
                      }
                      fundet=false;
                      del_rec_nr=0;
                      sprintf(sqlselect,"select intid from videometadata where filename like '%%");
                      sprintf(temptxt,"%s",submoviefil->d_name);
                      strcat(sqlselect,temptxt);
                      sprintf(temptxt,"%%' limit 1");
                      strcat(sqlselect,temptxt);
                      mysql_query(conn,sqlselect);
                      res = mysql_store_result(conn);
                      if (res) {
                        while ((row = mysql_fetch_row(res)) != NULL) {
                          fundet=true;
                          del_rec_nr=atoi(row[0]);
                        }
                      }
                      // write debug log
                      if (fundet) sprintf(debuglogdata,"Checking/Replace movietitle '%s'",movietitle);
                      else sprintf(debuglogdata,"Checking/Insert movietitle '%s'",movietitle);
                      write_logfile(logfile,(char *) debuglogdata);
                      // check if record exist (video file exist)
                      if ((fundet) && (del_rec_nr)) {
                        if (!(file_exists(moviepathcheck.c_str()))) {
                          ssqlselect = fmt::v8::format("delete from videometadata where intid={} limit 1",del_rec_nr);
                          mysql_query(conn,ssqlselect.c_str());
                          res = mysql_store_result(conn);
                        }
                      }
                      if (!(fundet)) {
                        // update movie info
                        dato=to_string(now->tm_year + 1900);
                        dato=dato + "-";
                        dato=dato + to_string(now->tm_mon + 1);
                        dato=dato + "-";
                        dato=dato + to_string(now->tm_mday);
                        dato=dato + " ";
                        dato=dato + to_string(now->tm_hour);
                        dato=dato + ":";
                        dato=dato + to_string(now->tm_min);
                        dato=dato + ":";
                        dato=dato + to_string(now->tm_sec);
                        // sprintf(sqlselect,"insert into videometadata(intid , title, subtitle, tagline, director, studio, plot, rating, inetref, collectionref, homepage, year, releasedate, userrating, length, playcount, season, episode,showlevel, filename,hash, coverfile, childid, browse, watched, processed, playcommand, category, trailer, host, screenshot, banner, fanart,insertdate, contenttype) values (0,'%s','%s','','director','','%s','','%s',0,'',%d,'2016-12-31',%2.5f,%d,0,0,0,0,'%s','hash','%s',0,0,0,0,'playcommand',0,'','','','','','%s',0)", movietitle,"moviesubtitle","movieplot","movieimdb",movieyear,movieuserrating,movielength ,moviepathcheck.c_str(),coverfile.c_str(),dato.c_str());
                        ssqlselect = fmt::v8::format("insert into videometadata(intid , title, subtitle, tagline, director, studio, plot, rating, inetref, collectionref, homepage, year, releasedate, userrating, length, playcount, season, episode,showlevel, filename,hash, coverfile, childid, browse, watched, processed, playcommand, category, trailer, host, screenshot, banner, fanart,insertdate, contenttype) values (0,'{}','{}','','director','','{}','','{}',0,'',{},'2016-12-31',{:2.5f},{},0,0,0,0,'{}','hash','{}',0,0,0,0,'playcommand',0,'','','','','','{}',0)",movietitle,"moviesubtitle","movieplot","movieimdb",movieyear,movieuserrating,movielength ,moviepathcheck.c_str(),coverfile.c_str(),dato.c_str());
                        recnr++;
                        mysql_query(conn,"set NAMES 'utf8'");
                        res = mysql_store_result(conn);
                        mysql_query(conn,ssqlselect.c_str());
                        res = mysql_store_result(conn);
                        if (mysql_error(conn)) {
                          write_logfile(logfile,(char *) "Mysql error 'insert into videometadata'");
                        }
                        // sprintf(sqlselect,"insert into videopathinfo(intid, path, contenttype, collectionref , recurse) values (0,'%s',0,0,0)",thismoviepathdir.c_str());
                        ssqlselect = fmt::v8::format("insert into videopathinfo(intid, path, contenttype, collectionref , recurse) values (0,'{}',0,0,0)",thismoviepathdir.c_str());
                        mysql_query(conn,ssqlselect.c_str());
                        res = mysql_store_result(conn);
                        if (mysql_error(conn)) {
                          write_logfile(logfile,(char *) "Mysql error 'insert into videpathinfo'");
                        }
                      }
                    }
                  }
                }
              } else if ((statbuffer.st_mode & S_IFMT)==S_IFREG) {                        // if file
                // if file get ext of filename
                ext = strrchr(moviefil->d_name, '.');
                if (ext) {
                  film_ok=false;
                  if (strcmp(ext,".avi")==0) film_ok=true;
                  if (strcmp(ext,".mp4")==0) film_ok=true;
                  if (strcmp(ext,".mkv")==0) film_ok=true;
                  if (strcmp(ext,".iso")==0) film_ok=true;
                  if (strcmp(ext,".ISO")==0) film_ok=true;
                  if (film_ok) {
                    strcpy(movietitle,moviefil->d_name);
                    // name title
                    ext = strrchr(movietitle, '.');
                    if (ext) {
                      *ext='\0';
                    }
                    // make cover file name if exist. (movie file name (no ext) + jpg)
                    coverfile=configmoviepath;
                    if (coverfile.back()!='/') coverfile=coverfile + "/";
                    coverfile=coverfile + moviefil->d_name;
                    size_t lastindex = coverfile.find_last_of("."); 
                    string tmpcovername = coverfile.substr(0, lastindex); 
                    coverfile=tmpcovername;                                           // get name - ext
                    coverfile=coverfile + ".jpg";
                    if (!(file_exists(coverfile.c_str()))) coverfile="";
                    // strcpy(moviepath1,moviefil->d_name);                         // get full filename
                    moviepath=moviefil->d_name;
                    fundet=false;
                    del_rec_nr=0;
                    sprintf(sqlselect,"select intid from videometadata where title like '%%");
                    sprintf(temptxt,"%s",movietitle);
                    strcat(sqlselect,temptxt);
                    sprintf(temptxt,"%%' and filename like '%%");
                    strcat(sqlselect,temptxt);
                    sprintf(temptxt,"%s",moviepath.c_str());
                    strcat(sqlselect,temptxt);
                    sprintf(temptxt,"%%' limit 1");
                    strcat(sqlselect,temptxt);
                    mysql_query(conn,sqlselect);
                    res = mysql_store_result(conn);
                    if (res) {
                      while ((row = mysql_fetch_row(res)) != NULL) {
                        fundet=true;
                        del_rec_nr=atoi(row[0]);
                      }
                    }
                    // write debug log
                    if (fundet) sprintf(debuglogdata,"Checking/Replace movietitle %s \n",movietitle);
                    else sprintf(debuglogdata,"Insert movietitle %s \n",movietitle);
                    write_logfile(logfile,(char *) debuglogdata);
                    // findes filmen i db i forvejen så slet den og opret den igen
                    // ellers bare opret den
                    // dette skal gøres hvis dir eller fil navn ændre sig
                    if ((fundet) && (del_rec_nr)) {
                      if (!(file_exists(moviepathcheck.c_str()))) {
                        // sprintf(sqlselect,"delete from videometadata where intid=%d limit 1",del_rec_nr);
                        ssqlselect = fmt::v8::format("delete from videometadata where intid={} limit 1",del_rec_nr);                        
                        mysql_query(conn,ssqlselect.c_str());
                        res = mysql_store_result(conn);
                      }
                    }
                    // add to db again 
                    if (!(fundet)) {
                      dato=to_string(now->tm_year + 1900);
                      dato=dato + "-";
                      dato=dato + to_string(now->tm_mon + 1);
                      dato=dato + "-";
                      dato=dato + to_string(now->tm_mday);
                      dato=dato + " ";
                      dato=dato + to_string(now->tm_hour);
                      dato=dato + ":";
                      dato=dato + to_string(now->tm_min);
                      dato=dato + ":";
                      dato=dato + to_string(now->tm_sec);
                      // sprintf(sqlselect,"insert into videometadata(intid , title, subtitle, tagline, director, studio, plot, rating, inetref, collectionref, homepage, year, releasedate, userrating, length, playcount, season, episode,showlevel, filename,hash, coverfile, childid, browse, watched, processed, playcommand, category, trailer, host, screenshot, banner, fanart,insertdate, contenttype, bitrate , width , high, fsize) values (0,'%s','%s','','director','','%s','','%s',0,'',%d,'2016-12-31',%2.5f,%d,0,0,0,0,'%s','hash','%s',0,0,0,0,'playcommand',0,'','','','','','%s',0,%d,%d,%d,%d,%d)", movietitle,"moviesubtitle","movieplot","movieimdb",movieyear,movieuserrating,movielength ,moviepath.c_str(),coverfile.c_str(),dato.c_str(),0,0,0,0,"");
                      ssqlselect = fmt::v8::format("insert into videometadata(intid , title, subtitle, tagline, director, studio, plot, rating, inetref, collectionref, homepage, year, releasedate, userrating, length, playcount, season, episode,showlevel, filename,hash, coverfile, childid, browse, watched, processed, playcommand, category, trailer, host, screenshot, banner, fanart,insertdate, contenttype, bitrate , width , high, fsize) values (0,'{}','{}','','director','','{}','','{}',0,'',{},'2016-12-31',{:2.5f},{},0,0,0,0,'{}','hash','{}',0,0,0,0,'playcommand',0,'','','','','','{}',0,{},{},{},{},{})",movietitle,"moviesubtitle","movieplot","movieimdb",movieyear,movieuserrating,movielength ,moviepath.c_str(),coverfile.c_str(),dato.c_str(),0,0,0,0,"");
                      recnr++;
                      fprintf(stderr, "Movie db update %2d title %s \n",recnr,movietitle);
                      //mysql_query(conn,"set NAMES 'utf8'");
                      //res = mysql_store_result(conn);
                      mysql_query(conn,ssqlselect.c_str());
                      res = mysql_store_result(conn);
                      if (mysql_error(conn)) {
                        string tmplog;
                        write_logfile(logfile,(char *) "Mysql error.");
                        tmplog="Mysql error :";
                        tmplog=tmplog + mysql_error(conn);
                        write_logfile(logfile,(char *) tmplog.c_str());
                        // printf("%s\n",mysql_error(conn));
                        // exit(0);
                      }
                    }
                  }
                } else {
                  // not file
                }
              }
            }
          }
        }
      }
      mysql_close(conn);
    }
    // Connect to database
    // fill array from db just created
    //
    conn=mysql_init(NULL);
    if (conn) {
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);      
      // sprintf(mainsqlselect,"SELECT videometadata.intid,title,filename,coverfile,length,year,rating,userrating,plot,inetref,videocategory.category,bitrate,width,high,fsize,fformat,subtitle from videometadata left join videocategory on videometadata.category=videocategory.intid and browse=1 order by category,title limit %d",FILM_OVERSIGT_TYPE_SIZE-1);
      mainsqlselect1 = fmt::v8::format("SELECT videometadata.intid,title,filename,coverfile,length,year,rating,userrating,plot,inetref,videocategory.category,bitrate,width,high,fsize,fformat,subtitle from videometadata left join videocategory on videometadata.category=videocategory.intid and browse=1 order by category,title limit {}",FILM_OVERSIGT_TYPE_SIZE-1);
      mysql_query(conn,mainsqlselect1.c_str());
      res = mysql_store_result(conn);
      i=0;
      filhandle=fopen("filmcover_gfx.log","w");
      if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (i<FILM_OVERSIGT_TYPE_SIZE)) {
          is_db_updated_then_do_clean_up=true;
          filmantal++;

          /* Felt nr from db call
          0 SELECT videometadata.intid 
          1 title
          2 filename
          3 coverfile
          4 length,
          5 year,
          6 rating,
          7 userrating,
          8 plot,
          9 inetref,
          10 videocategory.category,
          11 bitrate,
          12 width,
          13 high,
          14 fsize,
          15 fformat
          16 sub_title
          */

          filmoversigt[i].setfilmid(atoi(row[0]));
          filmoversigt[i].setfilmtitle(row[1]);
          hentcast(&filmoversigt[i],filmoversigt[i].getfilmid());
          hentgenre(&filmoversigt[i],filmoversigt[i].getfilmid());
          if (row[8]) {							                                  // hent film beskrivelse
            filmoversigt[i].setfilmsubtitle(row[8]);
          } else filmoversigt[i].setfilmsubtitle((char *) "");
          filmoversigt[i].setfilmfilename(row[2]);	                 // fil navn på film
          filmoversigt[i].setfilmcoverfile(row[3]);				           // fil navn på cover fil
          // filmoversigt[i].setfilmlength(atoi(row[4]));			         // film længde i unsigned int
          filmoversigt[i].setfilmaar(atoi(row[5]));
          filmoversigt[i].setimdbfilmrating(row[6]);	               // rating hmm imdb ?
          filmoversigt[i].setfilmrating(atoi(row[7]));          		 // user rating
          filmoversigt[i].setfilmimdbnummer(row[9]);
          if (row[10]) {							                              // category (type text)
            strncpy(filmoversigt[i].category_name,row[10],127);   // get name from db
          } else strcpy(filmoversigt[i].category_name,"");
          if (strcmp((char *) filmoversigt[i].getfilmcoverfile(),"No Cover")==0) filmoversigt[i].setfilmcoverfile((char *)"");
            // check file
          if (row[11]) {
            if (strlen(row[11])>0) filmoversigt[i].setBitrate(atoi(row[11]));
            if (strlen(row[12])>0) filmoversigt[i].setWidth(atoi(row[12]));
            if (strlen(row[13])>0) filmoversigt[i].setHigh(atoi(row[13]));
            if (strlen(row[14])>0) filmoversigt[i].setSize(atoll(row[14]));
            if (strlen(row[15])>0) filmoversigt[i].setFormat(row[15]);
          }
          if (row[16]) filmoversigt[i].setfilmsubtitle(row[16]);
          // if movie do not have any format check it and update
          if (strcmp(filmoversigt[i].getFormat(),"")==0) {
            if (filmoversigt[i].get_media_info_from_file((char *) row[2])) {
              // sprintf(sqlselect1,"update videometadata set length=%lu,bitrate=%d,width=%d,high=%d,fsize=%d,fformat='%s' where filename like '%s'",filmoversigt[i].getfilmlength(),filmoversigt[i].getBitrate(), filmoversigt[i].getWidth() ,filmoversigt[i].getHigh(), filmoversigt[i].getSize(), filmoversigt[i].getFormat() ,row[2]);
              ssqlselect1 = fmt::v8::format("update videometadata set length={},bitrate={},width={},high={},fsize={},fformat='{}' where filename like '{}'",filmoversigt[i].getfilmlength(),filmoversigt[i].getBitrate(), filmoversigt[i].getWidth() ,filmoversigt[i].getHigh(), filmoversigt[i].getSize(), filmoversigt[i].getFormat() ,row[2]);
              printf("update movie %s \n",row[1]);
              mysql_query(conn,ssqlselect1.c_str());
              res1 = mysql_store_result(conn);
            }
          }
	        i++;
        }
      }
    } else {
      write_logfile(logfile,(char *) "Can not connect to mysql server.");
    }
    if (filhandle) {
      fputs("No db avable\n",filhandle);
      fclose(filhandle);							// close log file again
    }
    // check if movied is deleted in dir
    if (is_db_updated_then_do_clean_up) {
      sprintf(mainsqlselect,"SELECT videometadata.intid,filename from videometadata");
      conn=mysql_init(NULL);
      if (conn) {
        filhandle=0;
        filhandle=fopen("movie_cleanup_info.log","r+");
        // if file not exist crete first time
        if (filhandle==NULL) filhandle=fopen("movie_cleanup_info.log","w+");
        mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,mainsqlselect);
        res = mysql_store_result(conn);
        if (res) {
          while ((res) && ((row = mysql_fetch_row(res)) != NULL))  {
            filepathsize=strlen(configmoviepath)+strlen(row[1])+1+1;            // + NULL + /
            file_to_check_path=new char[filepathsize];
            if (file_to_check_path) {
              strcpy(file_to_check_path,configmoviepath);                       // make path to file
              strcat(file_to_check_path,"/");
              strcat(file_to_check_path,row[1]);
              if (strlen(file_to_check_path)>1) {
                if (!(file_exists(file_to_check_path))) {
                  fputs("Movie deleted filename ",filhandle);
                  fputs(row[1],filhandle);                                      // write to log file
                  fputs("\n",filhandle);
                  delrecid=atol(row[0]);
                  // delete from db
                  // sprintf(sqlselect,"delete from videometadata where intid=%ld limit 1",delrecid);
                  ssqlselect = fmt::v8::format("delete from videometadata where intid={} limit 1",delrecid);
                  mysql_query(conn,ssqlselect.c_str());
                  res = mysql_store_result(conn);
                }
              }
              delete [] file_to_check_path;                                     // clean up
            }
          }
        }
        if (filhandle) fclose(filhandle);
      } else {
        write_logfile(logfile,(char *) "Can not connect to mysql server.");
      }
    }
    if (filmantal>0) this->filmoversigt_antal=filmantal-1; else this->filmoversigt_antal=0;
    if (conn) mysql_close(conn);
    return(filmantal);
}



// ****************************************************************************************
//
// overloaded function in .h file
// hent film oversigt
// create if not exist (mythtv/internal)
//
// ****************************************************************************************

int film_oversigt_typem::opdatere_film_oversigt(char *movietitle) {
    // char sqlselect[4000];
    std::string sqlselect;
    char mainsqlselect[2000];
    unsigned int i;
    int filmantal=0;
    char database[200];
    bool dbexist=false;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    // mysql stuf
    strcpy(database,dbname);
    strcpy(mainsqlselect,"SELECT videometadata.intid,title,filename,coverfile,length,year,rating,userrating,plot,inetref from videometadata where title like '%");
    strcat(mainsqlselect,movietitle);
    strcat(mainsqlselect,"%' order by category,title limit 100"); // ,FILM_OVERSIGT_TYPE_SIZE-1);
    conn=mysql_init(NULL);
    if (conn) {
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      // test table exist
      sqlselect = "SHOW TABLES LIKE 'videometadata'";
      mysql_query(conn,sqlselect.c_str());
      res = mysql_store_result(conn);
      if (res) {
        while ((res) && ((row = mysql_fetch_row(res)) != NULL)) {
          dbexist=true;
        }
      } else dbexist=false;
      if (dbexist) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,mainsqlselect);
        res = mysql_store_result(conn);
        i=0;
        if (res) {
          while ((res) && (((row = mysql_fetch_row(res)) != NULL) && (i<FILM_OVERSIGT_TYPE_SIZE))) {
            filmantal++;
            filmoversigt[i].setfilmid(atoi(row[0]));
            filmoversigt[i].setfilmtitle(row[1]);
            hentcast(&filmoversigt[i],filmoversigt[i].getfilmid());
            hentgenre(&filmoversigt[i],filmoversigt[i].getfilmid());
            if (row[8]) {							                                 // hent film beskrivelse
              filmoversigt[i].setfilmsubtitle(row[8]);
            } else filmoversigt[i].setfilmsubtitle((char *) "");
            filmoversigt[i].setfilmfilename(row[2]);	                 // fil navn på film
            filmoversigt[i].setfilmcoverfile(row[3]);				           // fil navn på cover fil
            filmoversigt[i].setfilmlength(atoi(row[4]));			         // film længde i unsigned int
            filmoversigt[i].setfilmaar(atoi(row[5]));
            filmoversigt[i].setimdbfilmrating(row[6]);	               // rating hmm imdb ?
            filmoversigt[i].setfilmrating(atoi(row[7]));          		 // user rating
            filmoversigt[i].setfilmimdbnummer(row[9]);
            if (row[10]) {							                              // category (type text)
              strncpy(filmoversigt[i].category_name,row[10],127);   // get name from db
            } else strcpy(filmoversigt[i].category_name,"");
            i++;
          }
        }
      }
      mysql_close(conn);
    }
    if (filmantal>0) this->filmoversigt_antal=filmantal; else this->filmoversigt_antal=0;
    return(filmantal);
}





// ****************************************************************************************
//
// mini oversigt
// i start menu
//
// ****************************************************************************************


void film_oversigt_typem::show_minifilm_oversigt(float _mangley,int filmnr) {
  int lfilmoversigt_antal=6;
  int i=0;
//  int txtbrede;
  bool cover3d=false;
  char *lastslash;
  float xvgaz=0.0f;
  char temptxt[200];
  unsigned int sofset=0;
  int bonline=8;
  float boffset;
  int ofs;
  static bool movie_oversigt_loaded=false;
  static int movie_oversigt_loaded_done=0;
  char tmpfilename[200];
  int loader_xpos,loader_ypos;
  int winsizx,winsizy;
  int xpos,ypos;
  // load dvd covers dynamic one pr frame
  if ((movie_oversigt_loaded==false) && (movie_oversigt_loaded_nr<6)) {
    /*
    strcpy(tmpfilename,this->filmoversigt[movie_oversigt_loaded_nr].getfilmcoverfile());
    if ((file_exists(tmpfilename)) && (this->filmoversigt[movie_oversigt_loaded_nr].gettextureid()==0)) {
      this->filmoversigt[movie_oversigt_loaded_nr].settextureidfile(tmpfilename);
    }
    */
    if (movie_oversigt_loaded_nr==(int) filmoversigt_antal) {
      movie_oversigt_loaded=true;
      movie_oversigt_loaded_done=1;
    } else movie_oversigt_loaded_nr++;
  }
  glTranslatef(0.0f, 0.0f ,0.0f);
  // mask
  winsizx=200;
  winsizy=200;
  xpos=220;
  ypos=700;
  while((i<lfilmoversigt_antal) && (i+sofset<filmoversigtsize)) {
    sofset=(_mangley/40)*8;
    if ((i+sofset)<filmoversigt_antal) {
      if (((i % bonline)==0) && (i>0)) {
        xpos=220;
        ypos=ypos-(winsizy+60);
      }
      if (i+1==(int) film_key_selected) boffset+=10; else boffset=0;
      if (((i+sofset)<filmoversigt_antal) && (filmoversigt[i+sofset].gettextureid())) {
        // print cover dvd
        //glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D,_defaultdvdcover);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(120+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(xpos,ypos+((orgwinsizey/2)-(800/2))-boffset , 0.0);
        glTexCoord2f(0, 1); glVertex3f(xpos,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset , 0.0);
        glTexCoord2f(1, 1); glVertex3f(xpos+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset , 0.0);
        glTexCoord2f(1, 0); glVertex3f(xpos+winsizx,ypos+((orgwinsizey/2)-(800/2))-boffset , 0.0);
        glEnd();
        // print movie cover over
        glBindTexture(GL_TEXTURE_2D,filmoversigt[i+sofset].gettextureid());
        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(120+i+sofset);
        glBegin(GL_QUADS); //Begin quadrilateral coordinates
        glTexCoord2f(0, 0); glVertex3f(xpos+24,ypos+((orgwinsizey/2)-(800/2))-boffset+5 , 0.0);
        glTexCoord2f(0, 1); glVertex3f(xpos+24,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset-5 , 0.0);
        glTexCoord2f(1, 1); glVertex3f(xpos+winsizx-3,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset-5 , 0.0);
        glTexCoord2f(1, 0); glVertex3f(xpos+winsizx-3,ypos+((orgwinsizey/2)-(800/2))-boffset+5 , 0.0);
        glEnd(); //End quadrilateral coordinates
      } else {
        // print cover dvd
        glEnable(GL_TEXTURE_2D);
        //glBlendFunc(GL_DST_COLOR, GL_ZERO);
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,_defaultdvdcover);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(120+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(xpos,ypos+((orgwinsizey/2)-(800/2))-boffset , 0.0);
        glTexCoord2f(0, 1); glVertex3f(xpos,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset , 0.0);
        glTexCoord2f(1, 1); glVertex3f(xpos+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset , 0.0);
        glTexCoord2f(1, 0); glVertex3f(xpos+winsizx,ypos+((orgwinsizey/2)-(800/2))-boffset , 0.0);
        glEnd();
      }
      strcpy(temptxt,filmoversigt[i+sofset].getfilmtitle());        // movie title     
      lastslash=strrchr(temptxt,'/');
      if (lastslash) strcpy(temptxt,lastslash+1);
      drawLinesOfText(temptxt, 14.00f+xpos, 110.0f+ypos, 0.4f , 12, 2 , 1);
    }
    xpos+=205;
    i++;
  }
}


// ****************************************************************************************
//
// normal oversigt
// ny udgave
//
// ****************************************************************************************

void film_oversigt_typem::show_film_oversigt(float _mangley,int filmnr) {
  int lfilmoversigt_antal=4*8;
  int film_nr=0;
  bool cover3d=false;
  char *lastslash;
  float xvgaz=0.0f;
  unsigned int sofset=0;
  int bonline=8;
  float boffset;
  int ofs;
  static bool movie_oversigt_loaded=false;
  static int movie_oversigt_loaded_done=0;
  char tmpfilename[200];
  int loader_xpos,loader_ypos;
  int winsizx,winsizy;
  int xpos,ypos;
  char *base,*right_margin;
  int length,width;
  int pline=0;
  bool stop=false;
  int buttonsize=200;
  int buttonsizey=180;
  int xof=5;
  int yof=orgwinsizey-(buttonsizey);
  // load dvd covers dynamic one pr frame
  if ((movie_oversigt_loaded==false) && (movie_oversigt_loaded_nr<(int) this->filmoversigt_antal)) {
    movie_oversigt_gfx_loading=true;
    strcpy(tmpfilename,this->filmoversigt[movie_oversigt_loaded_nr].getfilmcoverfile());
    if ((file_exists(tmpfilename)) && (this->filmoversigt[movie_oversigt_loaded_nr].gettextureid()==0)) {
      this->filmoversigt[movie_oversigt_loaded_nr].settextureidfile(tmpfilename);
    }
    if (movie_oversigt_loaded_nr==(int) filmoversigt_antal) {
      movie_oversigt_loaded=true;
      movie_oversigt_loaded_done=1;
      movie_oversigt_gfx_loading=false;
    } else movie_oversigt_loaded_nr++;
  }
  glTranslatef(0.0f, 0.0f ,0.0f);

  // mask
  winsizx=200;
  winsizy=200;
  xpos=20;
  ypos=700;
  while((film_nr<lfilmoversigt_antal) && (film_nr+sofset<filmoversigtsize)) {
    sofset=(_mangley/40)*8;
    if ((film_nr+sofset)<filmoversigt_antal) {
      if (((film_nr % bonline)==0) && (film_nr>0)) {
        xpos=20;
        ypos=ypos-(winsizy+60);
      }
      if (film_nr+1==(int) film_key_selected) boffset+=10; else boffset=0;
      if (filmoversigt[film_nr+sofset].gettextureid()) {
        // print cover dvd
        //glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        //glBlendFunc(GL_DST_COLOR, GL_ZERO);
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,_defaultdvdcover);                           //
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(120+film_nr+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(xpos,ypos+((orgwinsizey/2)-(800/2))-boffset , 0.0);
        glTexCoord2f(0, 1); glVertex3f(xpos,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset , 0.0);
        glTexCoord2f(1, 1); glVertex3f(xpos+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset , 0.0);
        glTexCoord2f(1, 0); glVertex3f(xpos+winsizx,ypos+((orgwinsizey/2)-(800/2))-boffset , 0.0);
        glEnd();
        // print movie cover over
        glBindTexture(GL_TEXTURE_2D,filmoversigt[film_nr+sofset].gettextureid());
        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(120+film_nr+sofset);
        glBegin(GL_QUADS); //Begin quadrilateral coordinates
        glTexCoord2f(0, 0); glVertex3f(xpos+24,ypos+((orgwinsizey/2)-(800/2))-boffset+5 , 0.0);
        glTexCoord2f(0, 1); glVertex3f(xpos+24,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset-5 , 0.0);
        glTexCoord2f(1, 1); glVertex3f(xpos+winsizx-3,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset-5 , 0.0);
        glTexCoord2f(1, 0); glVertex3f(xpos+winsizx-3,ypos+((orgwinsizey/2)-(800/2))-boffset+5 , 0.0);
        glEnd(); //End quadrilateral coordinates
      } else {
        // do default cover dvd
        glEnable(GL_TEXTURE_2D);
        //glBlendFunc(GL_DST_COLOR, GL_ZERO);
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,_defaultdvdcover);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(120+film_nr+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(xpos,ypos+((orgwinsizey/2)-(800/2))-boffset , 0.0);
        glTexCoord2f(0, 1); glVertex3f(xpos,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset , 0.0);
        glTexCoord2f(1, 1); glVertex3f(xpos+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset , 0.0);
        glTexCoord2f(1, 0); glVertex3f(xpos+winsizx,ypos+((orgwinsizey/2)-(800/2))-boffset , 0.0);
        glEnd();
      }
      // show movie name
      drawLinesOfText(filmoversigt[film_nr+sofset].getfilmtitle(),14.00f+xpos,114.0f+ypos,0.38f,22,2,1);
    }
    // next button
    xpos+=buttonsize;                                                // 205
    film_nr++;
    //glPopMatrix();
  }
  if ((film_nr==0) || (this->filmoversigt_antal==0)) {
    // show window
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D,_textureId9_askbox);                            // _texturemovieinfobox
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glTranslatef(550, 350, 0.0f);                                               // pos
    glBegin(GL_QUADS);    glPushMatrix();
    glTexCoord2f(0.0, 0.0); glVertex3f(10.0, 0.0, 0.0);                         // draw bos
    glTexCoord2f(0.0, 1.0); glVertex3f(10.0, 400.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(640.0, 400.0, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(640.0, 0.0, 0.0);
    glEnd();
    drawText("No movie info from backend.", 10.00f+xpos, 40.0f+ypos, 0.4f,1);
    glPopMatrix();
  }
}
