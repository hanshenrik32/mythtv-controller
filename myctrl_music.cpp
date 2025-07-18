//
// Music settings
//
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>
#include <mysql.h>
#include <GL/glc.h>                                       // gltext
#include <sys/types.h>                                    // listdir.c
#include <dirent.h>
#include <stdio.h>
#include <fmt/format.h>
#include <sqlite3.h>                    // sqlite interface to xbmc
#include <iostream>
#include <fmt/format.h>


using namespace std;

#include "myth_config.h"

#include "myctrl_music.h"
#include "myctrl_mplaylist.h"
#include "utility.h"
#include "readjpg.h"
#include "text3d.h"
#include "myth_ttffont.h"

#include "myth_saver.h"
#include "myctrl_glprint.h"

extern GLuint big_search_bar_playlist;                    // big search bar used by sporify search
extern GLuint big_search_bar_track;                    // big search bar used by sporify search
extern GLuint big_search_bar_albumm;                    // big search bar used by sporify search
extern GLuint big_search_bar_artist;                    // big search bar used by sporify search
extern GLuint tidal_big_search_bar_artist;
extern GLuint tidal_big_search_bar_album;
extern GLuint tidal_big_search_bar_track;

extern char keybuffer[512];                     // keyboard buffer

extern FILE *logfile;
extern char debuglogdata[1024];

extern float configdefaultmusicfontsize;                        // default font
extern char music_db_update_loader[256];                       //
extern int music_oversigt_loaded_nr;

extern char *dbname;                                          // internal database name in mysql (music,movie,radio)
extern int debug;
extern bool global_use_internal_music_loader_system;
extern char configdefaultmusicpath[256];                      // internal db for music
extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];
extern char configmusicmypath[];
extern int configmythtvver;
extern int screen_size;
extern int screensizey;
extern int screeny;
extern int debugmode;
extern unsigned int musicoversigt_antal;
extern int music_select_iconnr;
extern int do_music_icon_anim_icon_ofset;
extern GLuint _textureIdback1;
extern GLuint _textureId28_1;
extern GLuint _textureId29_1;
extern GLuint _textureIdloading;

// Screen saver box ********************************************************************************
extern boxarray mybox;                         // 3D screen saver
extern int fonttype;
extern fontctrl aktivfont;
mplaylist aktiv_playlist;
extern int sinusofset;
extern GLint cur_avail_mem_kb;
extern int orgwinsizey;
extern int orgwinsizex;
extern char localuserhomedir[4096];

extern bool do_sqlite;

extern config_icons config_menu;

// ****************************************************************************************
//
// parameret til mysql er dirid = directory_id i database
// retunere path og parent_id som bliver udfyldt fra mysql databasen
//
// ****************************************************************************************

void hent_dir_id(char *path,char *parent_id,char *dirid) {
    char database[256];
    std::string sqlselect;
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");    
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    sqlselect = "select parent_id,path from music_directories where directory_id=";
    sqlselect = sqlselect + dirid;
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect.c_str());
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        strcpy(parent_id,row[0]);
        strcpy(path,row[1]);
      }
    }
    mysql_close(conn);
}

// ****************************************************************************************
//
// retunere parent dirid fra dir_id i mythtv music
//
// ****************************************************************************************

unsigned int hent_parent_dir_id(int dirid) {
    // mysql stuf
    char database[256];
    char sqlselect[256];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char temp[256];
    unsigned returid=0;
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    snprintf(temp,sizeof(temp),"%d",dirid);
    strcpy(sqlselect,"select parent_id from music_directories where directory_id=");
    strcat(sqlselect,temp);
    try {
      conn=mysql_init(NULL);
      // Connect to database
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          returid=atol(row[0]);
        }
      }
      mysql_close(conn);
    }
    catch (...) {
      write_logfile(logfile,(char *) "Mysql db connect error");
    }
    return(returid);
}


// ****************************************************************************************
//
// check about cd cover to created from
// return fundetnavn variable hvis det er fundet.
//
// ****************************************************************************************

int findcoverfile(char *path,char *fundetnavn) {
    int n=0;
    bool fundet=false;
    const int ANTAL=7;
    char filename[10][20];
    char filepath[512];
    strcpy(filename[0],"Front.jpg");
    strcpy(filename[1],"front.jpg");
    strcpy(filename[2],"Cover.jpg");
    strcpy(filename[3],"cover.jpg");
    strcpy(filename[4],"Folder.jpg");
    strcpy(filename[5],"folder.jpg");
    strcpy(filename[6],"thumbnail.jpg");
    strcpy(filename[7],"Thumbnail.jpg");
    while(!(fundet) && (n<=ANTAL)) {
      strcpy(filepath,path);			// load org path again
      strcat(filepath,"/");			// add next filetype to test for
      strcat(filepath,filename[n]);		//
      if (file_exists(filepath)) fundet=true;
      n++;
    }
    // return file exist tested path
    if (fundet) {
      strcpy(fundetnavn,filepath);
      return(1);
    } else {
      strcpy(fundetnavn,"");
      return(0);
    }
}

// ****************************************************************************************
//
// get artisid from db
//
// ****************************************************************************************

int get_artistid(char *artistname) {
    // mysql stuf
    // mysql vars
    MYSQL *conn2;
    MYSQL_RES *res2;
    MYSQL_ROW row2;
    char *database = (char *) "mythtvcontroller";
    int artistid=0;
    std::string sqlselect;
    // hent atrist id
    try {
      conn2=mysql_init(NULL);
      if (conn2) {
        mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
        artistid=0;
        sqlselect = fmt::format("select artist_id from music_artists where artist_name like '{}'",artistname);
        mysql_query(conn2,sqlselect.c_str());
        res2 = mysql_store_result(conn2);
        if (res2) {
          while ((row2 = mysql_fetch_row(res2)) != NULL) {
            artistid=atol(row2[0]);
          }
        }
        mysql_close(conn2);
      }
    }
    catch (...) {
      write_logfile(logfile,(char *) "Mysql db connect error");
    }
    return(artistid);
}


// ****************************************************************************************
//
// check about song name exist in db
//
// ****************************************************************************************

int song_exist_in_db(char *filename,char *name) {
  char database[256];
  char sqlselect[1024];
  // mysql vars
  MYSQL *conn2;
  MYSQL_RES *res2;
  MYSQL_ROW row2;
  int songid=0;
  char sqlselect1[1024];
  // hent atrist id
  try {
    conn2=mysql_init(NULL);
    if (conn2) {
      mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      snprintf(sqlselect1,sizeof(sqlselect1),"select song_id from music_songs where filename like '%s' and name like '%s'",filename,name);
      mysql_query(conn2,sqlselect1);
      res2 = mysql_store_result(conn2);
      if (res2) {
        while ((row2 = mysql_fetch_row(res2)) != NULL) {
          songid=atol(row2[0]);
        }
      }
      mysql_close(conn2);
    }
  }
  catch (...) {
    write_logfile(logfile,(char *) "Mysql db connect error");
  }
  return(songid);
}


// ****************************************************************************************
//
// check music internal db exist if yes use it.
// return true if exist
// ****************************************************************************************

bool global_use_internal_music_loader_system_exist() {
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  bool dbexist=false;
  if (do_sqlite) {
    return(dbexist);
  } else {
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
    mysql_query(conn,"SHOW TABLES LIKE 'music_songs'");
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        dbexist=true;
        strcpy(configmusicpath,configdefaultmusicpath);       // set music global path
      }
      if (conn) mysql_close(conn);
    }
  }
  return(dbexist);
}


// *****************************************************************************
//
// load selected playlist from gui to play
//
// *****************************************************************************


void get_music_pick_playlist(long find_dir_id,bool *music_list_select_array) {
    GLuint texture; //The id of the texture
    char database[256];
    char tmptxt[512];
    char tmptxt1[512];
    char tmptxt2[512];
    char tmptxt3[512];                  // ny cover som skal loades
    char husk_tmptxt3[250];             // husk ny cover
    std::string sqlselect;
    int i;
    char parent_id[200];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    snprintf(tmptxt,sizeof(tmptxt),"%ld",find_dir_id);
    // old ver
    // sqlselect = "select song_id,filename,directory_id,music_albums.album_name,name,music_artists.artist_id,music_artists.artist_name,length from music_songs,music_albums,music_artists where directory_id=";
    // sqlselect = sqlselect + tmptxt;
    // sqlselect = sqlselect + " and music_artists.artist_id=music_songs.artist_id and music_songs.album_id=music_albums.album_id order by name";
    // 
    // 
    // new ver works in mariadb
    sqlselect = "select song_id,filename,directory_id,album_id,name,0,artist_name,length from music_songs left join music_artists on music_artists.artist_id=music_songs.artist_id WHERE directory_id=";
    sqlselect = sqlselect + tmptxt;
    sqlselect = sqlselect + " order by name";
    // printf("SQLSELECT = %s  \n ",sqlselect.c_str());
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect.c_str());
    res = mysql_store_result(conn);
    i=0;
    strcpy(husk_tmptxt3,"");
    strcpy(tmptxt3,"");
    if (res) {
      while (((row = mysql_fetch_row(res)) != NULL) && (i<MAX_IN_PLAYLIST)) {
        if (global_use_internal_music_loader_system) {
          snprintf(debuglogdata,sizeof(debuglogdata),"Found song song_id:%4s Artist id:%4s Filename:%40s",row[0],row[5],row[1]);
          write_logfile(logfile,(char *) debuglogdata);
        } else {
          snprintf(debuglogdata,sizeof(debuglogdata),"Found song song_id:%4s Artist id:%4s Filename:%40s",row[0],row[5],row[1]);
          write_logfile(logfile,(char *) debuglogdata);
        }
        if (global_use_internal_music_loader_system) {
          strcpy(tmptxt,"");
        } else strcpy(tmptxt,configmusicpath);                                  // set defult start path from internal or mythtv if exist
        snprintf(tmptxt2,sizeof(tmptxt2),"%s",row[2]);                          // hent dir id
        hent_dir_id(tmptxt1,parent_id,tmptxt2);                                 // hent path af tmptxt2 som er = dir_id
        strcpy(tmptxt3,tmptxt);                                                 // temptxt3 er = path
        strcat(tmptxt3,"mythcFront.jpg");                                       // add filename til cover
        strcat(tmptxt,row[1]);                                                  // add filename til sang
        if ((strcmp(tmptxt3,husk_tmptxt3)!=0) && (file_exists(tmptxt3))) {
          texture=loadTexture(tmptxt3);                                         // load texture
        } else {
          texture=0;
        }
        //                                                                           song_id,artistid,album_name,name,artist_name,length
        if ((music_list_select_array[i]==true)) aktiv_playlist.m_add_playlist(tmptxt,row[0],row[5],row[4],row[4],row[6],row[7],0,texture);      // add (gem) info i playlist
        i++;
      }
    }
    if (debugmode & 2) printf("Numbers in playlist=%d \n",aktiv_playlist.numbers_in_playlist());
    mysql_close(conn);
}



// ****************************************************************************************
//
// loader valgte play liste
//
// ****************************************************************************************

int musicoversigt_class::get_music_pick_playlist(long find_dir_id,bool *music_list_select_array) {
    GLuint texture; //The id of the texture
    char database[256];
    char tmptxt[512];
    char tmptxt1[512];
    char tmptxt2[512];
    char tmptxt3[512];			// ny cover som skal loades
    char husk_tmptxt3[250];		// husk ny cover
    // char sqlselect[512];
    std::string sqlselect;
    int i;
    char parent_id[200];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    snprintf(tmptxt,sizeof(tmptxt),"%ld",find_dir_id);
    sqlselect = "select song_id,filename,directory_id,music_albums.album_name,name,music_artists.artist_id,music_artists.artist_name,length from music_songs,music_artists,music_albums where directory_id=";
    sqlselect = sqlselect + tmptxt;
    sqlselect = sqlselect + " and music_artists.artist_id=music_songs.artist_id and music_songs.album_id=music_albums.album_id order by name";
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect.c_str());
    res = mysql_store_result(conn);
    i=0;
    strcpy(husk_tmptxt3,"");
    strcpy(tmptxt3,"");
    if (res) {
      while (((row = mysql_fetch_row(res)) != NULL) && (i<MAX_IN_PLAYLIST)) {
        if (global_use_internal_music_loader_system) {
          if (debugmode & 2) printf("Found song song_id:%4s Artist id:%4s Filename:%40s \n",row[0],row[5],row[1]);
        } else {
          if (debugmode & 2) printf("Found song song_id:%4s Artist id:%4s Filename:%40s \n",row[0],row[5],row[1]);
        }
        if (global_use_internal_music_loader_system) {
          strcpy(tmptxt,"");
        } else strcpy(tmptxt,configmusicpath);		// set defult start path from internal or mythtv if exist
        snprintf(tmptxt2,sizeof(tmptxt2),"%s",row[2]);		         	// hent dir id
        hent_dir_id(tmptxt1,parent_id,tmptxt2);		// hent path af tmptxt2 som er = dir_id
        strcpy(tmptxt3,tmptxt);		                // temptxt3 er = path
        strcat(tmptxt3,"mythcFront.jpg");		      // add filename til cover
        strcat(tmptxt,row[1]);				            // add filename til sang
        if ((strcmp(tmptxt3,husk_tmptxt3)!=0) && (file_exists(tmptxt3))) {
          texture=loadTexture(tmptxt3);				// load texture
        } else {
          texture=0;
        }
        if ((music_list_select_array[i]==true)) aktiv_playlist.m_add_playlist(tmptxt,row[0],row[5],row[3],row[4],row[6],row[7],0,texture);	// add (gem) info i playlist
        i++;
      }
    }
    if (debugmode & 2) printf("Numbers in playlist=%d \n",aktiv_playlist.numbers_in_playlist());
    mysql_close(conn);
    return(1);
}





// ****************************************************************************************
//
// opdateret antal af afspilinger samt afspilings dato
//
// ****************************************************************************************

int musicoversigt_class::update_afspillinger_music_song(char *filename) {
    std::string sqlselect;
    char songname[1024];
    char *pathpointer;
    char database[256];
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    MYSQL *conn;
    MYSQL_RES *res;
    //MYSQL_ROW row;
    conn=mysql_init(NULL);
    // Connect to mythtv database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    pathpointer=strrchr(filename,'/');
    if (pathpointer) {
      strcpy(songname,pathpointer+1);		// hentfilename efter path (sidste '/')
    } else {
      strcpy(songname,filename);		//
    }
    sqlselect = fmt::format("update music_songs set music_songs.lastplay=NOW() where music_songs.filename like '{}' limit 1",songname);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect.c_str());
    res = mysql_store_result(conn);
    sqlselect = fmt::format("update music_songs music_songs.numplays=music_songs.numplays+1 where music_songs.filename like '%s' limit 1",songname);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect.c_str());
    res = mysql_store_result(conn);
    mysql_close(conn);
    return(1);
}




// ****************************************************************************************
//
// load music conver gfx
//
// ****************************************************************************************

int musicoversigt_class::load_music_covergfx() {
    unsigned int i=0;                                                           // hent alle music convers
    char tmpfilename[200];
    while (i<antal_music_oversigt-1) {
      strcpy(tmpfilename,"");
      if (strcmp(tmpfilename,musicoversigt[i].album_coverfile)==0) strcpy(tmpfilename,"images/");
      strcat(tmpfilename,musicoversigt[i].album_coverfile);
      if ((strcmp(tmpfilename,"images/")!=0) && (file_exists(tmpfilename))) {
        if (musicoversigt[i].textureId==0) musicoversigt[i].textureId = loadTexture(tmpfilename);
      }
      i++;
    }
    return(i);                                                                  // return # of loaded images
}


// ************************************************************************
//
// get info from db
//
// ************************************************************************

char *musicoversigt_class::get_album_name(int nr) {
  if (nr<=antal_music_oversigt) {
    return(musicoversigt[nr].album_name);
  } else return {};
}

char musicoversigt_class::get_album_type(int nr) {
  if (nr<antal_music_oversigt) {
    return(musicoversigt[nr].oversigttype);
  } else return(0);
}

unsigned int musicoversigt_class::get_directory_id(int nr) {
  if (nr<=antal_music_oversigt) {
    return(musicoversigt[nr].directory_id);
  } else return(0);
}

char *musicoversigt_class::get_album_path(int nr) {
  if (nr<=antal_music_oversigt) {
    return(musicoversigt[nr].album_path);
  } else return(0);
}

GLuint musicoversigt_class::get_textureId(int nr) {
  if (nr<=antal_music_oversigt) {
    return(musicoversigt[nr].textureId);
  } else return(0);
}

// *****************************************************************************
//
// Load/update music info to new db mythtvcontroller
//
// *****************************************************************************


int musicoversigt_class::opdatere_music_oversigt_nodb() {
  int parent;
  int dirid;
  int sub_dirid;
  int artistid;
  int parent_dir_id;
  char *ext;
  unsigned int i=0;
  unsigned int ii=0;
  struct dirent *de=NULL;
  struct dirent *de2=NULL;
  int albumid=0;
  DIR *dirp=NULL;
  DIR *dirp1=NULL;
  DIR *dirp2=NULL;
  char sqlselect2[4096];
  char checkdir[4096];
  char debuglogdata[4096];
  std::string checkdir2s;
  std::string sqlselect;
  std::string sqlselect1;
  std::string dirpath;
  MYSQL *conn;
  MYSQL *conn1;
  MYSQL *conn2;
  MYSQL *conn3;
  MYSQL_RES *res;
  MYSQL_RES *res1;
  MYSQL_RES *res2;
  MYSQL_RES *res3;
  MYSQL_ROW row;
  MYSQL_ROW row2;
  MYSQL_ROW row3;
  int a;
  if (strcmp(configdefaultmusicpath,"")==0) {
    printf("No music patch in config file\nUSe default homedir/Music");
    dirpath = "~/Music/";
  } else dirpath = configdefaultmusicpath;                                     // get path from config file
  char filetype[10];
  char songname[1024];
  int dbexist=0;                                                                // use to check db exist
  bool dirfindes=false;
  clean_music_oversigt();                                               				// clear music oversigt 
  dirp=opendir(dirpath.c_str());
  if (dirp==NULL) {
    printf("Open dir error ->%s<-\n",dirpath.c_str());
    return 1;
  }
  if (do_sqlite) {
      musicoversigt_antal=i-1;
      strcpy(music_db_update_loader,"");

  } else {
    conn=mysql_init(NULL);
    // Connect to database
    if (conn) {
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      // test about musik table exist
      mysql_query(conn,"SHOW TABLES LIKE 'music_albums'");
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          dbexist=true;
        }
      } else dbexist=false;
      //
      // if database not exist do dir scan and create tables for music
      //
      if (!(dbexist)) {
        write_logfile(logfile,(char *) "Creating database for music if not exist.");
        sqlselect="create table IF NOT EXISTS music_directories(directory_id int NOT NULL AUTO_INCREMENT PRIMARY KEY,path text, parent_id int)";
        mysql_query(conn,sqlselect.c_str());
        res = mysql_store_result(conn);
        sqlselect="create table IF NOT EXISTS music_albums(album_id int NOT NULL AUTO_INCREMENT PRIMARY KEY, artist_id int, album_name varchar(255) ,year int, compilation int)";
        mysql_query(conn,sqlselect.c_str());
        res = mysql_store_result(conn);
        sqlselect="create table IF NOT EXISTS music_songs(song_id int NOT NULL AUTO_INCREMENT PRIMARY KEY,filename text,name varchar(255),track int, artist_id int, album_id int, genre_id int,year int,length int,numplays int,rating int,lastplay datetime, date_entered  datetime, date_modified datetime,format varchar(4), mythdigest varchar(255) ,size int,description  varchar(255), comment varchar(255), disc_count int, disc_number int, track_count  int, start_time int, stop_time int, eq_preset varchar(255),relative_volume int, sample_rate int, bitrate int,bpm int, directory_id int)";
        mysql_query(conn,sqlselect.c_str());
        res = mysql_store_result(conn);
        sqlselect="create table IF NOT EXISTS music_artists(artist_id int NOT NULL AUTO_INCREMENT PRIMARY KEY, artist_name varchar(255))";
        mysql_query(conn,sqlselect.c_str());
        res = mysql_store_result(conn);
            sqlselect="create table IF NOT EXISTS music_genres(genre_id int NOT NULL AUTO_INCREMENT PRIMARY KEY, genre varchar(255))";
        mysql_query(conn,sqlselect.c_str());
        res = mysql_store_result(conn);
        sqlselect="create table IF NOT EXISTS music_playlist(playlist_id int NOT NULL AUTO_INCREMENT PRIMARY KEY, playlist_name varchar(255),playlist_songs text,last_accessed datetime,length int,songcount int,hostname varchar(64))";
        mysql_query(conn,sqlselect.c_str());
        res = mysql_store_result(conn);
      }
      if (true) {
        // Empty old musicdb, and build new.
        sqlselect="TRUNCATE table music_songs";
        mysql_query(conn,sqlselect.c_str());
        res = mysql_store_result(conn);
        sqlselect="TRUNCATE table music_directories";
        mysql_query(conn,sqlselect.c_str());
        res = mysql_store_result(conn);
        sqlselect="TRUNCATE table music_albums";
        mysql_query(conn,sqlselect.c_str());
        res = mysql_store_result(conn);
        strcpy(musicoversigt[0].album_name,"PLAYLIST");
        strcpy(musicoversigt[0].album_path,"");
        strcpy(musicoversigt[0].album_coverfile,"");
        musicoversigt[0].textureId=0;
        musicoversigt[0].directory_id=0;			// husk directory id
        musicoversigt[0].parent_id=0;
        musicoversigt[0].album_id=0;
        musicoversigt[0].artist_id=0;
        musicoversigt[0].oversigttype=-1;			// type -1 = playlist
        i++;
        if (dirp) {
          printf("Music update/Loading directory %s\n",dirpath.c_str());
          write_logfile(logfile,(char *) "update/Loading directory ");
          // create db over all dirs in start path
          while(de = readdir(dirp)) {
            if ((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0) && (strcmp(de->d_name,"@eaDir")!=0)) {
              // if dir
              if (de->d_type==DT_DIR) {
                printf("Checking directory %20s \n" , de->d_name);
                dirfindes=false;
                conn2=mysql_init(NULL);
                if (conn2) {
                  mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
                  sqlselect1=fmt::format("select directory_id from music_directories where path like '{}'",de->d_name);
                  mysql_query(conn2,sqlselect1.c_str());
                  res2 = mysql_store_result(conn2);
                  if (res2) {
                    while ((row2 = mysql_fetch_row(res2)) != NULL) {
                      dirfindes=true;
                      printf("***** DIR Fundet %s \n",de->d_name);
                    }
                  }              
                  mysql_close(conn2);
                }
                sqlselect = fmt::format("insert into music_directories(directory_id,path,parent_id) values({},'{}',{})",0,de->d_name,0);
                mysql_query(conn,sqlselect.c_str());
                res = mysql_store_result(conn);
                strcpy(musicoversigt[i].album_name,de->d_name);
                strcpy(musicoversigt[i].album_path,"");
                //strcat(musicoversigt[i].album_path,"/");
                strcat(musicoversigt[i].album_path,de->d_name);
                // strcpy(musicoversigt[i].album_coverfile,icon_file);
                musicoversigt[i].directory_id=i;			// husk directory id
                musicoversigt[i].parent_id=0;
                musicoversigt[i].album_id=0;
                musicoversigt[i].artist_id=0;
                musicoversigt[i].oversigttype=0;
                parent_dir_id=0;
                // update artist db
                snprintf(sqlselect2,sizeof(sqlselect2),"insert into music_artists values (%d,'%s')",0,de->d_name);
                mysql_query(conn,sqlselect2);
                res = mysql_store_result(conn);
                i++;
              }
            }
          }
          // fill database music_albums from dir from music_directories
          sqlselect="select directory_id ,path ,parent_id from music_directories";
          if (conn) {
            mysql_query(conn,sqlselect.c_str());
            res = mysql_store_result(conn);
            // loop dirs database names from root / (music dir start path)
            while ((row = mysql_fetch_row(res)) != NULL) {
              // log info
              // snprintf(debuglogdata,4090,"Checking dir %s/%s ",dirpath,row[1]);
              // write_logfile((char *) debuglogdata);
              dirid=atoi(row[0]);
              snprintf(checkdir,sizeof(checkdir),"%s/%s",dirpath.c_str(),row[1]);
              dirp1=opendir(checkdir);
              // error handler
              if (dirp1==NULL) {
                printf("Open dir error ->%s<-\n",checkdir);
                return 1;
                exit(0);
              }
              artistid=0;
              // hent atrist id
              conn2=mysql_init(NULL);
              if (conn2) {
                mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
                sqlselect1=fmt::format("select artist_id from music_artists where artist_name like '{}'",row[1]);
                mysql_query(conn2,sqlselect1.c_str());
                res2 = mysql_store_result(conn2);
                if (res2) {
                  while ((row2 = mysql_fetch_row(res2)) != NULL) {
                    artistid=atol(row2[0]);
                  }
                }
                mysql_close(conn2);
              }
              // loop over checkdir
              if (dirp1) {
                // loop the dir and create the music records and music_dir if mount in loop dir
                while(de = readdir(dirp1)) {
                  if ((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0)) {
                    // if dir
                    // opret i album dmknapnr =b
                    if (de->d_type==DT_DIR) {
                      conn2=mysql_init(NULL);
                      if (conn2) {
                        mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
                        sqlselect1=fmt::format("insert into music_albums(album_id,artist_id,album_name,year,compilation) values({},{},'{}',{},{})",0,artistid,de->d_name,0,0);
                        mysql_query(conn2,sqlselect1.c_str());
                        res2 = mysql_store_result(conn2);
                        // husk last dir vi kommer fra
                        parent_dir_id=atoi(row[0]);
                        // create dir id for subdir
                        sqlselect1=fmt::format("insert into music_directories(directory_id,path,parent_id) values({},'{}',{})",0,de->d_name,parent_dir_id);
                        mysql_query(conn2,sqlselect1.c_str());
                        res2 = mysql_store_result(conn2);
                        // hent dirid der lige er oprettet
                        sqlselect1=fmt::format("select directory_id from music_directories where path like '{}'",de->d_name);
                        mysql_query(conn2,sqlselect1.c_str());
                        res2 = mysql_store_result(conn2);
                        if (res2) {
                          while ((row2 = mysql_fetch_row(res2)) != NULL) {
                            sub_dirid=atol(row2[0]);
                          }
                        }
                        mysql_close(conn2);
                        albumid=0;
                        // hent albumid til song db
                        conn2=mysql_init(NULL);
                        if (conn2) {
                          mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
                          // sprintf(sqlselect1,"select album_id from music_albums where album_name like '%s'",de->d_name);
                          sqlselect1=fmt::format("select album_id from music_albums where album_name like '{}'",de->d_name);
                          mysql_query(conn2,sqlselect1.c_str());
                          res2 = mysql_store_result(conn2);
                          if (res2) {
                            while ((row2 = mysql_fetch_row(res2)) != NULL) {
                              albumid=atol(row2[0]);
                            }
                          }
                          mysql_close(conn2);
                        }
                      }
                      // open found dir having the songs
                      // snprintf(debuglogdata,4090,"\t Checking sub dir %s ",de->d_name);
                      // write_logfile((char *) debuglogdata);
                      // make path                   
                      // snprintf(checkdir2,sizeof(checkdir2),"%s/%s",checkdir,de->d_name);
                      checkdir2s = fmt::format("{}/{}",checkdir,de->d_name);
                      dirp2=opendir(checkdir2s.c_str());
                      if (dirp2==NULL) {
                        printf("Open dir error ->%s<-\n",checkdir2s.c_str());
                        return 1;
                        exit(0);
                      }
                      // loop dir and update music songs db
                      while(de2 = readdir(dirp2)) {
                        if ((strcmp(de2->d_name,".")!=0) && (strcmp(de2->d_name,"..")!=0)) {
                          // get file extention
                          ext = strrchr(de2->d_name, '.');
                          if (ext) strcpy(filetype,ext+1); else strcpy(filetype,"");
                          if ((strcmp(filetype,"mp3")==0) || (strcmp(filetype,"flac")==0) || (strcmp(filetype,"wav")==0) || (strcmp(filetype,"ogg")==0)) {
                            // add found path
                            strcpy(songname,checkdir2s.c_str());
                            strcat(songname,"/");
                            strcat(songname,de2->d_name);
                            // show in music overview loader
                            sqlselect1 = fmt::format("insert into music_songs(song_id,filename,  name,    track, artist_id, album_id, genre_id, year, length, numplays, rating, lastplay,             date_entered,           date_modified,          format , mythdigest, size , description, comment, disc_count, disc_number, track_count, start_time, stop_time, eq_preset, relative_volume, sample_rate, bitrate, bpm, directory_id) values \
                                          ({},    '{}',      '{}',    {},    {},        {},       {},       {},    {},     {},      {},     '{}',                 '{}',                   '{}',                   '{}',    '{}',        {},   '{}',        '{}',    {},         {},          {},          {},          {},        '{}',       {},             {},          {},      {},     {})", \
                                          0,      songname,songname,0,    artistid,  albumid,   0,        0,     0,      0,       0,     "2012-01-01 00:00:00",   "2012-01-01 00:00:00","2012-01-01 00:00:00",  "",      "",          0,    "",          "",      0,          0,           0,           0,           0,         "",         0,              0,           0,       0,sub_dirid);

                            strcpy(music_db_update_loader,de->d_name);
                            music_oversigt_loaded_nr++;
                            conn1=mysql_init(NULL);
                            if (conn1) {
                              mysql_real_connect(conn1, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
                              mysql_query(conn1,sqlselect1.c_str());
                              res1 = mysql_store_result(conn1);
                              mysql_close(conn1);
                            }
                          }
                          printf("\t Update dir/music song %20s \n" , de2->d_name);
                        }
                      }
                      closedir(dirp2);
                    } else {
                      // it is a file
                      // get albumid
                      // find file extition
                      ext = strrchr(de->d_name, '.');
                      if (ext) strcpy(filetype,ext+1); else strcpy(filetype,"");
                      if ((strcmp(filetype,"mp3")==0) || (strcmp(filetype,"flac")==0) || (strcmp(filetype,"ogg")==0) || (strcmp(filetype,"wav")==0)) {
                        strcpy(songname,checkdir);
                        strcat(songname,"/");
                        strcat(songname,de->d_name);
                        sqlselect1 = fmt::format("insert into music_songs(song_id,filename,  name,    track, artist_id, album_id, genre_id, year, length, numplays, rating, lastplay,             date_entered,           date_modified,          format , mythdigest, size , description, comment, disc_count, disc_number, track_count, start_time, stop_time, eq_preset, relative_volume, sample_rate, bitrate, bpm, directory_id) values \
                                                                    ({},    '{}',      '{}',    {},    {},        {},       {},       {},    {},     {},      {},     '{}',                 '{}',                   '{}',                   '{}',    '{}',        {},   '{}',        '{}',    {},         {},          {},          {},          {},        '{}',       {},             {},          {},      {},     {})", \
                                                                    0,      songname,songname,0,    artistid,  albumid,   0,        0,     0,      0,       0,     "1970-01-01 00:00:00",   "1970-01-01 00:00:00","1970-01-01 00:00:00",  "",      "",          0,    "",          "",      0,          0,           0,           0,           0,         "",         0,              0,           0,       0, dirid);

                        conn1=mysql_init(NULL);
                        if (conn1) {
                          mysql_real_connect(conn1, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
                          mysql_query(conn1,sqlselect1.c_str());
                          res1 = mysql_store_result(conn1);
                          mysql_close(conn1);  
                        }
                        printf("\t Update music song %20s \n", de->d_name);
                      }
                    }
                    i++;  // next dir record
                  }
                }
              }
              if (dirp1) closedir(dirp1);
            }
          }
        }
        if (dirp) closedir(dirp);
      }
    }
    musicoversigt_antal=i-1;
    if (conn) mysql_close(conn);
    strcpy(music_db_update_loader,"");
  }
  write_logfile(logfile,(char *) "MUSIC Done update music directory.");
  if (i) return 0; else return 1;
}

// ****************************************************************************************
//
// callback function for sqlite3
//
// ****************************************************************************************

sqlite3 *sqlitedb_obj_music;

int music_sqldb_callback(void *data, int argc, char **argv, char **azColName) {
  int i;
  for (i=0; i<argc; i++) {
    printf("%s = %s\n",azColName[i], argv[i] ? argv[i] : "NULL");
  }
  return 0;
}

// ****************************************************************************************
//
// update_music_oversigt
// update list pr dir id from mythtv-backend or internal dbloader
//
// ****************************************************************************************

int musicoversigt_class::opdatere_music_oversigt(unsigned int directory_id) {
    char fundetpath[512];
    char convert_command[512];
    char tmpfilename[1024];
    std::string sqlselect;    
    char dirname[256];
    unsigned int i;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char database[256];
    char icon_file[512];
    char tmptxt[512];
    char tmptxt1[512];
    const char *sqllite_sql = "select directory_id,path,parent_id from music_directories where parent_id=0 order by path";
    char *zErrMsg = 0;
    int rc;
    char *data = (char *) "sqlitedb_obj_music";  
    // select the right db to update from
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    clean_music_oversigt();
    if (debugmode & 1) printf("Opdatere music oversigt fra database : %s \n",database);
    sprintf(debuglogdata,"Opdatere music oversigt fra database : %s ",database);
    write_logfile(logfile,(char *) debuglogdata);
    i=0;
    if (directory_id==0) {			// hent fra starten top music directory
      // strcpy(sqlselect,"select directory_id,path,parent_id from music_directories where parent_id=0 order by path");
      sqlselect = "select directory_id,path,parent_id from music_directories where parent_id=0 order by path";
      strcpy(musicoversigt[i].album_name,"PLAYLIST");
      strcpy(musicoversigt[i].album_path,"");
      strcpy(musicoversigt[i].album_coverfile,"");
      musicoversigt[i].textureId=0;
      musicoversigt[i].directory_id=0;			// husk directory id
      musicoversigt[i].parent_id=0;
      musicoversigt[i].album_id=0;
      musicoversigt[i].artist_id=0;
      musicoversigt[i].oversigttype=-1;			// type -1 = playlist
      i++;
    } else {
      // sprintf(sqlselect,"select directory_id,path,parent_id from music_directories where parent_id=%d",directory_id);		 // dir id in mythtv mysql
      sqlselect = fmt::format("select directory_id,path,parent_id from music_directories where parent_id={}",directory_id);     
      strcpy(musicoversigt[i].album_name,"   BACK");
      strcpy(musicoversigt[i].album_path,"");
      strcpy(musicoversigt[i].album_coverfile,"");
      musicoversigt[i].textureId=0;
      musicoversigt[i].directory_id=hent_parent_dir_id(directory_id); // husk directory id
      musicoversigt[i].parent_id=0;
      musicoversigt[i].album_id=0;
      musicoversigt[i].artist_id=0;
      musicoversigt[i].oversigttype=0;			// type 0 = dirid
      i++;
    }
    if (do_sqlite) {
      sqlite3_open("mythtvcontroller.db", &sqlitedb_obj_music);
      rc = sqlite3_exec(sqlitedb_obj_music, sqllite_sql,music_sqldb_callback, (void*)data, &zErrMsg);
      if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
      } else {
        printf("Operation done successfully\n");
      }
      sqlite3_close(sqlitedb_obj_music);
    } else {
      conn=mysql_init(NULL);
      // Connect to database
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      mysql_query(conn,sqlselect.c_str());
      res = mysql_store_result(conn);
      if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (i<MUSIC_OVERSIGT_TYPE_SIZE)) {
          strcpy(dirname,row[1]);
          strcpy(tmptxt,configmusicpath);			// config dir fra mythtv setup table
          strcat(tmptxt,row[1]);
          //strcat(tmptxt,"/Front.jpg");
          strcpy(tmptxt1,configmusicpath);			// config dir fra mythtv setup table
          strcat(tmptxt1,row[1]);
          strcat(tmptxt1,"/front.jpg");
          // check for some file types to convert to cover
          findcoverfile(tmptxt,fundetpath);			// return fundetpath = picture found        
          if (file_exists(fundetpath)) {
            /*
            strcpy(convert_command,"/usr/bin/convert -scale 128 ");
            strcat(convert_command,"\"");
            strcat(convert_command,fundetpath);
            strcat(convert_command,"\"");
            strcat(convert_command," \"");
            strcat(convert_command,configmusicpath);
            strcat(convert_command,row[1]);
            strcat(convert_command,"/");
            strcat(convert_command,"mythcFront.jpg\"");
            strcpy(tmptxt,configmusicpath);
            strcat(tmptxt,row[1]);
            strcat(tmptxt,"/mythcFront.jpg");
            if (!(file_exists(tmptxt))) {
              system(convert_command);
              if (debugmode) printf("Do Convert scale image %s to 128*128 \n",row[1]);
            }
            */
            strcpy(tmptxt,configmusicpath);
            strcat(tmptxt,row[1]);
            strcat(tmptxt,"/");
            strcat(tmptxt,"cover.jpg");
            if (!(file_exists(tmptxt))) {
              strcpy(tmptxt,configmusicpath);
              strcat(tmptxt,row[1]);
              strcat(tmptxt,"/");
              strcat(tmptxt,"Front.jpg");
            }
            strcpy(icon_file,fundetpath);		// gem icon file name
          } else {
            strcpy(icon_file,"");			// no icon
          }
          strcpy(musicoversigt[i].album_name,dirname);
          strcpy(musicoversigt[i].album_path,"");
          strcpy(musicoversigt[i].album_coverfile,icon_file);
          
          // måske crash
          std::string tmp123;
          if (global_use_internal_music_loader_system) strcpy(tmpfilename,configdefaultmusicpath); else strcpy(tmpfilename,configmusicpath);
          tmp123 = tmpfilename;
          tmp123 = tmp123 + row[1];
          tmp123 = tmp123 + "/";
          tmp123 = tmp123 + "cover.jpg";
          if (strlen(tmp123.c_str())>0) {
            if (file_exists(tmp123.c_str())) {
              // musicoversigt[i].textureId = loadTexture((char *) tmp123.c_str());
            }
          }
          musicoversigt[i].directory_id=atoi(row[0]);			// husk directory id
          musicoversigt[i].parent_id=atoi(row[2]);
          musicoversigt[i].album_id=0;
          musicoversigt[i].artist_id=0;
          musicoversigt[i].oversigttype=0;
          i++;
          antal_music_oversigt=i;
        }        	// end while
      } else {
        write_logfile(logfile,(char *) "mysql sql database err.");
        i=0;
      }
      if (i>0) {
        antal_music_oversigt=i-1;
        if (debugmode & 2) printf(" %d CD Covers loaded.\n",antal_music_oversigt);
      }
    }
    musicoversigt_antal=i;						// antal i oversigt
    mysql_close(conn);
    return(i);
}



// *********************************************************************************
//
// update by search
//
// *********************************************************************************


int musicoversigt_class::opdatere_music_oversigt_searchtxt(char *searchtxt,int search_art) {
    char convert_command[512];
    // char sqlselect[1000];
    std::string sqlselect;
    char dirname[256];
    unsigned int i;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char database[256];
    char icon_file[512];
    char tmptxt[512];
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    clean_music_oversigt();
    write_logfile(logfile,(char *) "Opdatere music oversigt fra database.");
    i=0;
    if (strcmp(searchtxt,"")==0) {			// hent fra starten top music directory
      sqlselect = "select directory_id,path,parent_id from music_directories where parent_id=0 order by path";
    } else {
      if (search_art==0) {
        sqlselect = fmt::format("select music_directories.directory_id,path,parent_id from music_directories left join music_songs on (music_directories.directory_id=music_songs.directory_id) left join music_artists on (music_songs.artist_id=music_artists.artist_id)  where music_artists.artist_name like '%{}%' group by directory_id",searchtxt);
      } else {
        sqlselect = fmt::format("select music_directories.directory_id,path,parent_id from music_directories left join music_songs on (music_directories.directory_id=music_songs.directory_id) left join music_artists on (music_songs.artist_id=music_artists.artist_id) where music_songs.name like '%{}%' group by directory_id",searchtxt);
      }
      /*
      strcpy(musicoversigt[i].album_name,(char *) "   BACK");
      strcpy(musicoversigt[i].album_path,(char *) "");
      strcpy(musicoversigt[i].album_coverfile,(char *) "");
      musicoversigt[i].textureId=0;
      musicoversigt[i].directory_id=0;			// husk directory id
      musicoversigt[i].parent_id=0;
      musicoversigt[i].album_id=0;
      musicoversigt[i].artist_id=0;
      musicoversigt[i].oversigttype=0;
      i++;
      */
    }
    //printf("SQL SELECT = %s \n",sqlselect.c_str());
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect.c_str());
    res = mysql_store_result(conn);
    if (res) {
      while (((row = mysql_fetch_row(res)) != NULL) && (i<MUSIC_OVERSIGT_TYPE_SIZE)) {
        // cout << "Opdatere music oversigt fra database: " << row[1] << std::endl;
        strcpy(dirname,row[1]);
        strcpy(tmptxt,configmusicpath);
        strcat(tmptxt,row[1]);
        strcat(tmptxt,"/Front.jpg");
        if (file_exists(tmptxt)) {
          strcpy(convert_command,"/usr/bin/convert -scale 128 ");
          strcat(convert_command,"\"");
          strcat(convert_command,configmusicpath);
          strcat(convert_command,row[1]);
          strcat(convert_command,"/Front.jpg\"");
          strcat(convert_command," \"");
          strcat(convert_command,configmusicpath);
          strcat(convert_command,row[1]);
          strcat(convert_command,"/");
          strcat(convert_command,"mythcFront.jpg\"");
          strcpy(tmptxt,configmusicpath);
          strcat(tmptxt,row[1]);
          strcat(tmptxt,"/mythcFront.jpg");
          if (!(file_exists(tmptxt))) {
            system(convert_command);
            printf("Do Convert scale image %s to 128*128 \n",row[1]);
          }
          strcpy(icon_file,tmptxt);		// gem icon file name
        } else {
          strcpy(icon_file,"");			// no icon
        }
        strcpy(musicoversigt[i].album_name,dirname);
        strcpy(musicoversigt[i].album_path,"");
        strcpy(musicoversigt[i].album_coverfile,icon_file);
        musicoversigt[i].directory_id=atoi(row[0]);			// husk directory id
        musicoversigt[i].parent_id=atoi(row[2]);
        musicoversigt[i].album_id=0;
        musicoversigt[i].artist_id=0;
        musicoversigt[i].oversigttype=0;  // not playlist
        i++;
      }        	// end while
    } else {
      printf("SQL DATBASE ERROR\n");
      i=0;
    }
    if (i>0) {
      antal_music_oversigt=i-1;
      musicoversigt_antal=i-1;
    } else {
      antal_music_oversigt=0;
      musicoversigt_antal=0;
    }
    if (debugmode & 2) printf("Fundet antal %d \n",i);    
    mysql_close(conn);
    return(i);
}


// ****************************************************************************************
//
// save playlistes in db
//
// ****************************************************************************************

// need cleanup

int musicoversigt_class::save_music_oversigt_playlists(char *playlistname) {
  bool fault;
  char sqlselect[8192];
  char temptxt[2048];
  std::string sqlselect1;
  std::string sqlselect2;
  unsigned int i;
  // mysql vars
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char database[256];
  strcpy(database,dbname);
  //clean_music_oversigt(musicoversigt);
  write_logfile(logfile,(char *) "Save music playlist.");
  i=0;
  conn=mysql_init(NULL);
  // Connect to database
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
  mysql_query(conn,"set NAMES 'utf8'");
  res = mysql_store_result(conn);
  if (conn) {
    snprintf(sqlselect,sizeof(sqlselect),"REPLACE INTO music_playlist (playlist_id,playlist_name,playlist_songs,last_accessed,length,songcount,hostname) values(0,'%s','",playlistname);
    sqlselect1 = fmt::format("REPLACE INTO music_playlist (playlist_id,playlist_name,playlist_songs,last_accessed,length,songcount,hostname) values(0,'{}','",playlistname);
    while (i<aktiv_playlist.numbers_in_playlist()) {
      sprintf(temptxt,"%d",aktiv_playlist.get_songid(i));
      sqlselect1 = sqlselect1 + std::to_string(aktiv_playlist.get_songid(i));
      sqlselect1 = sqlselect1 + " ";
      strcat(sqlselect,temptxt);
      strcat(sqlselect," ");
      i++;
    } // end while
    snprintf(temptxt,sizeof(temptxt),"','%s',%d,%d,'%s')","2018-01-01 00:00:00",0,aktiv_playlist.numbers_in_playlist(),"");
    sqlselect2 = fmt::format("','{}',{},{},'{}')","2018-01-01 00:00:00",0,aktiv_playlist.numbers_in_playlist(),"");
    strcat(sqlselect,temptxt);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) fault=false;
  }
  mysql_close(conn);
  return(!(fault));
}



// ****************************************************************************************
//
// load music play list
//
// ****************************************************************************************

int musicoversigt_class::load_music_oversigt_playlists(char *playlistname) {
  bool fault;
  std::string sqlselect1;
  unsigned int i;
  // mysql vars
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char database[256];
  std::string playlistarray_string;
  std::string songid;
  int pos=0;
  strcpy(database,dbname);
  //clean_music_oversigt(musicoversigt);
  write_logfile(logfile,(char *) "loading music playlist.");
  i=0;
  conn=mysql_init(NULL);
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
  mysql_query(conn,"set NAMES 'utf8'");
  res = mysql_store_result(conn);
  if (conn) {
    sqlselect1 = fmt::format("select playlist_id,playlist_name,playlist_songs,last_accessed,length,songcount,hostname from music_playlist where playlist_name like '{}'",playlistname);
    mysql_query(conn,sqlselect1.c_str());
    res = mysql_store_result(conn);
    playlistarray_string=row[2];    // playlist string have id's
    while((pos=playlistarray_string.find(' ')) != std::string::npos) {
      songid=playlistarray_string.substr(0,pos);
      printf("SONGID = %s \n",songid.c_str());
      playlistarray_string.erase(0,pos+1);
    }

    /*
    while (i<aktiv_playlist.numbers_in_playlist()) {
      sprintf(temptxt,"%d",aktiv_playlist.get_songid(i));
      strcat(sqlselect,temptxt);
      strcat(sqlselect," ");
      i++;
    }        	// end while
    
    snprintf(temptxt,sizeof(temptxt),"','%s',%d,%d,'%s')","2018-01-01 00:00:00",0,aktiv_playlist.numbers_in_playlist(),"");
    strcat(sqlselect,temptxt);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) fault=false;
    */
  }
  mysql_close(conn);
  return(!(fault));
}



// *****************************************************************************
//
// load icons
//
// *****************************************************************************


void musicoversigt_class::opdatere_music_oversigt_icons() {
  unsigned int i;
  char tmpfilename[2000];
  for(i=0;i<antal_music_oversigt;i++) {
    musicoversigt[i].textureId=0;
  }
  i=0;
  while (i<antal_music_oversigt) {
    strncpy(tmpfilename,musicoversigt[i].album_coverfile,200);
    if ((strcmp(tmpfilename,"")!=0) && (file_exists(tmpfilename))) {
      // load covers file into opengl as textures (png/jpg)
      // MAKE CRASH crash
      // musicoversigt[i].textureId = loadTexture((char *) tmpfilename);
    } else {
      musicoversigt[i].textureId=0;
    }
    i++;
  }
}


// *****************************************************************************
//
// clean all
//
// *****************************************************************************


void musicoversigt_class::clean_music_oversigt() {
  unsigned int i=0;
  while (i<antal_music_oversigt) {
    strcpy(musicoversigt[i].album_name,"");
    strcpy(musicoversigt[i].album_path,"");
    strcpy(musicoversigt[i].album_coverfile,"");
    musicoversigt[i].textureId=0;
    musicoversigt[i].directory_id=0;                        // husk directory id
    musicoversigt[i].parent_id=0;
    musicoversigt[i].album_id=0;
    musicoversigt[i].artist_id=0;
    musicoversigt[i].oversigttype=0;
    i++;
  }
  antal_music_oversigt=0;
  do_play=false;
  music_is_playing=false;
}



// *****************************************************************************
//
// Load playlist info
//
// *****************************************************************************


int musicoversigt_class::opdatere_music_oversigt_playlists() {
    char sqlselect[512];
    unsigned int i;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char database[256];
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    clean_music_oversigt();
    write_logfile(logfile,(char *) "Opdatere music oversigt fra database.");
    i=0;
    strcpy(sqlselect,"select playlist_id,playlist_name,last_accessed,length,songcount from music_playlist where hostname='' or playlist_name like 'default_playlist_storage'");
    strcpy(musicoversigt[0].album_name,"   BACK");
    strcpy(musicoversigt[0].album_path,"");
    strcpy(musicoversigt[0].album_coverfile,"");
    musicoversigt[0].textureId=0;
    musicoversigt[0].directory_id=0;			// husk directory id
    musicoversigt[0].parent_id=0;
    musicoversigt[0].album_id=0;
    musicoversigt[0].artist_id=0;
    musicoversigt[0].oversigttype=0;
    i++;
    write_logfile(logfile,(char *) "Loading playlists from internal db.");
    try {
      conn=mysql_init(NULL);
      // Connect to database
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (i<MUSIC_OVERSIGT_TYPE_SIZE)) {
          strcpy(musicoversigt[i].album_name,row[1]);
          strcpy(musicoversigt[i].album_path,"");
          musicoversigt[i].directory_id=atoi(row[0]);			// husk playlist_id
          musicoversigt[i].parent_id=0;
          musicoversigt[i].album_id=0;
          musicoversigt[i].artist_id=0;
          musicoversigt[i].oversigttype=-1;
          i++;
        }        	// end while
      } else {
        printf("SQL DATBASE ERROR\n");
        i=0;
      }
      if (debugmode & 2) printf("Fundet antal %d playlists. \n",i);
      musicoversigt_antal=i;						// antal i oversigt
      mysql_close(conn);
    }
    catch (...) {
      write_logfile(logfile,(char *) "Error process playlist.");
    }
    return(i);
}


// ****************************************************************************************
//
// Show music overview
//
// ****************************************************************************************


void musicoversigt_class::show_music_oversigt(GLuint normal_icon,GLuint back_icon,GLuint dirplaylist_icon,int _mangley,int music_key_selected) {
  int buttonsize=180;
  int buttonsizey=180;
  buttonsize=config_menu.config_music_main_window_icon_sizex;
  buttonsizey=config_menu.config_music_main_window_icon_sizey;
  int i=0;
  int ii=0;
  int lmusicoversigt_antal=(8*5);
  int xx=(float) config_menu.config_music_main_window_sizex/(buttonsize+60);
  int yy=(float) (config_menu.config_music_main_window_sizey/220)+1;
  lmusicoversigt_antal = xx*yy;        // 
  int sofset=0;
  int xof=0;
  int yof=orgwinsizey-(buttonsize);
  char *lastslash;
  char temptxt[200];
  int bonline=8;                    //# of element pr line
  int ofs;
  int pline=0;
  int width = 20;
  bool stop=false;
  char *base,*right_margin;
  int length;
  std::string temptxt1;
  sofset=(_mangley/40)*8;
  while((i<lmusicoversigt_antal) && (strcmp(musicoversigt[i+sofset].album_name,"")!=0) && ((int) i<(int) MUSIC_OVERSIGT_TYPE_SIZE)) {
    // do new line (if not first line)
    if (((i % bonline)==0) && (i>0)) {
      xof=0;
      yof=yof-(buttonsizey+28);
    }
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // show back or opem playlist list
    if (i==0) {
      // view is playlist ?
      if (musicoversigt[i+sofset].oversigttype!=-1) {
        if (i+sofset==0) {
          glBindTexture(GL_TEXTURE_2D,back_icon);
        } else {
          if (musicoversigt[i+sofset].textureId==0) glBindTexture(GL_TEXTURE_2D,normal_icon);
          else glBindTexture(GL_TEXTURE_2D,musicoversigt[i+sofset].textureId);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      } else {
        glBindTexture(GL_TEXTURE_2D,dirplaylist_icon);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }
    } else {
      // else normal icon loader if exist else load default icon (normal_icon) or playlist icon (dirplaylist_icon)
      if (musicoversigt[i+sofset].textureId!=0) {
        glBindTexture(GL_TEXTURE_2D,musicoversigt[i+sofset].textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      } else {
        if (musicoversigt[i+sofset].oversigttype==-1) {
          glBindTexture(GL_TEXTURE_2D,dirplaylist_icon);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
          glBindTexture(GL_TEXTURE_2D,normal_icon);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
      }
    }
    // if selected icon
    if (i+1==music_key_selected) buttonsize=190.0f;
    else buttonsize=180.0f;
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLoadName(100+i);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( xof,yof , 0.0);
    glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsizey, 0.0);
    glTexCoord2f(1, 1); glVertex3f( xof+buttonsize,yof+buttonsizey , 0.0);
    glTexCoord2f(1, 0); glVertex3f( xof+buttonsize,yof , 0.0);
    glEnd();
    glPopMatrix();
    drawLinesOfText(musicoversigt[i+sofset].album_name, xof+4, yof, 0.4f,18,5,1,true);
    /* // old print stuf
    glPushMatrix();
    glTranslatef(xof, yof ,0.0f);
    glColor4f(1.0f, 1.0f, 1.0f,1.0f);				//
    glRasterPos2f(	0.0f, 0.0f);
    strcpy(temptxt,musicoversigt[i+sofset].album_name);      	// album navn
    lastslash=strrchr(temptxt,'/');
    if (lastslash) strcpy(temptxt,lastslash+1);
    //glScalef(20.0, 20.0, 1.0);
    glScalef(configdefaultmusicfontsize, configdefaultmusicfontsize, 1.0);
    glDisable(GL_TEXTURE_2D);
    length=strlen(temptxt);
    base=temptxt;
    // print artist/song name
    const float xInitial = (width / 5) - (strlen(base) / 4);
    const float xTranslation=1.0f-(strlen(base)/1.6f)+2;
    pline=0;
    while(*base) {
      // if text can be on line
      if(length <= width) {
        glTranslatef(xInitial,0.0f,0.0f);
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
      glTranslatef(xInitial,-pline*1.2f,0.0f);
      length -= right_margin-base+1;                         // +1 for the space
      base = right_margin+1;
      if (pline>=2) break;
    }
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
    */
    xof+=210;
    i++;
  }
  if (i==0) {
    // show error message
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D,_textureIdloading);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f((orgwinsizex/3), 200 , 0.0);
    glTexCoord2f(0, 1); glVertex3f((orgwinsizex/3), 200+150, 0.0);
    glTexCoord2f(1, 1); glVertex3f((orgwinsizex/3)+450, 200+150 , 0.0);
    glTexCoord2f(1, 0); glVertex3f((orgwinsizex/3)+450, 200 , 0.0);
    glEnd();
    sprintf(temptxt,"Error no music loaded in db");
    drawText(temptxt, (orgwinsizex/3)+30, 275.0f, 0.4f,1);
  }
}



// ****************************************************************************************
//
// Show search music overview
//
// ****************************************************************************************


void musicoversigt_class::show_search_music_oversigt(GLuint normal_icon,GLuint back_icon,GLuint dirplaylist_icon,int _mangley,int music_key_selected) {
  int buttonsize=180;
  int buttonsizey=180;
  buttonsize=config_menu.config_music_main_window_icon_sizex;
  buttonsizey=config_menu.config_music_main_window_icon_sizey;
  int i=0;
  int ii=0;
  int lmusicoversigt_antal = (8*4);  
  int xx=(float) config_menu.config_music_main_window_sizex/(buttonsize+60);
  int yy=(float) (config_menu.config_music_main_window_sizey/220);
  lmusicoversigt_antal = xx*yy;        // 
  int bonline=xx;                    //# of element pr line full screen 8
  int sofset=0;
  int xof=0;
  int yof=orgwinsizey-(buttonsize*2);
  char *lastslash;
  char temptxt[200];  
  int ofs;
  int pline=0;
  int width = 20;
  bool stop=false;
  char *base,*right_margin;
  int length;
  std::string temptxt1;
  sofset=(_mangley/40)*8;
  char searchstring[200];
  bool cursor=true;
  float xof_top=((orgwinsizex-buttonsize)/2)-(1200/2);
  float yof_top=orgwinsizey-(buttonsizey*1)+20;                               // start ypos
  // top
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // type of search
  switch (searchtype) {
    case 0: glBindTexture(GL_TEXTURE_2D,tidal_big_search_bar_track);
            break;
    case 1: glBindTexture(GL_TEXTURE_2D,tidal_big_search_bar_artist);
            break;
    default:glBindTexture(GL_TEXTURE_2D,tidal_big_search_bar_track);
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(0);
  glBegin(GL_QUADS); 
  glTexCoord2f(0, 0); glVertex3f( xof_top+10, yof_top+10, 0.0);
  glTexCoord2f(0, 1); glVertex3f( xof_top+10,yof_top+buttonsizey-20, 0.0);
  glTexCoord2f(1, 1); glVertex3f( xof_top+1200-10, yof_top+buttonsizey-20 , 0.0);
  glTexCoord2f(1, 0); glVertex3f( xof_top+1200-10, yof_top+10 , 0.0);
  glEnd();
  // show music search string
  glPushMatrix();
  glTranslatef(xof+210+(buttonsize/2),yof+240,0);
  glDisable(GL_TEXTURE_2D);
  glScalef(100, 100, 1.0);
  strcpy(searchstring,keybuffer);
  if (strcmp(searchstring,"")!=0) {
    glcRenderString(searchstring);
  }
  if (cursor) glcRenderString("_"); else glcRenderString(" ");
  glPopMatrix();
  while((i<lmusicoversigt_antal) && (strcmp(musicoversigt[i+sofset].album_name,"")!=0) && ((int) i<(int) MUSIC_OVERSIGT_TYPE_SIZE)) {
    // do new line (if not first line)
    if (((i % bonline)==0) && (i>0)) {
      xof=0;
      yof=yof-(buttonsizey+28);
    }
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // show back or opem playlist list
      // else normal icon loader if exist else load default icon (normal_icon) or playlist icon (dirplaylist_icon)
    if (musicoversigt[i+sofset].textureId!=0) {
      glBindTexture(GL_TEXTURE_2D,musicoversigt[i+sofset].textureId);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
      glBindTexture(GL_TEXTURE_2D,normal_icon);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    // if selected icon
    if (i+1==music_key_selected) buttonsize=190.0f;
    else buttonsize=180.0f;
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLoadName(100+i);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( xof,yof , 0.0);
    glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsizey, 0.0);
    glTexCoord2f(1, 1); glVertex3f( xof+buttonsize,yof+buttonsizey , 0.0);
    glTexCoord2f(1, 0); glVertex3f( xof+buttonsize,yof , 0.0);
    glEnd();
    glPopMatrix();
    drawLinesOfText(musicoversigt[i+sofset].album_name, xof+4, yof, 0.4f,18,5,1,true);
    xof+=210;
    i++;
  }
  if (i==0) {
    // show error message
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D,_textureIdloading);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f((orgwinsizex/3), 200 , 0.0);
    glTexCoord2f(0, 1); glVertex3f((orgwinsizex/3), 200+150, 0.0);
    glTexCoord2f(1, 1); glVertex3f((orgwinsizex/3)+450, 200+150 , 0.0);
    glTexCoord2f(1, 0); glVertex3f((orgwinsizex/3)+450, 200 , 0.0);
    glEnd();
    sprintf(temptxt,"Error no music loaded in db");
    drawText(temptxt, (orgwinsizex/3)+30, 275.0f, 0.4f,1);
  }
}


// ****
