#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>
#include <mysql.h>
#include <GL/glc.h>
#include <sys/types.h>                                    // listdir.c
#include <dirent.h>
#include <stdio.h>


#include "myctrl_music.h"
#include "myctrl_mplaylist.h"
#include "utility.h"
#include "readjpg.h"
#include "text3d.h"
#include "myth_ttffont.h"

#include "myth_saver.h"

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

// ****************************************************************************************
//
// parameret til mysql er dirid = directory_id i database
// retunere path og parent_id som bliver udfyldt fra mysql databasen
//
// ****************************************************************************************

void hent_dir_id(char *path,char *parent_id,char *dirid) {
    // mysql stuf
    char database[256];
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    char sqlselect[256];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    strcpy(sqlselect,"select parent_id,path from music_directories where directory_id=");
    strcat(sqlselect,dirid);
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
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
    sprintf(temp,"%d",dirid);
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
      write_logfile((char *) "Mysql db connect error");
    }
    return(returid);
}

// ****************************************************************************************
//
// ****************************************************************************************

void clean_music_oversigt(music_oversigt_type musicoversigt[])

{
    unsigned int i=0;
    while (i<MUSIC_OVERSIGT_TYPE_SIZE) {
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
}

// ****************************************************************************************
//
// check about cd cover to created from
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
    char database[256];
    char sqlselect[1024];
    // mysql vars
    MYSQL *conn2;
    MYSQL_RES *res2;
    MYSQL_ROW row2;
    int artistid=0;
    char sqlselect1[1024];
    // hent atrist id
    try {
      conn2=mysql_init(NULL);
      if (conn2) {
        mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
        artistid=0;
        // sprintf(sqlselect1,"insert into music_directories values(%d,'%s',%d)",0,de->d_name,parent);
        sprintf(sqlselect1,"select artist_id from music_artists where artist_name like '%s'",artistname);
        mysql_query(conn2,sqlselect1);
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
      write_logfile((char *) "Mysql db connect error");
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
      sprintf(sqlselect1,"select song_id from music_songs where filename like '%s' and name like '%s'",filename,name);
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
    write_logfile((char *) "Mysql db connect error");
  }
  return(songid);
}

// ****************************************************************************************
//
// build music database from path (dirpath)
// return >0 if error
//
// ****************************************************************************************

int opdatere_music_oversigt_nodb(char *dirpath,music_oversigt_type musicoversigt[]) {
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
  char sqlselect[4096];
  char sqlselect1[4096];
  char sqlselect2[4096];
  char checkdir[4096];
  char checkdir2[4096];
  MYSQL *conn;
  MYSQL *conn1;
  MYSQL *conn2;
  MYSQL_RES *res;
  MYSQL_RES *res1;
  MYSQL_RES *res2;
  MYSQL_RES *res3;
  MYSQL_ROW row;
  MYSQL_ROW row2;
  MYSQL_ROW row3;
  int a;
  char filetype[10];
  char songname[1024];
  int dbexist=0;                              // use to check db exist
  clean_music_oversigt(musicoversigt);				// clear music oversigt
  dirp=opendir(dirpath);
  if (dirp==NULL) {
    printf("Open dir error ->%s<-\n",dirpath);
    return 1;
    exit(0);
  }
  conn=mysql_init(NULL);
  // Connect to database
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    // test fpom musik table exist
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
      write_logfile((char *) "Creating database for music.");
      strcpy(sqlselect,"create table IF NOT EXISTS music_directories(directory_id int NOT NULL AUTO_INCREMENT PRIMARY KEY,path text, parent_id int)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS music_albums(album_id int NOT NULL AUTO_INCREMENT PRIMARY KEY, artist_id int, album_name varchar(255) ,year int, compilation int)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS music_songs(song_id int NOT NULL AUTO_INCREMENT PRIMARY KEY,filename text,name varchar(255),track int, artist_id int, album_id int, genre_id int,year int,length int,numplays int,rating int,lastplay datetime, date_entered  datetime, date_modified datetime,format varchar(4), mythdigest varchar(255) ,size int,description  varchar(255), comment varchar(255), disc_count int, disc_number int, track_count  int, start_time int, stop_time int, eq_preset varchar(255),relative_volume int, sample_rate int, bitrate int,bpm int, directory_id int)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS music_artists(artist_id int NOT NULL AUTO_INCREMENT PRIMARY KEY, artist_name varchar(255))");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS music_genres(genre_id int NOT NULL AUTO_INCREMENT PRIMARY KEY, genre varchar(255))");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS music_playlist(playlist_id int NOT NULL AUTO_INCREMENT PRIMARY KEY, playlist_name varchar(255),playlist_songs text,last_accessed datetime,length int,songcount int,hostname varchar(64))");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
    }
    if (true) {
      //create table music_directories(directory_id int,path text, parent_id int);
      //create table music_albums(album_id  int, artist_id int, album_name   varchar(255) ,year int, compilation int);
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
        //printf("Loading directory %s\n",dirpath);
        // create db over all dirs in start path
        while(de = readdir(dirp)) {
          if ((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0)) {
            // if dir
            if (de->d_type==DT_DIR) {
              sprintf(sqlselect,"insert into music_directories(directory_id,path,parent_id) values(%d,'%s',%d)",0,de->d_name,0);  //
              mysql_query(conn,sqlselect);
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
              sprintf(sqlselect2,"insert into music_artists values (%d,'%s')",0,de->d_name);
              mysql_query(conn,sqlselect2);
              res = mysql_store_result(conn);
              i++;
            }
          }
        }
        // fill database music_albums from dir on music_directories
        strcpy(sqlselect,"select directory_id ,path ,parent_id from music_directories");
        if (conn) {
          mysql_query(conn,sqlselect);
          res = mysql_store_result(conn);
          // loop dirs database names from root / (music dir start path)
          while ((row = mysql_fetch_row(res)) != NULL) {
            if (debugmode & 2) printf("Checking dir %s/%s \n",dirpath,row[1]);
            dirid=atoi(row[0]);
            sprintf(checkdir,"%s/%s",dirpath,row[1]);
            dirp1=opendir(checkdir);
            // error handler
            if (dirp1==NULL) {
              printf("Open dir error ->%s<-\n",dirpath);
              return 1;
              exit(0);
            }
            artistid=0;
            // hent atrist id
            conn2=mysql_init(NULL);
            if (conn2) {
              mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
              // sprintf(sqlselect1,"insert into music_directories values(%d,'%s',%d)",0,de->d_name,parent);
              sprintf(sqlselect1,"select artist_id from music_artists where artist_name like '%s'",row[1]);
              mysql_query(conn2,sqlselect1);
              res2 = mysql_store_result(conn2);
              if (res2) {
                while ((row2 = mysql_fetch_row(res2)) != NULL) {
                  artistid=atol(row2[0]);
                }
              }
              mysql_close(conn2);
            }
            if (dirp1) {
              // loop the dir and create the music records and music_dir if mount in loop dir
              while(de = readdir(dirp1)) {
                if ((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0)) {
                  // if dir
                  // opret i album db
                  if (de->d_type==DT_DIR) {
                    conn2=mysql_init(NULL);
                    if (conn2) {
                      mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
                      sprintf(sqlselect1,"insert into music_albums(album_id,artist_id,album_name,year,compilation) values(%d,%d,'%s',%d,%d)",0,artistid,de->d_name,0,0);
                      mysql_query(conn2,sqlselect1);
                      res2 = mysql_store_result(conn2);
                      // husk last dir vi kommer fra
                      parent_dir_id=atoi(row[0]);
                      // create dir id for subdir
                      sprintf(sqlselect1,"insert into music_directories(directory_id,path,parent_id) values(%d,'%s',%d)",0,de->d_name,parent_dir_id);
                      mysql_query(conn2,sqlselect1);
                      res2 = mysql_store_result(conn2);
                      // hent dirid der lige er oprettet
                      sprintf(sqlselect1,"select directory_id from music_directories where path like '%s'",de->d_name);
                      mysql_query(conn2,sqlselect1);
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
                        sprintf(sqlselect1,"select album_id from music_albums where album_name like '%s'",de->d_name);
                        mysql_query(conn2,sqlselect1);
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
                    sprintf(checkdir2,"%s/%s",checkdir,de->d_name);
                    if (debugmode & 2) printf("\t Checking sub dir %s \n",de->d_name);
                    dirp2=opendir(checkdir2);
                    if (dirp2==NULL) {
                      printf("Open dir error ->%s<-\n",checkdir2);
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
                          // add foumnd  path
                          strcpy(songname,checkdir2);
                          strcat(songname,"/");
                          strcat(songname,de2->d_name);
                          if (song_exist_in_db(songname,songname)) printf("SONG EXIST **********************************");
                          sprintf(sqlselect1,"insert into music_songs(song_id,filename,  name,    track, artist_id, album_id, genre_id, year, length, numplays, rating, lastplay,             date_entered,           date_modified,          format , mythdigest, size , description, comment, disc_count, disc_number, track_count, start_time, stop_time, eq_preset, relative_volume, sample_rate, bitrate, bpm, directory_id) values \
                                        (%d,    '%s',      '%s',    %d,    %d,        %d,       %d,       %d,    %d,     %d,      %d,     '%s',                 '%s',                   '%s',                   '%s',    '%s',        %d,   '%s',        '%s',    %d,         %d,          %d,          %d,          %d,        '%s',       %d,             %d,          %d,      %d,     %d)", \
                                        0,      songname,songname,0,    artistid,  albumid,   0,        0,     0,      0,       0,     "2012-01-01 00:00:00",   "2012-01-01 00:00:00","2012-01-01 00:00:00",  "",      "",          0,    "",          "",      0,          0,           0,           0,           0,         "",         0,              0,           0,       0,sub_dirid);
                          // show in music overview loader
                          strcpy(music_db_update_loader,de->d_name);
                          music_oversigt_loaded_nr++;
                          conn1=mysql_init(NULL);
                          if (conn1) {
                            mysql_real_connect(conn1, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
                            mysql_query(conn1,sqlselect1);
                            res1 = mysql_store_result(conn1);
                            mysql_close(conn1);
                          }
                        }
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
                      if (song_exist_in_db(songname,songname)) printf("SONG EXIST **********************************");
                      sprintf(sqlselect1,"insert into music_songs(song_id,filename,  name,    track, artist_id, album_id, genre_id, year, length, numplays, rating, lastplay,             date_entered,           date_modified,          format , mythdigest, size , description, comment, disc_count, disc_number, track_count, start_time, stop_time, eq_preset, relative_volume, sample_rate, bitrate, bpm, directory_id) values \
                                                                  (%d,    '%s',      '%s',    %d,    %d,        %d,       %d,       %d,    %d,     %d,      %d,     '%s',                 '%s',                   '%s',                   '%s',    '%s',        %d,   '%s',        '%s',    %d,         %d,          %d,          %d,          %d,        '%s',       %d,             %d,          %d,      %d,     %d)", \
                                                                   0,      songname,songname,0,    artistid,  albumid,   0,        0,     0,      0,       0,     "1970-01-01 00:00:00",   "1970-01-01 00:00:00","1970-01-01 00:00:00",  "",      "",          0,    "",          "",      0,          0,           0,           0,           0,         "",         0,              0,           0,       0,dirid);
                      conn1=mysql_init(NULL);
                      if (conn1) {
                        mysql_real_connect(conn1, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
                        mysql_query(conn1,sqlselect1);
                        res1 = mysql_store_result(conn1);
                        mysql_close(conn1);
                      }
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
  if (i) return 0; else return 1;
}

// ****************************************************************************************
//
// update_music_oversigt
// update list pr dir id from mythtv-backend or internal loader
//
// ****************************************************************************************

int opdatere_music_oversigt(music_oversigt_type musicoversigt[],unsigned int directory_id) {
    char fundetpath[512];
    char convert_command[512];
    char sqlselect[512];
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
    // select the right db to update from
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    clean_music_oversigt(musicoversigt);				// clear music oversigt
    if (debugmode & 2) printf("Opdatere music oversigt fra database : %s \n",database);
    i=0;
    if (directory_id==0) {			// hent fra starten top music directory
      strcpy(sqlselect,"select directory_id,path,parent_id from music_directories where parent_id=0 order by path");
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
    } else {
      sprintf(sqlselect,"select directory_id,path,parent_id from music_directories where parent_id=%d",directory_id);		 // dir id in mythtv mysql
      strcpy(musicoversigt[0].album_name,"   BACK");
      strcpy(musicoversigt[0].album_path,"");
      strcpy(musicoversigt[0].album_coverfile,"");
      musicoversigt[0].textureId=0;
      musicoversigt[0].directory_id=hent_parent_dir_id(directory_id); // husk directory id
      musicoversigt[0].parent_id=0;
      musicoversigt[0].album_id=0;
      musicoversigt[0].artist_id=0;
      musicoversigt[0].oversigttype=0;			// type 0 = dirid
      i++;
    }
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
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
        musicoversigt[i].oversigttype=0;
        i++;
      }        	// end while
    } else {
      write_logfile((char *) "mysql sql database err.");
      i=0;
    }
    if (i>0) {
      if (debugmode & 2) printf(" %d CD Covers loaded.\n",i);
    }
    musicoversigt_antal=i;						// antal i oversigt
    mysql_close(conn);
    return(i);
}



// ****************************************************************************************
//
// update list med playlistes
//
// ****************************************************************************************

int opdatere_music_oversigt_playlists(music_oversigt_type musicoversigt[]) {
    char sqlselect[512];
    unsigned int i;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char database[256];
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    clean_music_oversigt(musicoversigt);
    write_logfile((char *) "Opdatere music oversigt fra database.");
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
    write_logfile((char *) "Loading playlists from internal db.");
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
      write_logfile((char *) "Error process playlist.");
    }
    return(i);
}

// ****************************************************************************************
//
// save playlistes in db
//
// ****************************************************************************************

int save_music_oversigt_playlists(music_oversigt_type musicoversigt[],char *playlistname) {
  bool fault;
  char sqlselect[8192];
  char temptxt[2048];
  unsigned int i;
  // mysql vars
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char database[256];
  strcpy(database,dbname);
  //clean_music_oversigt(musicoversigt);
  write_logfile((char *) "Save music playlist.");
  i=0;
  conn=mysql_init(NULL);
  // Connect to database
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
  mysql_query(conn,"set NAMES 'utf8'");
  res = mysql_store_result(conn);
  if (conn) {
    sprintf(sqlselect,"REPLACE INTO music_playlist (playlist_id,playlist_name,playlist_songs,last_accessed,length,songcount,hostname) values(0,'%s','",playlistname);
    while (i<aktiv_playlist.numbers_in_playlist()) {
      sprintf(temptxt,"%d",aktiv_playlist.get_songid(i));
      strcat(sqlselect,temptxt);
      strcat(sqlselect," ");
      i++;
    }        	// end while
    sprintf(temptxt,"','%s',%d,%d,'%s')","2018-01-01 00:00:00",0,aktiv_playlist.numbers_in_playlist(),"");
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
// ****************************************************************************************

int load_music_oversigt_playlists(music_oversigt_type musicoversigt[]) {
  return(0);
}

// ****************************************************************************************
//
// update music struct after search
//
// ****************************************************************************************

int opdatere_music_oversigt_searchtxt(music_oversigt_type musicoversigt[],char *searchtxt,int search_art) {
    char convert_command[512];
    char sqlselect[1000];
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
    clean_music_oversigt(musicoversigt);
    write_logfile((char *) "Opdatere music oversigt fra database.");
    i=0;
    if (strcmp(searchtxt,"")==0) {			// hent fra starten top music directory
      strcpy(sqlselect,"select directory_id,path,parent_id from music_directories where parent_id=0 order by path");
    } else {
      if (search_art==0) {
        strcpy(sqlselect,"select music_directories.directory_id,path,parent_id from music_directories left join music_songs on (music_directories.directory_id=music_songs.directory_id) left join music_artists on (music_songs.artist_id=music_artists.artist_id)  where music_artists.artist_name like '%");
      } else {
        strcpy(sqlselect,"select music_directories.directory_id,path,parent_id from music_directories left join music_songs on (music_directories.directory_id=music_songs.directory_id) left join music_artists on (music_songs.artist_id=music_artists.artist_id)  where music_songs.name like '%");
      }
      strcat(sqlselect,searchtxt);
      strcat(sqlselect,"%' group by directory_id");
      strcpy(musicoversigt[0].album_name,(char *) "   BACK");
      strcpy(musicoversigt[0].album_path,(char *) "");
      strcpy(musicoversigt[0].album_coverfile,(char *) "");
      musicoversigt[0].textureId=0;
      musicoversigt[0].directory_id=0;			// husk directory id
      musicoversigt[0].parent_id=0;
      musicoversigt[0].album_id=0;
      musicoversigt[0].artist_id=0;
      musicoversigt[0].oversigttype=0;
      i++;
    }
    //printf("SQL SELECT = %s \n",sqlselect);
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while (((row = mysql_fetch_row(res)) != NULL) && (i<MUSIC_OVERSIGT_TYPE_SIZE)) {
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
        musicoversigt[i].oversigttype=0;
        i++;
      }        	// end while
    } else {
      printf("SQL DATBASE ERROR\n");
      i=0;
    }
    if (debugmode & 2) printf("Fundet antal %d CD Covers. \n",i);
    musicoversigt_antal=i;						// antal i oversigt
    mysql_close(conn);
    return(i);
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
  char sqlselect[50];
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
  return(dbexist);
}

// ****************************************************************************************
//
// loader valgte play liste
//
// ****************************************************************************************

void get_music_pick_playlist(long find_dir_id,bool *music_list_select_array) {
    GLuint texture; //The id of the texture
    char database[256];
    char tmptxt[512];
    char tmptxt1[512];
    char tmptxt2[512];
    char tmptxt3[512];			// ny cover som skal loades
    char husk_tmptxt3[250];		// husk ny cover
    char sqlselect[512];
    int i;
    char parent_id[200];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    sprintf(tmptxt,"%ld",find_dir_id);
    strcpy(sqlselect,"select song_id,filename,directory_id,music_albums.album_name,name,music_artists.artist_id,music_artists.artist_name,length from music_songs,music_artists,music_albums where directory_id=");
    strcat(sqlselect,tmptxt);
    strcat(sqlselect," and music_artists.artist_id=music_songs.artist_id and music_songs.album_id=music_albums.album_id order by name");
//    if (debugmode) printf("SQLSELECT = %s  \n ",sqlselect);
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
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
        sprintf(tmptxt2,"%s",row[2]);		         	// hent dir id
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
}

// ****************************************************************************************
//
// load music conver gfx
//
// ****************************************************************************************

int load_music_covergfx(music_oversigt_type musicoversigt[]) {
    unsigned int i=0; // hent alle music convers
    char tmpfilename[200];
    while (i<MUSIC_OVERSIGT_TYPE_SIZE-1) {
      strcpy(tmpfilename,"");
      if (strcmp(tmpfilename,musicoversigt[i].album_coverfile)==0) strcpy(tmpfilename,"images/");
      strcat(tmpfilename,musicoversigt[i].album_coverfile);
      if ((strcmp(tmpfilename,"images/")!=0) && (file_exists(tmpfilename))) {
        if (musicoversigt[i].textureId==0) musicoversigt[i].textureId = loadTexture(tmpfilename);
      }
      i++;
    }
    return(i);
}

// ****************************************************************************************
//
// show music oversigt
// denne som bruges **************************
// this in use in main
//
// ****************************************************************************************

void show_music_oversigt(music_oversigt_type *musicoversigt,GLuint normal_icon,GLuint back_icon,GLuint dirplaylist_icon,GLuint dirplaylist_icon_mask,int _mangley,int music_key_selected) {
    int buttonsize=180;
    int buttonsizey=180;
    int i=0;
    int ii=0;
    int lmusicoversigt_antal=(5*8);
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
    sofset=(_mangley/40)*8;
    while((i<lmusicoversigt_antal) && (strcmp(musicoversigt[i+sofset].album_name,"")!=0) && ((int) i<(int) MUSIC_OVERSIGT_TYPE_SIZE)) {
      // do new line (if not first line)
      if (((i % bonline)==0) && (i>0)) {
        xof=0;
        yof=yof-(buttonsizey+44);
      }
      glPushMatrix();
      glEnable(GL_TEXTURE_2D);
      //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      // show back or opem playlist list
      if (i==0) {
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
      glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsize, 0.0);
      glTexCoord2f(1, 1); glVertex3f( xof+buttonsize,yof+buttonsize , 0.0);
      glTexCoord2f(1, 0); glVertex3f( xof+buttonsize,yof , 0.0);
      glEnd();
      glPopMatrix();
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
      while(*base) {
        // if text can be on line
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
        glTranslatef(1.0f-(strlen(base)/1.6f)+2,-pline*1.2f,0.0f);
        length -= right_margin-base+1;                         // +1 for the space
        base = right_margin+1;
        if (pline>=2) break;
      }
      glEnable(GL_TEXTURE_2D);
      glPopMatrix();
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
      glPushMatrix();
      glTranslatef((orgwinsizex/3)+30, 275.0f , 0.0f);
      glDisable(GL_TEXTURE_2D);
      glScalef(24.0, 24.0, 1.0);
      glColor3f(0.6f, 0.6f, 0.6f);
      sprintf(temptxt,"Error no music loaded in db");
      glcRenderString(temptxt);
      glPopMatrix();
    }
}
