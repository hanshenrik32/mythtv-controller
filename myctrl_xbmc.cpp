#include "myctrl_xbmc.h"

extern char configdefaultmusicpath[256];                      // internal db for music



// xbmc version loader
//

int xbmcsqlite::xbmcloadversion() {
    int rc;
    const char *sql = "SELECT * from version";
    const char* data = "Callback function called";
    char *zErrMsg = 0;
    opensqldb(dbfilename);
    rc = sqlite3_exec(sqlitedb_obj, sql, xbmc_load_sqldb_callback_version, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK ) {
      fprintf(stderr, "XBMC - SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      fprintf(stdout, "XBMC - Music loader Operation done successfully\n");
    }
    sqlite3_close(sqlitedb_obj);
}



// xbmc music db loader

void xbmcsqlite::xbmc_readmusicdb() {
    int rc;
    const char *sql = "SELECT * from songview";
    const char* data = "Callback function called";
    char *zErrMsg = 0;
    opensqldb(dbmusicname);
    // Execute SQL statement
    rc = sqlite3_exec(sqlitedb_obj, sql, xbmc_load_sqldb_callback_music, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK ) {
      fprintf(stderr, "XBMC - SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      fprintf(stdout, "XBMC - Music loader Operation done successfully\n");
    }
    sqlite3_close(sqlitedb_obj);
}





// xbmc movie db loader

void xbmcsqlite::xbmc_readmoviedb() {
    int rc;
    const char *sql = "SELECT * from movieview"; // order by dateAdded";
    const char* data = "Callback function called";
    char *zErrMsg = 0;
    opensqldb(dbmoviename);
    // Execute SQL statement
    rc = sqlite3_exec(sqlitedb_obj, sql, xbmc_load_sqldb_callback_movie, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      fprintf(stdout, "XBMC - Movie loader operation done successfully\n");
    }
    sqlite3_close(sqlitedb_obj);
}





// callback to fill music db in mythtv-controller
int xbmcsqlite::xbmc_load_sqldb_callback_music(void *data, int argc, char **argv, char **azColName) {
  MYSQL *conn;
  MYSQL *conn2;
  MYSQL *conn3;
  MYSQL_RES *res;
  MYSQL_RES *res2;
  MYSQL_RES *res3;
  MYSQL_ROW row;
  MYSQL_ROW row2;
  MYSQL_ROW row3;
  char sqlselect[2048];
  char sqlselect1[2048];
  char sqlselect2[2048];
  char filepath[1024];
  char filename[256];
  char filename1[256];            // full name
  char songname[256];
  char artistname[256];
  char albumname[256];
  char temp[256];
  char *temppointer;
  int year;
  int length;
  int albumid=0;
  int artistid=0;
  int track;
  int sub_dirid;
  int directoryid=0;
  int last_directoryid=0;
  bool dbexist=false;
  bool exist=false;
  bool cr=false;

  int n=0;
  int nn;
  char *tt;

  musicoversigt[0].textureId=0;
  musicoversigt[0].directory_id=0;			// husk directory id
  musicoversigt[0].parent_id=0;
  musicoversigt[0].album_id=0;
  musicoversigt[0].artist_id=0;
  musicoversigt[0].oversigttype=0;
  for(int i=0; i<argc; i++) {

    //printf("i=%d     %s = %s\n",i, azColName[i], argv[i] ? argv[i] : "NULL");

    cr=false;
    if (i==0) {
      musicoversigt[i].textureId=0;
      musicoversigt[i].directory_id=0;			// husk directory id
      musicoversigt[i].parent_id=0;
      musicoversigt[i].album_id=0;
      musicoversigt[i].artist_id=0;
      musicoversigt[i].oversigttype=0;
      strcpy(musicoversigt[i].album_name,"");
      strcpy(musicoversigt[i].album_path,"");
    }
    if (strncmp("strPath",azColName[i],8)==0) {
      strcpy(artistname,argv[i]);

      //printf("Artist name = %s \n",argv[i]);
      // remove default path from search stuf
      if (strncmp(artistname,configdefaultmusicpath,strlen(configdefaultmusicpath))==0) {
        nn=strlen(configdefaultmusicpath);
        if (nn>=0) strcpy(artistname,artistname+nn-1);               // remove the default path
      }
      // loop all dir in string (artistname)
      // and create them in music db directory
      do {
        n=0;
        nn=0;
        tt=strrchr(artistname,'/');
        while ((n<strlen(artistname)) && (nn==0)) {
          if (artistname[n]=='/') {
            nn=n;
          }
          n++;
        };

        strncpy(temp,artistname+1,nn);
        temp[nn-1]=0;

        printf("nn=%d artistname=%s \n ",nn,temp);

        if (file_exists(temp)) {
          // husk sidste dirid
          last_directoryid=directoryid;
          // check if exist
          exist=false;
          conn2=mysql_init(NULL);
          if (conn2) {
            mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0);
            //sprintf(sqlselect1,"search  music_directories after %s \n",temp);
            sprintf(sqlselect1,"select directory_id from music_directories where path like '%s'",temp);
            mysql_query(conn2,sqlselect1);
            res2 = mysql_store_result(conn2);
            if (res2) {
              while ((row2 = mysql_fetch_row(res2)) != NULL) {
                directoryid=atol(row2[0]);
                exist=true;
              }
            }
            mysql_close(conn2);
          }
          if (!(exist)) {
            conn=mysql_init(NULL);
            mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0);
            //      printf("create directoryid %d on artist *%s* on db music_directories \n ",directoryid,temp);
            sprintf(sqlselect2,"insert into music_directories values (%d,'%s',%d)",0,temp,last_directoryid);
            mysql_query(conn,sqlselect2);
            res = mysql_store_result(conn);

            // hent dir id
            sprintf(sqlselect2,"select directory_id from music_directories where path like '%s'",temp);
            mysql_query(conn,sqlselect2);
            res2 = mysql_store_result(conn);
            if (res) {
              while ((row = mysql_fetch_row(res2)) != NULL) {
                directoryid=atol(row2[0]);
              }
            }
            mysql_close(conn);
          }
        }

        // get rest of path
        nn=strlen(temp)+2;
        if (nn>=0) strcpy(temp,artistname+nn-1);               // remove the last done
        strcpy(artistname,temp);
      } while (nn<strlen(artistname));
    }

    if (strncmp("strTitle",azColName[i],8)==0) {
      strcpy(filename,argv[i]);
    }

    if (strncmp("strFileName",azColName[i],8)==0) {
      strcpy(filename1,argv[i]);
    }

    if (strncmp("strArtists",azColName[i],10)==0) {
      strcpy(artistname,argv[i]);
      // check if exist
      conn2=mysql_init(NULL);
      if (conn2) {
        mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0);
        // sprintf(sqlselect1,"insert into music_directories values(%d,'%s',%d)",0,de->d_name,parent);
        sprintf(sqlselect1,"select artist_id from music_artists where artist_name like '%s'",artistname);
        mysql_query(conn2,sqlselect1);
        res2 = mysql_store_result(conn2);
        if (res2) {
          while ((row2 = mysql_fetch_row(res2)) != NULL) artistid=atol(row2[0]);
        }
        mysql_close(conn2);
        if (artistid==0) {
          cr=true;
          // create artist if noy exist
          conn=mysql_init(NULL);
          mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0);
//          printf("create artist %20s ",artistname);
          sprintf(sqlselect2,"insert into music_artists values (%d,'%s')",0,artistname);
          mysql_query(conn,sqlselect2);
          res = mysql_store_result(conn);
          mysql_close(conn);

          // hent artist id
          conn2=mysql_init(NULL);
          mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0);
          sprintf(sqlselect1,"select artist_id from music_artists where artist_name like '%s'",artistname);
          mysql_query(conn2,sqlselect1);
          res2 = mysql_store_result(conn2);
          if (res2) {
            while ((row2 = mysql_fetch_row(res2)) != NULL) artistid=atol(row2[0]);
          }
          mysql_close(conn2);
        }
      }
    }
    if (strncmp("iDuration",azColName[i],9)==0) {
      length=atoi(argv[i]);
    }
    if (strncmp("strTitle",azColName[i],8)==0) {
      strcpy(songname,argv[i]);
    }
    if (strncmp("strPath",azColName[i],7)==0) {
      strcpy(filepath,argv[i]);
    }

    if (strncmp("strAlbum",azColName[i],8)==0) {
      strcpy(albumname,argv[i]);
      conn2=mysql_init(NULL);
      if (conn2) {
        mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0);
        // sprintf(sqlselect1,"insert into music_directories values(%d,'%s',%d)",0,de->d_name,parent);
        sprintf(sqlselect1,"select album_id from music_albums where album_name like '%s'",albumname);
        mysql_query(conn2,sqlselect1);
        res2 = mysql_store_result(conn2);
        if (res2) {
          while ((row2 = mysql_fetch_row(res2)) != NULL) albumid=atol(row2[0]);
        }
        mysql_close(conn2);
      }
      // create album if not exist
      if (albumid==0) {
        cr=true;
//        printf("create album %20s  ",albumname);
        sprintf(sqlselect1,"insert into music_albums values(%d,%d,'%s',%d,%d)",0,artistid,albumname,0,0);
        conn=mysql_init(NULL);
        if (conn) {
          mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0);
          mysql_query(conn,sqlselect1);
          res = mysql_store_result(conn);
          mysql_close(conn);
        }

        // get albumid
        conn2=mysql_init(NULL);
        if (conn2) {
          mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0);
          // sprintf(sqlselect1,"insert into music_directories values(%d,'%s',%d)",0,de->d_name,parent);
          sprintf(sqlselect1,"select album_id from music_albums where album_name like '%s'",albumname);
          mysql_query(conn2,sqlselect1);
          res2 = mysql_store_result(conn2);
          if (res2) {
            while ((row2 = mysql_fetch_row(res2)) != NULL) albumid=atol(row2[0]);
          }
          mysql_close(conn2);
        }
      }
    }
//    if (cr) printf("\n");
  }
  strcpy(temp,filepath);

  char *lastpath;
  char sted[255];
  int aaa=strlen(filepath);
  if (aaa) aaa-=2;
  while((aaa>0) && (filepath[aaa]!='/')) {
    aaa--;
  }
  strcpy(sted,filepath+(aaa+1));
  aaa=strlen(sted);
  sted[aaa-1]=0;
  /*
  conn2=mysql_init(NULL);
  if (conn2) {
    mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0);

    sprintf(sqlselect1,"select directory_id from music_directories where path like '%s'",sted);
    mysql_query(conn2,sqlselect1);
    res2 = mysql_store_result(conn2);
    if (res2) {
      while ((row2 = mysql_fetch_row(res2)) != NULL) {
        directoryid=atol(row2[0]);
        exist=true;
      }
    }
    mysql_close(conn2);
  }
*/
  strcat(temp,filename1);

  //strcat(temp,songname);

  printf("dirid %d artistid=%4d albumid=%4d path %s songname=%10s \n",directoryid,artistid,albumid,sted,songname);

  //printf("Song name %s \n",songname);
  sprintf(sqlselect,"insert into music_songs(song_id,filename,  name,    track, artist_id, album_id, genre_id, year, length, numplays, rating, lastplay,             date_entered,           date_modified,          format , mythdigest, size , description, comment, disc_count, disc_number, track_count, start_time, stop_time, eq_preset, relative_volume, sample_rate, bitrate, bpm, directory_id) values \
                (%d,    '%s',      '%s',    %d,    %d,        %d,       %d,       %d,    %d,     %d,      %d,     '%s',                 '%s',                   '%s',                   '%s',    '%s',        %d,   '%s',        '%s',    %d,         %d,          %d,          %d,          %d,        '%s',       %d,             %d,          %d,      %d,     %d)", \
                0,      temp,songname,0,    artistid,  albumid,   0,        0,     0,      0,       0,     "2012-01-01 00:00:00",   "2012-01-01 00:00:00","2012-01-01 00:00:00",  "",      "",          0,    "",          "",      0,          0,           0,           0,           0,         "",         0,              0,           0,       0,directoryid);
  conn=mysql_init(NULL);
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass,"mythtvcontroller", 0, NULL, 0);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    mysql_close(conn);
  }

  //printf("XBMC - Antal xbmc music loaded %d \n",argc);
  return(0);
}
