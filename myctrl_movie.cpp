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

#include "utility.h"
#include "myctrl_movie.h"
#include "readjpg.h"
#include "myth_vlcplayer.h"
#include "myctrl_music.h"
extern float configdefaultmoviefontsize;                                      // font size
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
extern GLuint _dvdcovermask;
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

extern int debugmode;

// constructor

film_oversigt_type::film_oversigt_type() {
    film_id=0;				// filmid i mysql
    film_title=new char[128];
    film_subtitle=new char[1024];	      // film info (sub title)
    film_filename=new char[256];	      // path to file to play
    film_coverfile=new char[256];	      // path to created (by convert) 3d gfx file
    film_frontcoverfile=new char[256];	// path to created (by convert) 3d
    film_backcoverfile=new char[256];	  // path to back cover file hentet fra 3d
    film_sidecoverfile=new char[256];	  // path to side cover file hentet fra 3d
    film_adddate=new char[25];		      // format (åååå-mm-dd hh:mm:ss)
    rating=new char[100];		            // imdb rating
    textureId=0;		                  	// texture id for 3D cover hvis der findes en cover til filmen
    frontcover=0;			                  // normal cover
    sidecover=0;
    backcover=0;
    length=0;	                    			// film length
    year=0;				                      // aar som den udkom
    userrating=0;			                  // bruger rating
    film_imdbnr=new char[20];           //
    category_name=new char[128];	      // from mythtv film type (tal = database)
    genre=new char[200];		            //
    for(int n=0;n<20;n++) strcpy(cast[n],"");
    cover3d=false;
}


// destructor

film_oversigt_type::~film_oversigt_type() {
    delete [] film_title;
    delete [] film_subtitle;
    delete [] film_filename;
    delete [] film_coverfile;
    delete [] film_frontcoverfile;
    delete [] film_backcoverfile;
    delete [] film_sidecoverfile;
    delete [] film_adddate;
    delete [] rating;
    delete [] film_imdbnr;
    delete [] category_name;
    delete [] genre;
}

// gem texture filname

void film_oversigt_type::settextureidfile(char *filename) {
    if (textureId==0) textureId=loadTexture ((char *) filename);
}

// load texture from file

void film_oversigt_type::loadfronttextureidfile() {
    if (frontcover==0) frontcover=loadTexture ((char *) film_frontcoverfile);
}

// load texture from file

void film_oversigt_type::loadbacktextureidfile() {
    if (backcover==0) backcover=loadTexture ((char *) film_backcoverfile);
}

// load texture from file

void film_oversigt_type::loadsidetextureidfile() {
    if (sidecover==0) sidecover=loadTexture ((char *) film_sidecoverfile);
}



void film_oversigt_type::swap_film(film_oversigt_type *film1,film_oversigt_type *film2) {
    film_oversigt_type tempfilm;
    int i;
    if ((film1) && (film2)) {
        // gem org
        tempfilm.textureId = film1->textureId;
        tempfilm.frontcover= film1->frontcover;
        tempfilm.backcover = film1->backcover;
        tempfilm.sidecover = film1->sidecover;
        tempfilm.film_id   = film1->film_id;
        tempfilm.length    = film1->length;
        tempfilm.year      = film1->year;
        tempfilm.userrating= film1->userrating;
        strcpy(tempfilm.rating , film1->rating);
        strcpy(tempfilm.film_imdbnr , film1->film_imdbnr);
        strcpy(tempfilm.film_title , film1->film_title);
        strcpy(tempfilm.film_coverfile , film1->film_coverfile);
        strcpy(tempfilm.film_frontcoverfile , film1->film_frontcoverfile);
        strcpy(tempfilm.film_backcoverfile , film1->film_backcoverfile);
        strcpy(tempfilm.film_sidecoverfile , film1->film_sidecoverfile);
        strcpy(tempfilm.film_filename , film1->film_filename);
        strcpy(tempfilm.film_adddate , film1->film_adddate);
        tempfilm.cover3d   =film1->cover3d;
        strcpy(tempfilm.film_subtitle , film1->film_subtitle);
        strcpy(tempfilm.category_name, film1->category_name);
        strcpy(tempfilm.genre , film1->genre);
        for(i=0;i<castlinieantal;i++) {
          strcpy(tempfilm.cast[i],film1->cast[i]);
        }
        // copy 2 to 1
        film1->textureId=film2->textureId;
        film1->frontcover=film2->frontcover;
        film1->backcover=film2->backcover;
        film1->sidecover=film2->sidecover;
        film1->film_id=film2->film_id;
        film1->length=film2->length;
        film1->year=film2->year;
        film1->userrating=film2->userrating;
        strcpy(film1->rating,film2->rating);
        strcpy(film1->film_imdbnr,film2->film_imdbnr);
        strcpy(film1->film_title,film2->film_title);
        strcpy(film1->film_coverfile,film2->film_coverfile);
        strcpy(film1->film_frontcoverfile,film2->film_frontcoverfile);
        strcpy(film1->film_backcoverfile,film2->film_backcoverfile);
        strcpy(film1->film_sidecoverfile,film2->film_sidecoverfile);
        strcpy(film1->film_filename,film2->film_filename);
        strcpy(film1->film_adddate,film2->film_adddate);
        film1->cover3d=film2->cover3d;
        strcpy(film1->film_subtitle,film2->film_subtitle);
        strcpy(film1->category_name,film2->category_name);
        strcpy(film1->genre,film2->genre);
        for(i=0;i<castlinieantal;i++) {
          strcpy(film1->cast[i],film2->cast[i]);
        }
        // copy 1(gemt) til 2
        film2->textureId=tempfilm.textureId;
        film2->frontcover=tempfilm.frontcover;
        film2->backcover=tempfilm.backcover;
        film2->sidecover=tempfilm.sidecover;
        film2->film_id=tempfilm.film_id;
        film2->length=tempfilm.length;
        film2->year=tempfilm.year;
        film2->userrating=tempfilm.userrating;
        strcpy(film2->rating,tempfilm.rating);
        strcpy(film2->film_imdbnr,tempfilm.film_imdbnr);
        strcpy(film2->film_title,tempfilm.film_title);
        strcpy(film2->film_coverfile,tempfilm.film_coverfile);
        strcpy(film2->film_frontcoverfile,tempfilm.film_frontcoverfile);
        strcpy(film2->film_backcoverfile,tempfilm.film_backcoverfile);
        strcpy(film2->film_sidecoverfile,tempfilm.film_sidecoverfile);
        strcpy(film2->film_filename,tempfilm.film_filename);
        strcpy(film2->film_adddate,tempfilm.film_adddate);
        film2->cover3d=tempfilm.cover3d;
        strcpy(film2->film_subtitle,tempfilm.film_subtitle);
        strcpy(film2->category_name,tempfilm.category_name);
        strcpy(film2->genre,tempfilm.genre);
        for(i=0;i<castlinieantal;i++) {
          strcpy(film2->cast[i],tempfilm.cast[i]);
        }
    }
}



// reset film info

void film_oversigt_type::resetfilm() {
     // reset film oversigt
     strcpy(film_coverfile,"");
     strcpy(category_name,"");
     textureId=0;                    // texture id for 3D cover hvis der findes en cover til filmen
     frontcover=0;                   // normal cover
     sidecover=0;		                 // side cover
     backcover=0;             			 // back cover
}


// constructor

film_oversigt_typem::film_oversigt_typem(unsigned int antal) {
    filmoversigt=new film_oversigt_type[antal];
    filmoversigtsize=antal;
    film_oversigt_loaded=false;
    film_is_playing=false;
    film_is_pause=false;
    volume=100;
}

// destructor

film_oversigt_typem::~film_oversigt_typem() {
    if (filmoversigt) delete [] filmoversigt;
    filmoversigtsize=0;
}

// reset all movies in array

void film_oversigt_typem::resetallefilm() {
     for(unsigned int i=0;i<filmoversigtsize-1;i++) {
       filmoversigt[i].resetfilm();			// back cover
    }

}


// default player
// stop playing movie

void film_oversigt_typem::stopmovie() {
  if ((vlc_in_playing()) && (film_is_playing)) vlc_controller::stopmedia();
  film_is_playing=false;
}

// vlc stop player

void film_oversigt_typem::softstopmovie() {
  if ((vlc_in_playing()) && (film_is_playing)) vlc_controller::stopmedia();
  film_is_playing=false;
}


// to play streams from web
//vlc_m = libvlc_media_new_location(vlc_inst, "http://www.ukaff.ac.uk/movies/cluster.avi");

// start playing movie by vlclib

int film_oversigt_typem::playmovie(int nr) {
    char path[PATH_MAX];                                  // max path length from os
    strcpy(path,"");
    strcat(path,configmoviepath);                         // get movie path
    strcat(path,"/");
    film_is_playing=true;
    strcat(path,this->filmoversigt[nr].getfilmfilename());
    vlc_controller::playmedia(path);
}

// pause movie

void film_oversigt_typem::pausemovie() {
  vlc_controller::pause(1);
}

// get position

float film_oversigt_typem::getmovieposition() {
  vlc_controller::get_position();
}

void film_oversigt_typem::next_movie_chapther() {
  vlc_controller::pnext_chapter();
}

void film_oversigt_typem::prevous_movie_chapther() {
  vlc_controller::plast_chapter();
}

void film_oversigt_typem::volumeup() {
  if (volume<100) volume+=10;
  vlc_controller::volume_up(volume);
}

void film_oversigt_typem::volumedown() {
  if (volume>0) volume-=10;
  vlc_controller::volume_down(volume);
}

void film_oversigt_typem::setcolume(int volume) {
  //if ((volume>=0) && (volume<=100)) this->volume=volume;
  vlc_controller::setvolume(volume);
}


// sort movies after type

//type=0 by movie name
//type=1 by id
//type=2 by add date


void film_oversigt_typem::sortfilm(int type) {
  bool swap=false;
  unsigned int i;
  unsigned int antal=get_film_antal();
  if (type==0) {
    do {
      swap=false;
      for(i=0;i<antal-1;i++) {
        if (strcmp(filmoversigt[i].getfilmtitle(),filmoversigt[i+1].getfilmtitle())>0) {
          filmoversigt->swap_film(&filmoversigt[i],&filmoversigt[i+1]);
          swap=true;
        }
      }
    } while(swap);
  } else if (type==1) {
    do {
      swap=false;
      for(i=0;i<antal-1;i++) {
        if (filmoversigt[i].getfilmnr()>filmoversigt[i+1].getfilmnr()) {
          filmoversigt->swap_film(&filmoversigt[i],&filmoversigt[i+1]);
          swap=true;
        }
      }
    } while(swap);
  } else if (type==2) {
    do {
        swap=false;
        for(i=0;i<antal-1;i++) {
          if (strcmp(filmoversigt[i].getfilm_adddate(),filmoversigt[i+1].getfilm_adddate())>0) {
            filmoversigt->swap_film(&filmoversigt[i],&filmoversigt[i+1]);
            swap=true;
          }
        }
    } while(swap);
  }
}



// load dvd gfx files covers in

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



void hentgenre(struct film_oversigt_type *film,unsigned int refnr) {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char sqlselect[1000];
    unsigned int i;
    // mysql stuf
    sprintf(sqlselect,"select idvideo,videogenre.genre from videometadatagenre left join videogenre on videometadatagenre.idgenre=videogenre.intid where idvideo=%d",refnr);
    char *database = (char *) "mythconverg";
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
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


void hentcast(struct film_oversigt_type *film, unsigned int refnr) {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char sqlselect[1000];
    int i=0;
    // mysql stuf
    sprintf(sqlselect,"select videocast.cast from videometadatacast left join videocast on videometadatacast.idcast=videocast.intid where idvideo=%d",refnr);
    // mysql stuf
    char *database = (char *) "mythconverg";
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while ((res) && (((row = mysql_fetch_row(res)) != NULL) && (i<19))) {
        strcpy(film->cast[i],row[0]);		// hent crew
        i++;
      }
      mysql_close(conn);
    }
}



// hent antal elementer i directory dirname

int countEntriesInDir(const char* dirname) {
    int n=0;
    dirent* d;
    DIR* dir = opendir(dirname);
    if (dir == NULL) return 0;
    while((d = readdir(dir))!=NULL) n++;
    closedir(dir);
    return n;
}


// overloaded function in .h file
// hent film oversigt
// create if not exist (mythtv/internal)

int film_oversigt_typem::opdatere_film_oversigt(void) {
    char convert_command[2000];
    char convert_command1[2000];
    char sqlselect[4000];
    char mainsqlselect[2000];
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
    char moviepath1[2000];
    char moviepathcheck[2000];
    char statfilename[2000];
    bool fundet;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    struct stat statbuffer;
    bool film_ok=false;
    int movieyear=2000;
    float movieuserrating=1.0f;
    int movielength=120;
    unsigned int del_rec_nr;
    bool is_db_updated_then_do_clean_up=false;
    long delrecid;
    unsigned int filepathsize;
    char *file_to_check_path;
    // mysql stuf
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    strcpy(database,dbname);
    int checkdirexist=0;
//    gotoxy(10,16);
    if (debugmode & 16) printf("Opdatere Film oversigt fra db :");
    sprintf(mainsqlselect,"SELECT videometadata.intid,title,filename,coverfile,length,year,rating,userrating,plot,inetref,videocategory.category from videometadata left join videocategory on videometadata.category=videocategory.intid and browse=1 order by category,title limit %d",FILM_OVERSIGT_TYPE_SIZE-1);
//    sprintf(sqlselect,"SELECT videometadata.intid,title,filename,coverfile,length,year,rating,userrating,plot,inetref,videocategory.category,videogenre.genre from videogenre,videometadatagenre,videometadata left join videocategory on videometadata.category=videocategory.intid where videometadatagenre.idvideo=videometadata.intid and browse=1 group by idvideo order by category,title limit %d",FILM_OVERSIGT_TYPE_SIZE-1);
    conn=mysql_init(NULL);
    if (conn) {
      allokay=true;
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      // test fpom musik table exist
      sprintf(sqlselect,"SHOW TABLES LIKE '%s.Videometadata'",database);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          dbexist=true;
        }
      } else dbexist=false;
      // create databases/tables if not exist
      // needed by movie loader
      if (!(dbexist)) {
        strcpy(sqlselect,"create table IF NOT EXISTS videometadata(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, title varchar(120), subtitle text, tagline varchar(255), director varchar(128), studio varchar(128), plot text, rating varchar(128), inetref  varchar(255), collectionref int, homepage text,year int, releasedate date, userrating float, length int, playcount int, season int, episode int,showlevel int, filename text,hash varchar(128), coverfile text, childid int, browse int, watched int, processed int, playcommand varchar(255), category int, trailer text,host text, screenshot text, banner text, fanart text,insertdate timestamp, contenttype int)");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"create table IF NOT EXISTS videocategory(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, category varchar(128))");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"create table IF NOT EXISTS videogenre(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, genre varchar(128))");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"create table IF NOT EXISTS videocountry(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, country varchar(128))");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"create table IF NOT EXISTS videocollection(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, title varchar(256), contenttype int, plot text,network varchar(128), collectionref varchar(128), certification varchar(128), genre varchar(128),releasedate date, language varchar(10),status varchar(64), rating float, ratingcount int, runtime int, banner text,fanart text,coverart text)");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"create table IF NOT EXISTS videopathinfo(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, path text, contenttype int, collectionref int,recurse  int)");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"create table IF NOT EXISTS videotypes(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, extension varchar(128),playcommand varchar(255), f_ignore int,  use_default int)");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"insert into videotypes values (0,'txt','',1,0)");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"insert into videotypes values (0,'log','',1,0)");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"insert into videotypes values (0,'mpg','',0,0)");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"insert into videotypes values (0,'avi','',0,1)");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"insert into videotypes values (0,'vob','',0,0)");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"insert into videotypes values (0,'mpeg','',0,0)");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"insert into videotypes values (0,'VIDEO_TS','',0,0)");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"insert into videotypes values (0,'iso','',0,0)");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        strcpy(sqlselect,"insert into videotypes values (0,'img','',0,0)");
        mysql_query(conn,sqlselect);
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
      conn=mysql_init(NULL);
      if ((conn) && (dirp)) {
        mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass,database, 0, NULL, 0);
        // loop files in dirp
        while (moviefil = readdir(dirp)) {
          if ((strcmp(moviefil->d_name,".")!=0) && (strcmp(moviefil->d_name,"..")!=0)) {
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
                  if (strcmp(ext,".iso")==0) film_ok=true;
                  if (strcmp(ext,".ISO")==0) film_ok=true;
                  if (film_ok) {
                    strcpy(movietitle,submoviefil->d_name);
                    // name title
                    ext = strrchr(movietitle, '.');
                    if (ext) {
                      *ext='\0';
                    }
                    strcpy(moviepath1,moviefil->d_name);
                    strcat(moviepath1,"/");
                    strcat(moviepath1,submoviefil->d_name);
                    strcpy(moviepathcheck,configmoviepath);
                    strcat(moviepathcheck,"/");
                    strcat(moviepathcheck,moviefil->d_name);                        // path
                    strcat(moviepathcheck,"/");
                    strcat(moviepathcheck,submoviefil->d_name);                     // get full filename
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
                    if (debugmode & 16) {
                      if (fundet) printf("checking/replace movietitle %s \n",movietitle);
                      else printf("checking..insert movietitle %s \n",movietitle);
                    }
                    // check if record exist (video file exist)
                    if ((fundet) && (del_rec_nr)) {
                      if (!(file_exists(moviepathcheck))) {
                        sprintf(sqlselect,"delete from videometadata where intid=%d limit 1",del_rec_nr);
                        mysql_query(conn,sqlselect);
                        res = mysql_store_result(conn);
                      }
                    }
                    if (!(fundet)) {
                      sprintf(sqlselect,"insert into videometadata(intid , title, subtitle, tagline, director, studio, plot, rating, inetref, collectionref, homepage, year, releasedate, userrating, length, playcount, season, episode,showlevel, filename,hash, coverfile, childid, browse, watched, processed, playcommand, category, trailer, host, screenshot, banner, fanart,insertdate, contenttype) values \
                                                                (0,'%s','%s','','director','','%s','','%s',0,'',%d,'2016-12-31',%2.5f,%d,0,0,0,0,'%s','hash','%s',0,0,0,0,'playcommand',0,'','','','','','2016-01-01',0)", \
                                                                movietitle,"moviesubtitle","movieplot","movieimdb",movieyear,movieuserrating,movielength ,moviepath1,"filetodownload");
                      recnr++;
                      mysql_query(conn,"set NAMES 'utf8'");
                      res = mysql_store_result(conn);
                      mysql_query(conn,sqlselect);
                      res = mysql_store_result(conn);
                      if ((mysql_error(conn)) && (debugmode & 512)) {
                        printf("%s\n",mysql_error(conn));
                        exit(0);
                      }
                    }
                  }
                }
              }
            } else if ((statbuffer.st_mode & S_IFMT)==S_IFREG) {
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
                  strcpy(moviepath1,moviefil->d_name);                         // get full filename
                  fundet=false;
                  del_rec_nr=0;
                  sprintf(sqlselect,"select intid from videometadata where title like '%%");
                  sprintf(temptxt,"%s",movietitle);
                  strcat(sqlselect,temptxt);
                  sprintf(temptxt,"%%' and filename like '%%");
                  strcat(sqlselect,temptxt);
                  sprintf(temptxt,"%s",moviepath1);
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
                  if (debugmode & 16) {
                    if (fundet) printf("checking/replace movietitle %s \n",movietitle);
                    else printf("checking ..found movietitle %s \n",movietitle);
                  }
                  // findes filmen i db i forvejen så slet den og opret den igen
                  // ellers bare opret den
                  // dette skal gøres hvis dir eller fil navn ændre sig
                  if ((fundet) && (del_rec_nr)) {
                    if (!(file_exists(moviepathcheck))) {
                      sprintf(sqlselect,"delete from videometadata where intid=%d limit 1",del_rec_nr);
                      mysql_query(conn,sqlselect);
                      res = mysql_store_result(conn);
                    }
                  }
                  if (!(fundet)) {
                    sprintf(sqlselect,"insert into videometadata(intid , title, subtitle, tagline, director, studio, plot, rating, inetref, collectionref, homepage, year, releasedate, userrating, length, playcount, season, episode,showlevel, filename,hash, coverfile, childid, browse, watched, processed, playcommand, category, trailer, host, screenshot, banner, fanart,insertdate, contenttype) values \
                                                              (0,'%s','%s','','director','','%s','','%s',0,'',%d,'2016-12-31',%2.5f,%d,0,0,0,0,'%s','hash','%s',0,0,0,0,'playcommand',0,'','','','','','2016-01-01',0)", \
                                                              movietitle,"moviesubtitle","movieplot","movieimdb",movieyear,movieuserrating,movielength ,moviepath1,"filetodownload");
                    recnr++;
                    fprintf(stderr, "Movie db update %2d title %s \n",recnr,movietitle);
                    mysql_query(conn,"set NAMES 'utf8'");
                    res = mysql_store_result(conn);
                    mysql_query(conn,sqlselect);
                    res = mysql_store_result(conn);
                    if ((mysql_error(conn)) && (debugmode & 512)) {
                      printf("%s\n",mysql_error(conn));
                      exit(0);
                    }
                  }
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
      mysql_query(conn,mainsqlselect);
      res = mysql_store_result(conn);
      i=0;
      filhandle=fopen("filmcover_gfx.log","w");
      if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (i<FILM_OVERSIGT_TYPE_SIZE)) {
          is_db_updated_then_do_clean_up=true;
          filmantal++;
          filmoversigt[i].setfilmid(atoi(row[0]));
          filmoversigt[i].setfilmtitle(row[1]);
          hentcast(&filmoversigt[i],filmoversigt[i].getfilmid());
          hentgenre(&filmoversigt[i],filmoversigt[i].getfilmid());
          if (row[8]) {							// hent film beskrivelse
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
          if (strcmp((char *) filmoversigt[i].getfilmcoverfile(),"No Cover")==0) filmoversigt[i].setfilmcoverfile((char *)"");
          // hvis cover ikke findes som jpg fil lav dem med convert kommandoen.
          strcpy(convert_command,"convert/");
          strcat(convert_command,filmoversigt[i].getfilmtitle());
          strcat(convert_command,"_3d.jpg");
          // temp test om file exist
          strcpy(convert_command1,"convert/");
          strcat(convert_command1,filmoversigt[i].getfilmtitle());
          strcat(convert_command1,".jpg");
          // set der findes 3dcover flag i struct
          if (file_exists(convert_command)) filmoversigt[i].setcover3d(true);
           // henter antal af filer i dir
          checkdirexist=countEntriesInDir("/usr/share/mythtv-controller/convert/hires/");
          if (checkdirexist==2) checkdirexist=0;
          if ((checkdirexist>0) && (!(file_exists(convert_command))) && (!(file_exists(convert_command1)))) {
            strcpy(convert_command,"convert/hires/");
            strcat(convert_command,filmoversigt[i].getfilmtitle());
            strcat(convert_command,".jpg");
            if (file_exists(convert_command)) {									// findes der et hires billede fra cc covers
                                                            // yes convert to mythtv use
              strcpy(convert_command,"/usr/bin/convert -quiet -resize 1024x687! \"convert/hires/");		// first convert and resize dvd cover
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,".jpg\" \"convert/");
              strcat(convert_command,row[1]);
              strcat(convert_command,".jpg\" > /dev/null 2>&1");
              system(convert_command);
              // cut front
              strcpy(convert_command,"/usr/bin/convert -quiet ");
              strcat(convert_command,"\"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,".jpg\"");
              strcat(convert_command," -gravity West -chop 542x0 \"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_front.jpg\" > /dev/null 2>&1");
              system(convert_command);
              // cut ryg
              strcpy(convert_command,"/usr/bin/convert -quiet ");
              strcat(convert_command,"\"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,".jpg\"");
              strcat(convert_command," -gravity West -chop 486x0 -gravity East -chop 486x0 \"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_ryg.jpg\" > /dev/null 2>&1");
              system(convert_command);
              // cut back
              strcpy(convert_command,"/usr/bin/convert -quiet ");
              strcat(convert_command,"\"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,".jpg\"");
              strcat(convert_command," -gravity East -chop 542x0 \"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_back.jpg\" > /dev/null 2>&1");
              system(convert_command);
              // create 3d dvd cover
              strcpy(convert_command,"/usr/bin/convert -quiet -virtual-pixel transparent");
              strcat(convert_command," \"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_ryg.jpg\" -matte +distort Perspective '0,0 -45,20 0,687 -45,667  52,687 0,687  52,0 0,0'"); //  '0,0 -30,20  0,200 -30,179  19.5,200 0,200  19.5,0 0.5,0'
              strcat(convert_command," \"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_front.jpg\" -matte +distort Perspective '0,0 0,5   0,687 0,907   558,687 551,859  558,0 551,47'"); // '0.5,0 0.5,0  0.5,200  0.5,200  150,200 100,156  150,0 100,30'
              strcat(convert_command," -background black -layers merge  +repage -bordercolor black  \"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_3d_big.jpg\" > /dev/null 2>&1");
              system(convert_command);
              // scale and convert cover to BMP
              strcpy(convert_command,"/usr/bin/convert -quiet -scale 512 ");
              strcat(convert_command,"\"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_3d_big.jpg\"");
              strcat(convert_command," \"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_3d.jpg\" > /dev/null 2>&1");
              system(convert_command);
              // scale and convert front cover to BMP
              strcpy(convert_command,"/usr/bin/convert -quiet -scale 512 ");
              strcat(convert_command,"\"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_front.jpg\"");
              strcat(convert_command," \"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_front.jpg\" > /dev/null 2>&1");
              system(convert_command);
              // scale and convert back cover to BMP
              strcpy(convert_command,"/usr/bin/convert -quiet -scale 512 ");
              strcat(convert_command,"\"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_back.jpg\"");
              strcat(convert_command," \"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_back.jpg\" > /dev/null 2>&1");
              system(convert_command);
              // scale and convert ryg cover to BMP
              strcpy(convert_command,"/usr/bin/convert -quiet -scale 512 ");
              strcat(convert_command,"\"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_ryg.jpg\"");
              strcat(convert_command," \"convert/");
              strcat(convert_command,filmoversigt[i].getfilmtitle());
              strcat(convert_command,"_ryg.jpg\" > /dev/null 2>&1");
              system(convert_command);
            } else {  // else create cover from mythtv database
              if (strcmp(filmoversigt[i].getfilmcoverfile(),"")!=0) {			// der findes et cover
                strcpy(convert_command,"convert  -quiet -resize 128x200! '");		// first convert and resize dvd cover
                strcat(convert_command,row[3]);
                strcat(convert_command,"' \"convert/");
                strcat(convert_command,row[1]);
                strcat(convert_command,".jpg\" > /dev/null 2>&1");
                system(convert_command);
              } else {								// lav front cover selv
                strcpy(convert_command,"convert -quiet -size 128x200 xc:gray  -fill white -pointsize 12 -gravity north -annotate +0+15 \"");
                strcat(convert_command,filmoversigt[i].getfilmtitle());
                strcat(convert_command,"\"  -stroke white -strokewidth 1 -draw 'line 0,0 0,200' ");
                strcat(convert_command," -fill black -pointsize 15 -gravity south -annotate +0+5 '");
                strcat(convert_command,"DVD Film");
                strcat(convert_command,"' -stroke blue -strokewidth 2 -draw 'line 0,169 150,169' \"convert/");
                strcat(convert_command,filmoversigt[i].getfilmtitle());
                strcat(convert_command,".jpg\" > /dev/null 2>&1");
                system(convert_command);
              }
              strcpy(convert_command,filmoversigt[i].getfilmtitle());
              if (file_exists(convert_command)) {
                // create dvd cover ryg
                strcpy(convert_command,"convert -quiet -size 200x20 xc:gray -fill white -pointsize 12 -gravity north -annotate +0+5 \"");
                strcat(convert_command,filmoversigt[i].getfilmtitle());
                strcat(convert_command,"\" -stroke blue -strokewidth 2 -draw 'line 30,0 30,40' -rotate -90 \"convert/");
                strcat(convert_command,filmoversigt[i].getfilmtitle());
                strcat(convert_command,"_ryg.jpg\" > /dev/null 2>&1");
                system(convert_command);
                // create 3d dvd cover
                strcpy(convert_command,"convert -quiet -virtual-pixel transparent");
                strcat(convert_command," \"convert/");
                strcat(convert_command,filmoversigt[i].getfilmtitle());
                strcat(convert_command,"_ryg.jpg\" -matte  +distort Perspective '0,0 -10,5  0,200 -10,195  19.5,200 0,200  19.5,0 0.5,0'"); //  '0,0 -30,20  0,200 -30,179  19.5,200 0,200  19.5,0 0.5,0'
                strcat(convert_command," \"convert/");
                strcat(convert_command,filmoversigt[i].getfilmtitle());
                strcat(convert_command,".jpg\" -matte  +distort Perspective '0.5,0 0.5,0  0.5,200  0.5,200  100,200 100,192  100,0 100,5'"); // '0.5,0 0.5,0  0.5,200  0.5,200  150,200 100,156  150,0 100,30'
                strcat(convert_command," -background black -layers merge  +repage -bordercolor black  \"convert/");
                strcat(convert_command,filmoversigt[i].getfilmtitle());
                strcat(convert_command,"_3d.jpg\" > /dev/null 2>&1");
                system(convert_command);
                // scale
                strcpy(convert_command,"/usr/bin/convert -quiet -scale 128 ");
                strcat(convert_command,"\"convert/");
                strcat(convert_command,filmoversigt[i].getfilmtitle());
                strcat(convert_command,"_3d.jpg\"");
                strcat(convert_command," \"convert/");
                strcat(convert_command,filmoversigt[i].getfilmtitle());
                strcat(convert_command,"_3d.jpg\" > /dev/null 2>&1");
                system(convert_command);
                // scale
                strcpy(convert_command,"/usr/bin/convert -quiet -scale 128 ");
                strcat(convert_command,"\"convert/");
                strcat(convert_command,filmoversigt[i].getfilmtitle());
                strcat(convert_command,".jpg\"");
                strcat(convert_command," \"convert/");
                strcat(convert_command,filmoversigt[i].getfilmtitle());
                strcat(convert_command,".jpg\" > /dev/null 2>&1");
                system(convert_command);
              }
            }
          } else {
            sprintf(resl,"Movie Cover missing: %s \n",row[1]);
            fputs(resl,filhandle);
          }
          // set 3d cover
          strcpy(temptxt,"convert/");					// gem fil navn til image loader
          strcat(temptxt,filmoversigt[i].getfilmtitle());		// fil navn
          strcat(temptxt,"_3d.jpg");

          if (file_exists(temptxt)) {
            filmoversigt[i].setfilmcoverfile(temptxt);
          }
          // set front dvd cover
          strcpy(temptxt,"convert/");					// gem fil navn til image loader
          strcat(temptxt,filmoversigt[i].getfilmtitle());         	// fil navn
          strcat(temptxt,"_front.jpg");
          if (file_exists(temptxt)) {
            filmoversigt[i].setfilmfcoverfile(temptxt);
          }
          // set back cover
          strcpy(temptxt,"convert/");         		    	// gem fil navn til image loader
          strcat(temptxt,filmoversigt[i].getfilmtitle());          	// fil navn
          strcat(temptxt,"_back.jpg");
          if (file_exists(temptxt)) {
            filmoversigt[i].setfilmbcoverfile(temptxt);
          }
          // set ryg cover
          strcpy(temptxt,"convert/");         		    	// gem fil navn til image loader
          strcat(temptxt,filmoversigt[i].getfilmtitle());          	// fil navn
          strcat(temptxt,"_ryg.jpg");
          if (file_exists(temptxt)) {
            filmoversigt[i].setfilmscoverfile(temptxt);
          }
	        i++;
        }
      }
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
              strcpy(file_to_check_path,configmoviepath);                                        // make path to file
              strcat(file_to_check_path,"/");
              strcat(file_to_check_path,row[1]);
              if (strlen(file_to_check_path)>1) {
                if (!(file_exists(file_to_check_path))) {
                  fputs("Movie deleted filename ",filhandle);
                  fputs(row[1],filhandle);                                        // write to log file
                  fputs("\n",filhandle);
                  delrecid=atol(row[0]);
                  // delete from db
                  sprintf(sqlselect,"delete from videometadata where intid=%d limit 1",delrecid);
                  mysql_query(conn,sqlselect);
                  res = mysql_store_result(conn);
                }
              }
              delete [] file_to_check_path;
            }
          }
        }
        if (filhandle) fclose(filhandle);
      }
    }
    if (filmantal>0) this->filmoversigt_antal=filmantal-1; else this->filmoversigt_antal=0;
    //gotoxy(10,18);
    if (debugmode & 16) printf(" %d dvd covers loaded\n",filmantal);
    mysql_close(conn);
    return(filmantal);
}








// overloaded function in .h file
// hent film oversigt
// create if not exist (mythtv/internal)

int film_oversigt_typem::opdatere_film_oversigt(char *movietitle) {
    char sqlselect[4000];
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
      // test fpom musik table exist
      sprintf(sqlselect,"SHOW TABLES LIKE 'videometadata'",database);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        while ((res) && ((row = mysql_fetch_row(res)) != NULL)) {
          dbexist=true;
        }
      } else dbexist=false;
    }
    conn=mysql_init(NULL);
    // Connect to database
    if (conn) {
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
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
    if (filmantal>0) this->filmoversigt_antal=filmantal; else this->filmoversigt_antal=0;
    //gotoxy(10,18);
    if (debugmode & 16) printf(" %d dvd covers loaded\n",filmantal);
    mysql_close(conn);
    return(filmantal);
}





//
// mini oversigt
// i start menu

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
    strcpy(tmpfilename,this->filmoversigt[movie_oversigt_loaded_nr].getfilmcoverfile());
    if ((file_exists(tmpfilename)) && (this->filmoversigt[movie_oversigt_loaded_nr].gettextureid()==0)) {
      this->filmoversigt[movie_oversigt_loaded_nr].settextureidfile(tmpfilename);
    }
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
        glBindTexture(GL_TEXTURE_2D,_dvdcovermask);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
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
        glLoadName(100+i+sofset);
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
        glBindTexture(GL_TEXTURE_2D,_dvdcovermask);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(xpos,ypos+((orgwinsizey/2)-(800/2))-boffset , 0.0);
        glTexCoord2f(0, 1); glVertex3f(xpos,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset , 0.0);
        glTexCoord2f(1, 1); glVertex3f(xpos+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset , 0.0);
        glTexCoord2f(1, 0); glVertex3f(xpos+winsizx,ypos+((orgwinsizey/2)-(800/2))-boffset , 0.0);
        glEnd();
      }
      strcpy(temptxt,filmoversigt[i+sofset].getfilmtitle());        // album navn
      lastslash=strrchr(temptxt,'/');
      if (lastslash) strcpy(temptxt,lastslash+1);
      glPushMatrix();
      if (strlen(temptxt)<=14) {
        ofs=(strlen(temptxt)/2)*12;
        glTranslatef(xpos+(100-ofs), ypos+120 ,0.0f);
        glRasterPos2f(0.0f, 0.0f);
        glDisable(GL_TEXTURE_2D);
        glScalef(20.0, 20.0, 1.0);
        glcRenderString(temptxt);
      } else {
        glTranslatef(xpos+(60-ofs), ypos+120 ,0.0f);
        glScalef(20.0, 20.0, 1.0);
        glDisable(GL_TEXTURE_2D);
        ofs=(strlen(temptxt)/2)*9;
        // show movie title
        float ytextofset=0.0f;
        int ii,j,k,pos;
        int xof,yof;
        ii=pos=0;
        char word[16000];
        while((1) && (ytextofset<=10.0)) {		// max 2 linier
          j=0;
          while(!isspace(temptxt[ii])) {
            if (temptxt[ii]=='\0') break;
            word[j]=temptxt[ii];
            ii++;
            j++;
          }
          word[j]='\0';	// j = word length
          if (j>13) {		// print char by char
            k=0;
            while(word[k]!='\0') {
              if (pos>=13) {
                if ( k != 0 ) glcRenderChar('-');
                pos=0;
                ytextofset+=15.0f;
                ofs=0;
                glTranslatef(xof-50,  yof-60-20-ytextofset ,xvgaz);
                glRasterPos2f(0.0f, 0.0f);
                glScalef(14.0, 14.0, 1.0);
              }
              glcRenderChar(word[k]);
              pos++;
              k++;
            }
          } else {
            if (pos+j>13) {	// word doesn't fit line
              ytextofset+=15.0f;
              pos=0;
              ofs=(int) (strlen(word)/2)*9;
              glTranslatef(xof-50,  yof-60-20-ytextofset ,xvgaz);
              glRasterPos2f(0.0f, 0.0f);
              glScalef(14.0, 14.0, 1.0);
            }
            glcRenderString(word);
            pos+=j;
          }
          if (pos<12) {
            glcRenderChar(' ');
            pos++;
          }
          if (temptxt[ii]=='\0') break;
          ii++;	// skip space
        }
      }
      glEnable(GL_TEXTURE_2D);
      glPopMatrix();
    }
    xpos+=205;
    i++;
  }
}


//
// normal oversigt
// ny udgave

void film_oversigt_typem::show_film_oversigt(float _mangley,int filmnr) {
  int lfilmoversigt_antal=4*8;
  int film_nr=0;
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
        glBindTexture(GL_TEXTURE_2D,_dvdcovermask);                           //
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+film_nr+sofset);
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
        glLoadName(100+film_nr+sofset);
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
        glBindTexture(GL_TEXTURE_2D,_dvdcovermask);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+film_nr+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(xpos,ypos+((orgwinsizey/2)-(800/2))-boffset , 0.0);
        glTexCoord2f(0, 1); glVertex3f(xpos,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset , 0.0);
        glTexCoord2f(1, 1); glVertex3f(xpos+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy+boffset , 0.0);
        glTexCoord2f(1, 0); glVertex3f(xpos+winsizx,ypos+((orgwinsizey/2)-(800/2))-boffset , 0.0);
        glEnd();
      }
      strcpy(temptxt,filmoversigt[film_nr+sofset].getfilmtitle());        // album navn
      lastslash=strrchr(temptxt,'/');
      if (lastslash) strcpy(temptxt,lastslash+1);
      glPushMatrix();
      if (strlen(temptxt)<=14) {
        ofs=(strlen(temptxt)/2)*12;
        glTranslatef(xpos+(100-ofs), ypos+120 ,0.0f);
        glRasterPos2f(0.0f, 0.0f);
        glDisable(GL_TEXTURE_2D);
        glScalef(configdefaultmoviefontsize, configdefaultmoviefontsize, 1.0);
        glcRenderString(temptxt);
      } else {
        glTranslatef(xpos+(50-ofs), ypos+120 ,0.0f);
        glScalef(configdefaultmoviefontsize, configdefaultmoviefontsize, 1.0);
        glDisable(GL_TEXTURE_2D);
        ofs=(strlen(temptxt)/2)*9;
        float ytextofset=0.0f;
        int ii,j,k,pos;
        int xof,yof;
        ii=pos=0;
        char word[16000];
        while((1) && (ytextofset<=10.0)) {		// max 2 linier
          j=0;
          while(!isspace(temptxt[ii])) {
            if (temptxt[ii]=='\0') break;
            word[j]=temptxt[ii];
            ii++;
            j++;
          }
          word[j]='\0';	// j = word length
          if (j>13) {		// print char by char
            k=0;
            while(word[k]!='\0') {
              if (pos>=13) {
                if ( k != 0 ) glcRenderChar('-');
                pos=0;
                ytextofset+=15.0f;
                ofs=0;
                glTranslatef(xof-50,  yof-60-20-ytextofset ,xvgaz);
                glRasterPos2f(0.0f, 0.0f);
                //glScalef(configdefaultmoviefontsize, configdefaultmoviefontsize, 1.0);
              }
              glcRenderChar(word[k]);
              pos++;
              k++;
            }
          } else {
            if (pos+j>13) {	// word doesn't fit line
              ytextofset+=15.0f;
              pos=0;
              ofs=(int) (strlen(word)/2)*9;
              glTranslatef(xof-50,  yof-60-20-ytextofset ,xvgaz);
              glRasterPos2f(0.0f, 0.0f);
              //glScalef(configdefaultmoviefontsize, configdefaultmoviefontsize, 1.0);
            }
            glcRenderString(word);
            pos+=j;
          }
          if (pos<12) {
            glcRenderChar(' ');
            pos++;
          }
          if (temptxt[ii]=='\0') break;
          ii++;	// skip space
        }
      }
//        glTranslatef(xpos+ofs, ypos+120 ,0.0f);
//        glRasterPos2f(0.0f, 0.0f);
//        glDisable(GL_TEXTURE_2D);
//        glScalef(20.0, 20.0, 1.0);
//        glcRenderString(temptxt);
      glEnable(GL_TEXTURE_2D);
      glPopMatrix();
    }
    xpos+=205;
    film_nr++;
    //glPopMatrix();
  }
  if ((film_nr==0) || (this->filmoversigt_antal==0)) {
    sprintf(temptxt,"No info from %s backend.",configbackend);
    strcat(temptxt,configmysqlhost);
    glPushMatrix();
    xpos=600;
    ypos=400;
    glTranslatef(xpos+10, ypos+40 ,0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glDisable(GL_TEXTURE_2D);
    glScalef(20.0, 20.0, 1.0);
    glcRenderString(temptxt);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
  }
}
