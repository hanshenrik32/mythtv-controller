#include "myctrl_xbmc.h"
#include <string.h>
#include <libxml/parser.h>

extern char configdefaultmusicpath[256];                      // internal db for music
//extern int debugmode;
extern char *dbname;                                           // internal database name in mysql (music,movie,radio)




// ****************************************************************************************
//
// hent path(s) fra kodi xml file
//
// ****************************************************************************************

int xbmcsqlite::getxmlfilepath() {

  char userhomedir[200];
  char path[1024];
  xmlDoc *document;
  xmlNode *root, *first_child, *node;
  getuserhomedir(userhomedir);
  strcpy(path,userhomedir);
  //strcat(path,"/.kodi/userdata/sources.xml");
  strcat(path,"/tvguide.xml");
  document = xmlReadFile(path, NULL, 0);
  if (document) {
    root = xmlDocGetRootElement(document);
    //fprintf(stdout, "Root is <%s> \n", root->name);
    first_child = root->children;
    for (node = first_child; node; node = node->next) {
      //fprintf(stdout, "\t Child is <%s> (%i) \n", node->name,node->type);
      if (strcmp((char *) node->name,"<video>")==0) {
      }
    }
    fprintf(stdout, "...\n");
    xmlFreeDoc(document);
  }
  return(1);
}



// ****************************************************************************************
//
// xbmc version loader
//
// ****************************************************************************************


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
      fprintf(stdout, "XBMC - version check successfully\n");
    }
    sqlite3_close(sqlitedb_obj);
    return(1);
}



// ****************************************************************************************
// xbmc music db loader
//
// ****************************************************************************************

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




// ****************************************************************************************
//
// xbmc movie db loader
//
// ****************************************************************************************


void xbmcsqlite::xbmc_readmoviedb() {
    int rc;
    const char *sql = "SELECT * from movie_view"; // order by dateAdded";
    const char* data = "Callback function called";
    char *zErrMsg = 0;
    opensqldb(dbmoviename);
    // Execute SQL statement
    rc = sqlite3_exec(sqlitedb_obj, sql, xbmc_load_sqldb_callback_movie, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK ) {
      if (zErrMsg) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
      }
      //exit(0);
    } else {
      fprintf(stdout, "XBMC - Movie loader operation done successfully\n");
    }
    sqlite3_close(sqlitedb_obj);
}

// ****************************************************************************************
//
//
//
// ****************************************************************************************

char search_and_replace(char *text) {
  int n=0;
  int nn=0;
  char newtext[2048];
  strcpy(newtext,"");
  while(n<strlen(text)) {
    if (text[n]=='\'') {
      strcat(newtext,"\\");
      strcat(newtext,"\'");
      n++;
      nn++;
      nn++;
    } else {
      newtext[nn]=text[n];
      nn++;
      n++;
    }

  }
  newtext[n]=0;
  strcpy(text,newtext);
}


// ****************************************************************************************
//
// callback to fill movie db in mythtv-controller from xbmc/sqlite
//
// ****************************************************************************************

int xbmcsqlite::xbmc_load_sqldb_callback_movie(void *data, int argc, char **argv, char **azColName) {
  MYSQL *conn;
  MYSQL *conn1;
  MYSQL_RES *res;
  MYSQL_ROW row;
  int n=0;
  int i;
  char *ext;
  char sqlselect[4096];
  char moviepath[1024];
  char moviepath1[1024];
  char movietitle[1024];
  char moviescoverfile[2048];
  int movieyear;
  char moviecategory[1024];
  char moviedateadded[64];
  char moviesubtitle[1024];
  char moviescontry[80];
  char movietrailerlink[1024];
  char movieimdb[80];
  char moviecover[4096];
  float movieuserrating;
  int movielength;
  char c22[256];
  char c00[256];
  char movieplot[4096];
  bool fundet;
  char fullcovertxt[1024];
  char *coverpointer;
  char filetodownload[2048];
  for(int i=0; i<argc; i++) {
    // file path
    fundet=false;
    if (strncmp("strPath",azColName[i],7)==0) {
      strcpy(moviepath,argv[i]);
      sprintf(sqlselect,"select path from videopathinfo where path='%s'",moviepath);
      conn1=mysql_init(NULL);
      if (conn1) {
        mysql_real_connect(conn1, configmysqlhost,configmysqluser, configmysqlpass,dbname, 0, NULL, 0);
        mysql_query(conn1,sqlselect);
        res = mysql_store_result(conn1);
        fundet=false;
        if (res) {
          while ((row = mysql_fetch_row(res)) != NULL) fundet=true;
        }
        mysql_close(conn1);
      }
      // create if not exist
      if (!(fundet)) {
        sprintf(sqlselect,"insert into videopathinfo values(%d,'%s',%d,%d,%d)",0,moviepath,0,0,0);
        conn1=mysql_init(NULL);
        if (conn1) {
          mysql_real_connect(conn1, configmysqlhost,configmysqluser, configmysqlpass,dbname, 0, NULL, 0);
          mysql_query(conn1,sqlselect);
          res = mysql_store_result(conn1);
          mysql_close(conn1);
        }
      }
    }
    if (azColName[i]) {
      // movie name
      if (strncmp("c00",azColName[i],3)==0) {
        strcpy(movietitle,argv[i]);
      }
      // plot in db
      if (strncmp("c01",azColName[i],3)==0) {
        strcpy(movieplot,argv[i]);
      }
      // userrating in db
      if (strncmp("c05",azColName[i],3)==0) {
        movieuserrating=atof(argv[i]);
      }
      // year
      if ((azColName[i]) && (strncmp("c07",azColName[i],3)==0)) {
        if (argv[i]) movieyear=atoi(argv[i]);
      }
      // movie cover
      if ((azColName[i]) && (strncmp("c08",azColName[i],3)==0)) {
        if (argv[i]) strcpy(fullcovertxt,argv[i]);
      }
      // imdb nr
      if (strncmp("c09",azColName[i],3)==0) {
        strcpy(movieimdb,argv[i]);
      }
      // length of movie
      if (strncmp("c11",azColName[i],3)==0) {
        movielength=atoi(argv[i]);
        movielength=movielength/60; // calc to min
      }
      // category
      // create if not exist
      if (strncmp("c14",azColName[i],3)==0) {
        fundet=false;
        strcpy(moviecategory,argv[i]);
        // check if exist
        sprintf(sqlselect,"select category from videocategory where category like '%s'",moviecategory);
        conn1=mysql_init(NULL);
        if (conn1) {
          mysql_real_connect(conn1, configmysqlhost,configmysqluser, configmysqlpass,dbname, 0, NULL, 0);
          mysql_query(conn1,sqlselect);
          res = mysql_store_result(conn1);
          if (res) {
            while ((row = mysql_fetch_row(res)) != NULL) fundet=true;
          }
          mysql_close(conn1);
        }
        if (!(fundet)) {
          sprintf(sqlselect,"insert into videocategory(intid , category) values (%d,'%s')",0,moviecategory);
          conn1=mysql_init(NULL);
          if (conn1) {
            mysql_real_connect(conn1, configmysqlhost,configmysqluser, configmysqlpass,dbname, 0, NULL, 0);
            mysql_query(conn1,sqlselect);
            res = mysql_store_result(conn1);
            mysql_close(conn1);
          }
        }
      }
      // sub title
      if (strncmp("c16",azColName[i],3)==0) {
        strcpy(moviesubtitle,argv[i]);
      }
      // youtube trailer link
      if (strncmp("c19",azColName[i],3)==0) {
        strcpy(movietrailerlink,argv[i]);
      }
      // cover/screenshot file
      if (strncmp("c20",azColName[i],3)==0) {
        strcpy(moviescoverfile,argv[i]);
      }
      // contry (name)
      if (strncmp("c21",azColName[i],3)==0) {
        strcpy(moviescontry,argv[i]);
      }
      // link to file to play (path+filername)
      if (strncmp("c22",azColName[i],3)==0) {
        strcpy(moviepath1,argv[i]);
      }
      if (strncmp("dateAdded",azColName[i],9)==0) {
        strcpy(moviedateadded,argv[i]);
      }
    }
  }
  // sample from kodi
  // <thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/il9XWx5CbNd2KdDUwrcClEZiLkv.jpg">http://image.tmdb.org/t/p/original/il9XWx5CbNd2KdDUwrcClEZiLkv.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/fIHF63oznk2PXlYM6pPfhnOB3SD.jpg">http://image.tmdb.org/t/p/original/fIHF63oznk2PXlYM6pPfhnOB3SD.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/2LXAj56D3NKuJuT2jnD7CpDpJIG.jpg">http://image.tmdb.org/t/p/original/2LXAj56D3NKuJuT2jnD7CpDpJIG.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/n7W7iUtfY9ll87AQPVqRwIdxNCY.jpg">http://image.tmdb.org/t/p/original/n7W7iUtfY9ll87AQPVqRwIdxNCY.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/hEMWpQbMoAzd0UpSpn9f1DMyRHi.jpg">http://image.tmdb.org/t/p/original/hEMWpQbMoAzd0UpSpn9f1DMyRHi.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/fa4PxEPRKWRyjzYje1jM4m30qzd.jpg">http://image.tmdb.org/t/p/original/fa4PxEPRKWRyjzYje1jM4m30qzd.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/oVkzfeIi8SugSjFeGCcWJ8CbZlK.jpg">http://image.tmdb.org/t/p/original/oVkzfeIi8SugSjFeGCcWJ8CbZlK.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/q8uffTLftPBftvHZ6EPjVARKRsf.jpg">http://image.tmdb.org/t/p/original/q8uffTLftPBftvHZ6EPjVARKRsf.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/i722AJYKvyZC2dSLVM0UJEYNw4M.jpg">http://image.tmdb.org/t/p/original/i722AJYKvyZC2dSLVM0UJEYNw4M.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/lSm7UUirk30xcEmMkphirrEkMl8.jpg">http://image.tmdb.org/t/p/original/lSm7UUirk30xcEmMkphirrEkMl8.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/jDlYj9nhIOIXb3F1G6hheRQBh4o.jpg">http://image.tmdb.org/t/p/original/jDlYj9nhIOIXb3F1G6hheRQBh4o.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/igrxC4ZwP9ZLq00tG66YSJcGRQR.jpg">http://image.tmdb.org/t/p/original/igrxC4ZwP9ZLq00tG66YSJcGRQR.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/9XHePtcuQlu7oTNrX1VUOKt2qnE.jpg">http://image.tmdb.org/t/p/original/9XHePtcuQlu7oTNrX1VUOKt2qnE.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/dg6NYC9WIBUDtF6Dn0szijL5s2f.jpg">http://image.tmdb.org/t/p/original/dg6NYC9WIBUDtF6Dn0szijL5s2f.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/2laS4xqkwA8TsQcvPAEAubbDJXp.jpg">http://image.tmdb.org/t/p/original/2laS4xqkwA8TsQcvPAEAubbDJXp.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/cc1d0A94BhBWC0GDWqETBs8xaxB.jpg">http://image.tmdb.org/t/p/original/cc1d0A94BhBWC0GDWqETBs8xaxB.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/igLAV1EWlPTiuRLsyYbpXkaDhQD.jpg">http://image.tmdb.org/t/p/original/igLAV1EWlPTiuRLsyYbpXkaDhQD.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/1iZXoQX8sDTSnRU8QxoqdaRMbi2.jpg">http://image.tmdb.org/t/p/original/1iZXoQX8sDTSnRU8QxoqdaRMbi2.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/vcRejvkzTL0pkfboJlV1t2a76P1.jpg">http://image.tmdb.org/t/p/original/vcRejvkzTL0pkfboJlV1t2a76P1.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/geuj8N3d5yPex9ZRLz9LADNh0jp.jpg">http://image.tmdb.org/t/p/original/geuj8N3d5yPex9ZRLz9LADNh0jp.jpg</thumb><thumb aspect="poster" preview="http://image.tmdb.org/t/p/w500/mAb0rdCFfBZ14JPVhYY5ArdlZ22.jpg">http://image.tmdb.org/t/p/original/mAb0rdCFfBZ14JPVhYY5ArdlZ22.jpg</thumb>

  //download cover from web if exist in kodi db
  coverpointer=strstr(moviescoverfile,"preview=");   // find startpointer

  if (coverpointer) {
    char *sted=strchr(coverpointer,'>');
    if (sted) {
      coverpointer[(sted-coverpointer)-1]=0;
      strcpy(moviecover,coverpointer+8+1);
      // add path to download link to cover file
      strcpy(filetodownload,moviepath);
      strcat(filetodownload,movietitle);
      // get last name of file (file type)
      ext=strrchr(moviecover,'.');
      if (ext) {
        if (strcmp(ext,".jpg")==0) strcat(filetodownload,".jpg"); else
          if (strcmp(ext,".png")==0) strcat(filetodownload,".png"); else
            strcat(filetodownload,ext);
      }
      // download file and save it same path as movile file
      if (!(file_exists(filetodownload))) {
        if (get_webfile(moviecover,filetodownload)) {
          //if (debugmode & 512) printf("Downloading cover %s\n",movietitle);
        } else {
          printf("Error downloading file cover from %s\n",moviecover);
        }
      }
    }
  }
  fundet=false;
  // create record if not exist
  // do check if exist
  sprintf(sqlselect,"select title from videometadata where title like '%s' and filename like '%s' limit 1",movietitle,moviepath1);
  conn1=mysql_init(NULL);
  if (conn1) {
    mysql_real_connect(conn1, configmysqlhost,configmysqluser, configmysqlpass,dbname, 0, NULL, 0);
    mysql_query(conn1,sqlselect);
    res = mysql_store_result(conn1);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) fundet=true;
    }
    mysql_close(conn1);
  }
  // create if not exist
  if (!(fundet)) {
    //if (debugmode & 512) printf("Import kodi title %40s\n",movietitle);
    sprintf(sqlselect,"insert into videometadata(intid , title, subtitle, tagline, director, studio, plot, rating, inetref, collectionref, homepage, year, releasedate, userrating, length, playcount, season, episode,showlevel, filename,hash, coverfile, childid, browse, watched, processed, playcommand, category, trailer, host, screenshot, banner, fanart,insertdate, contenttype) values \
                                              (0,'%s','%s','','director','','%s','','%s',0,'',%d,'2016-12-31',%2.5f,%d,0,0,0,0,'%s','hash','%s',0,0,0,0,'playcommand',0,'','','','','','2016-01-01',0)", \
                                              movietitle,moviesubtitle,movieplot,movieimdb,movieyear,movieuserrating,movielength ,moviepath1,filetodownload);
    conn=mysql_init(NULL);
    if (conn) {
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass,dbname, 0, NULL, 0);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (mysql_error(conn)) fprintf(stderr,"%s\n",mysql_error(conn));
      mysql_close(conn);
    }
  }
  return(0);
}


// ****************************************************************************************
//
// callback to fill music db in mythtv-controller
//
// ****************************************************************************************

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
  char temp1[256];
  char husklastdir[256];
  char *temppointer;
  int year;
  int length;
  int albumid=0;
  int artistid=0;
  int track;
  int sub_dirid;
  int directoryid=0;
  int last_directoryid=0;
  int argomgang=0;
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
      // working ver
      strcpy(husklastdir,"");
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
        //if (debugmode & 2) printf("nn=%d artistname=%s \n ",nn,temp);
        strcpy(temp1,configdefaultmusicpath);
        //
        if (strcmp(husklastdir,"")!=0) {
          strcat(temp1,husklastdir);
          strcat(temp1,"/");
        }
        strcat(temp1,temp);
        if (file_exists(temp1)) {
          // husk sidste dirid
          last_directoryid=directoryid;
          // check if exist
          exist=false;
          conn2=mysql_init(NULL);
          if (conn2) {
            mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
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
            mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
            sprintf(sqlselect2,"insert into music_directories values (%d,'%s',%d)",0,temp,last_directoryid);
            mysql_query(conn,sqlselect2);
            res = mysql_store_result(conn);
            // hent dir id
            sprintf(sqlselect2,"select directory_id from music_directories where path like '%s'",temp);
            mysql_query(conn,sqlselect2);
            res2 = mysql_store_result(conn);
            if (res2) {
              while ((row2 = mysql_fetch_row(res2)) != NULL) {
                directoryid=atol(row2[0]);
              }
            }
            strcpy(albumname,temp);
            mysql_close(conn);
          }
        }
        strcpy(husklastdir,temp);
        // get rest of path
        nn=strlen(temp)+2;
        if (nn>=0) strcpy(temp,artistname+nn-1);               // remove the last done
        strcpy(artistname,temp);
        argomgang++;                                          // next round
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
      conn2=mysql_init(NULL);
      if (conn2) {
        mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
        sprintf(sqlselect1,"select artist_id from music_artists where artist_name like '%s'",artistname);
        mysql_query(conn2,sqlselect1);
        res2 = mysql_store_result(conn2);
        if (res2) {
          while ((row2 = mysql_fetch_row(res2)) != NULL) artistid=atol(row2[0]);
        }
        mysql_close(conn2);
        if (artistid==0) {
          cr=true;
          conn=mysql_init(NULL);
          mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
          sprintf(sqlselect2,"insert into music_artists values (%d,'%s')",0,artistname);
          mysql_query(conn,sqlselect2);
          res = mysql_store_result(conn);
          mysql_close(conn);
          conn2=mysql_init(NULL);
          mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
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
      conn2=mysql_init(NULL);
      if (conn2) {
        mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
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
        sprintf(sqlselect1,"insert into music_albums values(%d,%d,'%s',%d,%d)",0,artistid,albumname,0,0);
        conn=mysql_init(NULL);
        if (conn) {
          mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
          mysql_query(conn,sqlselect1);
          res = mysql_store_result(conn);
          mysql_close(conn);
        }
        // get albumid
        conn2=mysql_init(NULL);
        if (conn2) {
          mysql_real_connect(conn2, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
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
  strcat(temp,filename1);
  //if (debugmode & 512) printf("Import kodi song %40s \n",songname);
  sprintf(sqlselect,"insert into music_songs(song_id,filename,  name,    track, artist_id, album_id, genre_id, year, length, numplays, rating, lastplay,             date_entered,           date_modified,          format , mythdigest, size , description, comment, disc_count, disc_number, track_count, start_time, stop_time, eq_preset, relative_volume, sample_rate, bitrate, bpm, directory_id) values \
                (%d,    '%s',      '%s',    %d,    %d,        %d,       %d,       %d,    %d,     %d,      %d,     '%s',                 '%s',                   '%s',                   '%s',    '%s',        %d,   '%s',        '%s',    %d,         %d,          %d,          %d,          %d,        '%s',       %d,             %d,          %d,      %d,     %d)", \
                0,      temp,songname,0,    artistid,  albumid,   0,        0,     0,      0,       0,     "2012-01-01 00:00:00",   "2012-01-01 00:00:00","2012-01-01 00:00:00",  "",      "",          0,    "",          "",      0,          0,           0,           0,           0,         "",         0,              0,           0,       0,directoryid);
  conn=mysql_init(NULL);
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass,dbname, 0, NULL, 0);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    mysql_close(conn);
  }
  return(0);
}
