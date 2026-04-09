#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <algorithm>
#include <string.h>
#include <mysql.h>
#include <GL/glc.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <fmt/format.h>
#include <sqlite3.h>                    // sqlite interface to xbmc
#include <curl/curl.h>                  // lib curl

// json parser
#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>
#include "myctrl_readwebfile.h"
#include "myctrl_radio.h"
#include "utility.h"
#include "myth_ttffont.h"
#include "utility.h"
#include "readjpg.h"
#include "loadpng.h"
#include "myctrl_glprint.h"
#include "myth_config.h"

extern config_icons config_menu;
extern GLuint tidal_big_search_bar_artist;

extern FILE *logfile;
extern char debuglogdata[1024];                                  // used by log system
extern Character characters[];
extern int orgwinsizey;                                                         // screen size
extern int orgwinsizex;
extern char keybuffer[];                                    // keyboard buffer
extern float configdefaultradiofontsize;
extern const char *dbname;                                    // db name in mysql
extern char configmysqluser[];                             //
extern char configmysqlpass[];                             //
extern char configmysqlhost[];                             //
extern char configmusicpath[];
extern int configmythtvver;
extern int screen_size;                                       //
extern int screensizey;                                       //
extern int screeny;                                           //
extern unsigned int musicoversigt_antal;                      //
extern int radio_key_selected;                                //
extern int music_select_iconnr;                               //
extern int do_music_icon_anim_icon_ofset;                     //
extern GLuint _textureIdback1;                                //
extern GLuint onlineradio;					                          //
extern GLuint onlineradio_empty;				                      //
extern GLuint onlineradio_selected;				                    //
extern GLuint onlineradio192;					                        //
extern GLuint onlineradio320;					                        //
extern GLuint radiooptions;                                   //
extern int fonttype;
extern fontctrl aktivfont;
extern GLuint _textureIdloading;
extern GLuint gfxlande[45];
extern GLuint gfxlandemask;
extern radiostation_class radiooversigt;
extern GLint cur_avail_mem_kb;
extern bool radio_oversigt_loaded;
extern bool radio_oversigt_loaded_done;
extern bool radio_oversigt_loaded_begin;


extern mFont font12;  // 12px font
extern mFont font18;  // 18px font
extern mFont font24;  // 24px font

extern int radio_oversigt_loaded_nr;
extern int radio_oversigt_antal;
extern bool do_sqlite;

// ****************************************************************************************
//
// constructor
//
// ****************************************************************************************

radiostation_class::radiostation_class() : antal(0) {
  int i;
  for(i=0;i<radiooptionsmax;i++) {
    strcpy(radiosortopt[i].radiosortopt,"");
    radiosortopt[i].antal=0;
  }
  radiooptionsselect=0;							// selected line in radio options
  playing=false;        						// playing radio station  
}


// ****************************************************************************************
//
// destructor
//
// ****************************************************************************************

radiostation_class::~radiostation_class() {
  clean_radio_oversigt();
}


// ****************************************************************************************
//
// clean up number of created
//
// ****************************************************************************************

void radiostation_class::clean_radio_oversigt() {
  startup_loaded=false;				// set radio station loaded in
  for(int i=0;i<antal;i++) {
    if (stack[i].textureId) glDeleteTextures(1, &stack[i].textureId);	// delete radio texture
  }
  stack.clear();
  antal=0;
}



// ****************************************************************************************
//
// write data
//
// ****************************************************************************************

static size_t radio_write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}


// *******************************************************************************************
//
// download image
//
// *******************************************************************************************

int radiostation_class::radio_download_image(char *imgurl,char *filename) {
  FILE *file;
  std::string response_string;
  CURLcode res;
  CURL *curl;
  char *base64_code;
  char errbuf[CURL_ERROR_SIZE];
  if (!(file_exists(filename))) {
    curl = curl_easy_init();
    if (curl) {
      curl_easy_setopt(curl, CURLOPT_URL, imgurl);
      // send data to curl_writeFunction_file
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, radio_write_data);
      // ask libcurl to use TLS version 1.3 or later
      curl_easy_setopt(curl, CURLOPT_SSLVERSION, (long)CURL_SSLVERSION_TLSv1_3);
      curl_easy_setopt(curl, CURLOPT_VERBOSE,0L);
      curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:89.0) Gecko/20100101 Firefox/89.0");
      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); // <-- ssl don't forget this
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0); // <-- ssl and this
      curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 3000L);
      curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 10000L);
      curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
      curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 5L);      
      // folow redirect
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      try {
        file = fopen(filename, "wb");
        if (file) {
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
          // get file
          res = curl_easy_perform(curl);
          fclose(file);
        }
        if(res != CURLE_OK) {
          // fprintf(stderr, "%s\n", curl_easy_strerror(res));
        }
      }
      catch (...) {
        printf("Error write file.\n");
      }
      curl_easy_cleanup(curl);
    }
  }
  return(1);
}


// ******************************************************************************************
//
// load show cover image if not loaded
// 
// ******************************************************************************************

GLuint radiostation_class::get_texture_r(int nr) {
  GLuint textureId=0;
  std::string filename=stack[nr].gfxfilename;
  std::string onlyname;
  size_t pos=(filename.find_last_of('.'));
  onlyname=filename.substr(0, pos);
  if (!(stack[nr].texture_r_loaded)) {
    if (onlyname.length()>0) {
      onlyname = "/opt/mythtv-controller/images/radiostations/" + onlyname;
      onlyname = onlyname + "_r.png";
      textureId = loadTexture((char *) onlyname.c_str());
      stack[nr].textureId_r = textureId;
      stack[nr].texture_r_loaded=true;
      return(textureId);
    }
  }
  return(stack[nr].textureId_r);
}


// ******************************************************************************************
//
// json radio file paser
//
// ******************************************************************************************

int radiostation_class::load_radio_stations_from_json_file() {
  MYSQL *conn1;
  MYSQL_RES *res;
  MYSQL_ROW row;
  const char *database = (char *) "mythtvcontroller";
  int ok=0;
  FILE *f;
  radio_oversigt_type new_radio_record;
  int antal=0;
  int antal_in_db=0;
  int return_code;
  bool fundet=false;
  char downloadfilename[8192];
  std::string downloadfilenamelong;
  std::string downloadfilenamelong_out;
  std::string downloadfilenamelong2;
  std::string do_cmd;
  std::string sql_update;
  Json::Value cfg_root;
  std::string downloadfilenamelong3;
  int art=0;
  conn1=mysql_init(NULL);
  if (mysql_real_connect(conn1, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0)) {    
    std::ifstream cfgfile("stations-big_all.json");
    cfgfile >> cfg_root;
    // Tjek at root er array
    if (!cfg_root.isArray()) {
      std::cout << "Root er ikke et array!\n";
      return 0;
    }
    // loop gemmen array
    int antal_in_json=cfg_root.size();
    std::cout << "Please wait process json file 'stations-big_all.json' Total " << antal_in_json << " Radio stations." << std::endl;
    sql_update  = fmt::format("select count(name) from radio_stations");
    mysql_query(conn1,sql_update.c_str());
    res = mysql_store_result(conn1);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        antal_in_db=atoi(row[0]);
      }
    }

    // std::cout << "antal_in_db = " << antal_in_db << " antal_in_json " << antal_in_json << std::endl;

    if (antal_in_db==0) {
      for (const auto& station : cfg_root) {
        printf(".");
        fflush( stdout );
        std::string name = station.get("name", "").asString();
        std::string url  = station.get("url_resolved", "").asString();
        std::string country = station.get("country", "").asString();
        std::string gfxurl = station.get("favicon", "").asString();
        std::string contrycode = station.get("contrycode", "").asString();
        int bitrate = station.get("bitrate", 0).asInt();   
        std::string desc = station.get("tags", "").asString();
        int clickcount = station.get("clickcount", "").asInt();
        strncpy(new_radio_record.station_name,name.c_str(),10);
        new_radio_record.station_name[11]=0;
        new_radio_record.streamurl=url;
        new_radio_record.art=0;
        new_radio_record.online=true;
        new_radio_record.aktiv=true;
        new_radio_record.kbps=bitrate;
        std::transform(contrycode.begin(), contrycode.end(), contrycode.begin(), ::toupper);
        if (contrycode=="US") new_radio_record.land=7;
        else if (contrycode=="GERMAN") new_radio_record.land=8;
        else if (contrycode=="FR") new_radio_record.land=4;
        else if (contrycode=="CN") new_radio_record.land=63;
        else if (contrycode=="IN") new_radio_record.land=37;
        else if (contrycode=="RU") new_radio_record.land=10;
        else if (contrycode=="DE") new_radio_record.land=8;
        else if (contrycode=="PL") new_radio_record.land=22;
        else if (contrycode=="IT") new_radio_record.land=42;
        else if (contrycode=="CA") new_radio_record.land=27;
        else if (contrycode=="MX") new_radio_record.land=30;
        else if (contrycode=="DK") new_radio_record.land=45;
      
        if (desc.find("news") != std::string::npos) art=1;
        else if (desc.find("rock") != std::string::npos) art=3;
        else if (desc.find("pop") != std::string::npos) art=4;
        else if (desc.find("house") != std::string::npos) art=5;
        else if (desc.find("trance") != std::string::npos) art=6;
        else if (desc.find("classical") != std::string::npos) art=7;
        else if (desc.find("talk") != std::string::npos) art=8;
        else if (desc.find("music") != std::string::npos) art=9;
        else if (desc.find("electronic") != std::string::npos) art=10;
        else if (desc.find("comedy") != std::string::npos) art=11;
        else if (desc.find("country") != std::string::npos) art=12;
        else if (desc.find("80s") != std::string::npos) art=13;
        else if (desc.find("dance") != std::string::npos) art=9;
        else if (desc.find("chillout") != std::string::npos) art=20;
        else if (desc.find("culture") != std::string::npos) art=21;
        else art=0;
        fundet=false;
        sql_update  = fmt::format("select name from radio_stations where stream_url = '{}'",new_radio_record.streamurl);
        mysql_query(conn1,sql_update.c_str());
        res = mysql_store_result(conn1);
        if (res) {
          while ((row = mysql_fetch_row(res)) != NULL) {
            fundet=true;
          }
        }
        // if not found create db record
        if (fundet==false) {
          // get random name back
          get_webfilename(downloadfilename,(char *) gfxurl.c_str());
          downloadfilenamelong = downloadfilename;
          downloadfilenamelong2 = "/opt/mythtv-controller/images/radiostations/";
          downloadfilenamelong2 = downloadfilenamelong2 + downloadfilename;
          radio_download_image((char *) gfxurl.c_str(),(char *) downloadfilenamelong2.c_str());                // download file
          downloadfilenamelong_out = downloadfilenamelong2 + ".png";
          new_radio_record.gfxfilename=downloadfilenamelong;
          if ((!(file_exists(downloadfilenamelong_out.c_str()))) && (file_exists(downloadfilenamelong2.c_str()))) {
            do_cmd = "convert '";
            char filenamepath[1024];
            std::string popenstring="/usr/bin/convert '";
            popenstring = popenstring + downloadfilenamelong2;
            popenstring = popenstring + "' -resize 320x320^ -gravity center -extent 320x320 -crop 320x320+0+0 +repage ";
            popenstring = popenstring + " -set colorspace RGB -type TrueColorAlpha -alpha set -bordercolor none -fill none -draw 'matte 0,0 floodfill' +repage '";
            // popenstring = popenstring + " -set colorspace RGB -alpha set -bordercolor none -border 1 -fuzz 20% -fill none -draw 'matte 0,0 floodfill' -shave 1x1  -channel A -blur 0x1  -trim +repage -type TrueColorAlpha '";
            popenstring = popenstring + downloadfilenamelong_out;
            popenstring = popenstring + "' 2> /dev/null";
            f = popen(popenstring.c_str(), "r");
            if (f) {
              fgets(filenamepath, 1024, f);
              return_code=pclose(f);
            }
            if (return_code==0) {
              downloadfilenamelong3 = downloadfilename;
              popenstring="/usr/bin/convert ";
              popenstring=popenstring + downloadfilenamelong_out;
              popenstring=popenstring + R"delim( \( +clone -alpha transparent -fill white -draw "roundrectangle 0,0 320,320 10,10" \) -alpha set -compose DstIn -composite /opt/mythtv-controller/images/radiostations/)delim";
              size_t pos=(downloadfilenamelong3.find_last_of('.'));
              std::string onlyname=downloadfilenamelong3.substr(0, pos);
              if (onlyname.length()>0) {
                popenstring=popenstring + onlyname;
                popenstring=popenstring + "_r.png";
                f = popen(popenstring.c_str(), "r");
                if (f) {
                  fgets(filenamepath, 1024, f);
                  return_code=pclose(f);
                }
              }
            }
            if (return_code==0) {
              new_radio_record.gfxfilename=downloadfilenamelong3;
            } else new_radio_record.gfxfilename="";
          }
          new_radio_record.textureId=0;
          if (conn1) {
            sql_update  = fmt::format("insert IGNORE INTO radio_stations(name,beskriv,stream_url,homepage,aktiv,art,gfx_link,bitrate,online,landekode,createdate,popular,intnr) values ('{}','{}','{}','{}',{},{},'{}',{},{},{},now(),{},{})",new_radio_record.station_name,desc, new_radio_record.streamurl, new_radio_record.homepage, 1, art, new_radio_record.gfxfilename,new_radio_record.kbps,1,new_radio_record.land,clickcount,0);
            mysql_query(conn1,sql_update.c_str());
            res = mysql_store_result(conn1);
          }
          antal++;
        }
      }
    }
    std::cout << std::endl;
    std::cout << "Done parsing json file." << std::endl;
    mysql_close(conn1);
  }
  if (antal>0) return(1); else return(0);
}


// Next *******************************************************************************************
//
//
//
// *************************************************************************************************

// select next in radio sort option menu
void radiostation_class::nextradiooptselect() {
  if ((radiooptionsselect<40) && (strcmp(radiosortopt[radiooptionsselect+1].radiosortopt,"")!=0)) radiooptionsselect++;
}

// Last *******************************************************************************************
//
//
//
// *************************************************************************************************

// select last in radio sort option menu
void radiostation_class::lastradiooptselect() {
  if (radiooptionsselect>0) radiooptionsselect--;
}


// set en radio icon image

void radiostation_class::set_texture(int nr,GLuint idtexture) {
  stack[nr].textureId=idtexture;
}

// ****************************************************************************************
//
// update radio stations gfx link
//
// ****************************************************************************************

int radiostation_class::opdatere_radiostation_gfx(int nr,char *gfxpath) {
  char sqlselect[512];
  // mysql vars
  MYSQL *conn;
  MYSQL_RES *res;
  sprintf(sqlselect,"update radio_stations set gfx_link='%s' where intnr=%d",gfxpath,nr);
  conn=mysql_init(NULL);
  // Connect to database
  try {
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, dbname, 0, NULL, 0)) {
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
    }
    if (conn) mysql_close(conn);
  }
  catch (...) {
    fprintf(stdout,"Error update radio station db.\n");
    write_logfile(logfile,(char *) "Error update radio station db.");
  }
  return(1);
}

// ****************************************************************************************
//
// load all radio stations gfx
//
// ****************************************************************************************

int radiostation_class::load_radio_stations_gfx() {
  int i=0;
  GLuint texture;  
  std::string tmpfilename;
  std::string gfxfilename;
  char resl[200];
  FILE *filhandle;
  try {
    filhandle=fopen("radio_gfx.log","w");
    if (startup_loaded) return(0);
    startup_loaded=true;
    while(i<radiooversigt.radioantal()) {
      tmpfilename = "/opt/mythtv-controller/images/radiostations/";	// hent path
      gfxfilename = radiooversigt.get_station_gfxfile(i);
      tmpfilename = tmpfilename + gfxfilename;			// add filename to path
      if ((gfxfilename.length() > 0) && (file_exists(tmpfilename.c_str()))) {	// den har et navn samt gfx filen findes.
        texture=loadTexture ((char *) tmpfilename.c_str());					// load texture
        set_texture(i,texture);							// save it in radio station struct to show
      } else if (gfxfilename.length() == 0) {
        // check hvis ikke noget navn om der findes en fil med radio station navnet *.png/jpg
        // hvis der gør load denne fil.
        tmpfilename = "/opt/mythtv-controller/images/radiostations/";
        tmpfilename = tmpfilename + radiooversigt.get_station_name(i);
        tmpfilename = tmpfilename + ".png";      
        if (file_exists(tmpfilename.c_str())) {		// den har et navn samt gfx filen findes.
          texture=loadTexture ((char *) tmpfilename.c_str());                                 // load texture
          set_texture(i,texture);                         		            // save it in radio station struct
          stack[i].gfxfilename=(char *) get_station_name(i);      // update station gfxfilename to station name
          stack[i].gfxfilename = stack[i].gfxfilename + ".png";
          opdatere_radiostation_gfx(stack[i].intnr,(char *) stack[i].gfxfilename.c_str());           // and update db filename
        } else {
          tmpfilename = "/opt/mythtv-controller/images/radiostations/";
          tmpfilename = tmpfilename + radiooversigt.get_station_name(i);
          tmpfilename = tmpfilename + ".jpg";
          if (file_exists(tmpfilename.c_str())) {
            texture=loadTexture ((char *) tmpfilename.c_str());                                        // load texture
            set_texture(i,texture);     		                                                           // save it in radio station struct
            stack[i].gfxfilename=get_station_name(i);                                                  // update station gfxfilename to station name
            stack[i].gfxfilename = stack[i].gfxfilename +".png";
            opdatere_radiostation_gfx(stack[i].intnr,(char *) stack[i].gfxfilename.c_str());           // and update db filename
          }
        }
      } else {
        sprintf(resl,"Radio station gfx file %s for %s is missing.\n",get_station_gfxfile(i),get_station_name(i));
        fputs(resl,filhandle);
      }      
      i++;
    }
    if (filhandle) fclose(filhandle);
  }
  catch (...) {
    fprintf(stdout,"Error loading radio station graphic icons.\nWriting to radio_gfx.log fault.\n");
    write_logfile(logfile,(char *) "Error loading radio station graphic icons.");
  }
  return(1);
}






// ****************************************************************************************
//
// OVERLOAD opdatere_radio_oversigt
//
// ****************************************************************************************

int radiostation_class::opdatere_radio_oversigt() {
  std::string sqlselect_str;
  // mysql vars
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  int art,intnr,kbps;
  int land;
  bool online;
  //gotoxy(10,13);
  //printf("Opdatere radio oversigt fra database. type %d \n",radiosortorder);  
  sqlselect_str ="select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 order by popular desc,name";
  try {
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, dbname, 0, NULL, 0)) {
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      mysql_query(conn,sqlselect_str.c_str());
      res = mysql_store_result(conn);
      if (res) {
        while (((row = mysql_fetch_row(res)) != NULL)) {
          radio_oversigt_type new_station;
          // printf("Hent info om radio station nr %s %-20s\n",row[6],row[0]);
          art=atoi(row[3]);
          intnr=atoi(row[6]);
          kbps=atoi(row[7]);
          online=atoi(row[8]);
          land=atoi(row[9]);
          strncpy(new_station.station_name,row[0],stationamelength);
          new_station.desc=std::string(row[4]);
          new_station.streamurl=std::string(row[1]);
          new_station.homepage=std::string(row[2]);
          new_station.gfxfilename=std::string(row[5]);
          new_station.art=art;
          new_station.kbps=kbps;
          new_station.online=online;
          new_station.land=land;
          new_station.textureId=0;
          new_station.intnr=intnr;
          stack.push_back(new_station);
          antal++;
        }
        if (antal==0) printf("No Radio station loaded");
      } else {
        fprintf(stderr,"\nFailed to update radiodb, can not connect to database: mythtvcontroller Error: %s\n",mysql_error(conn));
      }
      //load_radio_stations_gfx();
      return(antal-1);
    } else {
      fprintf(stderr,"\nFailed to update radiodb, can not connect to database: mythtvcontroller Error: %s\n",mysql_error(conn));
    //  exit(0);
    }
    if (conn) mysql_close(conn);
  }
  catch (...) {
    fprintf(stdout,"Error connect to radio station db.\n");
    write_logfile(logfile,(char *) "Error connect to mysql radio station db.");
  }
  return(0);
}


// ****************************************************************************************
//
// search radio station in db after searchtxt
// OVERLOAD
//
// ****************************************************************************************

int radiostation_class::opdatere_radio_oversigt(char *searchtxt) {
    std::string sqlselect_str;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    int art,intnr,kbps;
    int land;
    bool online;
    sqlselect_str = "select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and name like '%";
    sqlselect_str += searchtxt;
    sqlselect_str += "%'";
    try {
      conn=mysql_init(NULL);
      // Connect to database
      if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, dbname, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,sqlselect_str.c_str());
        res = mysql_store_result(conn);
        if (res) {
          while (((row = mysql_fetch_row(res)) != NULL)) {
            radio_oversigt_type new_station;
            art=atoi(row[3]);
            intnr=atoi(row[6]);
            kbps=atoi(row[7]);
            online=atoi(row[8]);
            land=atoi(row[9]);
            strncpy(new_station.station_name,row[0],stationamelength);
            new_station.desc=std::string(row[4]);
            new_station.streamurl=std::string(row[1]);
            new_station.homepage=std::string(row[2]);
            new_station.gfxfilename=std::string(row[5]);
            new_station.art=art;
            new_station.kbps=kbps;
            new_station.online=online;
            new_station.land=land;
            new_station.textureId=0;
            new_station.intnr=intnr;
            new_station.noiconloaded=false;
            stack.push_back(new_station);
            antal++;
          }
        }
        if (antal>0) return(antal-1); else return(0);
      } else fprintf(stderr,"Failed to connect to database: Error: %s\n",mysql_error(conn));
      if (conn) mysql_close(conn);
    }
    catch (...) {
      fprintf(stdout,"Error connect to mysql radio station db.\n");
      write_logfile(logfile,(char *) "Error connect to mysql radio station db.");
    }
    return(0);
}



// ****************************************************************************************
//
// Opdatere liste efter sort order (radiosortorder)
// OVERLOAD
//
// ****************************************************************************************

int radiostation_class::opdatere_radio_oversigt(int radiosortorder) {
    // char sqlselect[512];
    std::string sqlselect_str;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    int art,intnr,kbps;
    int land;
    bool online;
    //gotoxy(10,13);
    //printf("Opdatere radio oversigt fra database. type %d \n",radiosortorder);
    if (do_sqlite) {

    } else {
      if (radiosortorder==0)			// start order default
        sqlselect_str = "select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by popular desc,name";
      else if (radiosortorder==28)		// bit rate
        sqlselect_str = "select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by bitrate desc,popular desc,name";
      else if (radiosortorder==27)		// land kode
        sqlselect_str = "select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by landekode desc,popular desc,name";
      else if (radiosortorder==19)		// mest hørt
        sqlselect_str = "select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 order by lastplayed desc,popular desc,name";
      else 					// ellers efter art
        sqlselect_str = fmt::format("select name,stream_url,homepage,art,beskriv,gfx_link,intnr,bitrate,online,landekode from radio_stations where aktiv=1 and online=1 and art={} order by popular desc,name",radiosortorder);
      try {
        conn=mysql_init(NULL);
        // Connect to database
        if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, dbname, 0, NULL, 0)) {
          mysql_query(conn,"set NAMES 'utf8'");
          res = mysql_store_result(conn);
          mysql_query(conn,sqlselect_str.c_str());
          res = mysql_store_result(conn);
          if (res) {
            while (((row = mysql_fetch_row(res)) != NULL)) {
              radio_oversigt_type new_station;
              art=atoi(row[3]);
              intnr=atoi(row[6]);
              kbps=atoi(row[7]);
              online=atoi(row[8]);
              land=atoi(row[9]);
              strncpy(new_station.station_name,row[0],stationamelength);
              new_station.desc=std::string(row[4]);
              new_station.streamurl=std::string(row[1]);
              new_station.homepage=std::string(row[2]);
              new_station.gfxfilename=std::string(row[5]);
              new_station.art=art;
              new_station.kbps=kbps;
              new_station.online=online;
              new_station.land=land;
              new_station.textureId=0;
              new_station.intnr=intnr;
              new_station.noiconloaded=false;
              stack.push_back(new_station);
              antal++;
            }
            if (antal==0) printf("No Radio station loaded");
          } else {
            fprintf(stderr,"Failed to update radiodb, can not connect to database: mythtvcontroller Error: %s\n",mysql_error(conn));
          }
          if (conn) mysql_close(conn);
          //load_radio_stations_gfx();
          return(antal-1);
        } else {
          fprintf(stderr,"Failed to update radiodb, can not connect to database: mythtvcontroller Error: %s\n",mysql_error(conn));
        }
        if (conn) mysql_close(conn);
      }
      catch (...) {
        fprintf(stdout,"Error connect to mysql radio station db.\n");
        write_logfile(logfile,(char *) "Error connect to mysql radio station db.");
      }
    }
    return(0);
}


// ****************************************************************************************
//
// show radio stations overview
//
// ****************************************************************************************

/*
bool radiostation_class::show_radio_oversigt(GLuint normal_icon,GLuint normal_icon_mask,GLuint back_icon,GLuint dirplaylist_icon,int _mangley) {
  static bool show_all_kode_errors=false;
  int buttonsizex = config_menu.config_radio_main_window_icon_sizex;
  int buttonsizey = config_menu.config_radio_main_window_icon_sizey;
  int xof=config_menu.config_radio_main_windowx;
  int yof=orgwinsizey-(buttonsizey);
  // yof=config_menu.config_radio_main_window_sizey-(buttonsizey);
  int boffset=154;
  int bonline=8;                        // numbers of icons pr line
  int i=0;
  int sofset=0;
  int lradiooversigt_antal=(bonline*5);
  
  int xx=(float) (config_menu.config_radio_main_window_sizex/config_menu.config_radio_main_window_icon_sizex)-1;
  int yy=(float) (config_menu.config_radio_main_window_sizey/config_menu.config_radio_main_window_icon_sizey)-1;
  lradiooversigt_antal = xx*yy;        // 

  int buttonzoom;
  char tmpfilename[200];
  char gfxfilename[200];
  char temptxt[200];
  char *lastslash;
  bool radio_pictureloaded=true;
  const char *radiostation_iconsgfx="/opt/mythtv-controller/images/radiostations/";
  char *base,*right_margin;
  int length,width;
  int pline=0;
  sofset=(_mangley/40)*8;
  GLuint texture;
  if (screen_size==2) {
    bonline=6;
  }
  if (screen_size==4) {
    bonline=7;
    buttonsizex=160;
    buttonsizey=140;
  }
  // er gfx loaded
  // if no load 1 at eatch run
  // loader
  if ((radio_oversigt_loaded==false) && (radio_oversigt_loaded_nr<radiooversigt.radioantal())) {
    radio_oversigt_loaded_begin=true;
    radio_pictureloaded=false;
    strcpy(tmpfilename,radiostation_iconsgfx);      		                                    // hent path
    strcpy(gfxfilename,radiooversigt.get_station_gfxfile(radio_oversigt_loaded_nr));        // hent radio icon gfx filename
    strcat(tmpfilename,gfxfilename);        						// add filename to path
    if ((strcmp(gfxfilename,"")!=0) && (file_exists(tmpfilename))) {			// den har et navn samt gfx filen findes.
      texture=loadTexture ((char *) tmpfilename);						// load texture
      set_texture(radio_oversigt_loaded_nr,texture);					// save it in radio station struct to show
    } else if (strcmp(gfxfilename,"")==0) {
      // check hvis ikke noget navn om der findes en fil med radio station navnet *.png/jpg
      // hvis der gør load denne fil.
      strcpy(tmpfilename,radiostation_iconsgfx);
      strcat(tmpfilename,radiooversigt.get_station_name(radio_oversigt_loaded_nr));
      strcat(tmpfilename,".png");
      if (file_exists(tmpfilename)) {
        texture=loadTexture ((char *) tmpfilename);                      						           // load texture
        set_texture(radio_oversigt_loaded_nr,texture);                  				       		            // save it in radio station struct
        strncpy(stack[radio_oversigt_loaded_nr].gfxfilename,get_station_name(radio_oversigt_loaded_nr),stationamelength-1);      // update station gfxfilename to station name
        strcat(stack[radio_oversigt_loaded_nr].gfxfilename,".png");
        opdatere_radiostation_gfx(stack[radio_oversigt_loaded_nr].intnr,stack[radio_oversigt_loaded_nr].gfxfilename);           // and update db filename
      } else {
        strcpy(tmpfilename,radiostation_iconsgfx);
        strcat(tmpfilename,radiooversigt.get_station_name(radio_oversigt_loaded_nr));
        strcat(tmpfilename,".jpg");
        if (file_exists(tmpfilename)) {
          texture=loadTexture ((char *) tmpfilename);                                 // load texture
          set_texture(radio_oversigt_loaded_nr,texture);     		                                // save it in radio station struct
          strncpy(stack[radio_oversigt_loaded_nr].gfxfilename,get_station_name(radio_oversigt_loaded_nr),stationamelength-1);      // update station gfxfilename to station name
          strcat(stack[radio_oversigt_loaded_nr].gfxfilename,".png");
          opdatere_radiostation_gfx(stack[radio_oversigt_loaded_nr].intnr,stack[radio_oversigt_loaded_nr].gfxfilename);           // and update db filename
        }
      }
    }
    if (radio_oversigt_loaded_nr>=radiooversigt.radioantal()-1) {
      radio_oversigt_loaded=true;
      radio_oversigt_loaded_done=true;
    } else radio_oversigt_loaded_nr++;
  } 
  glPushMatrix();
  while((i<lradiooversigt_antal) && ((int) i+(int) sofset<(int) antal)) {
    if (((i % bonline)==0) && (i>0)) {
      xof=config_menu.config_radio_main_windowx;	// reset xof
      yof=yof-(config_menu.config_radio_main_window_icon_sizey+20); // old buttonsizey
    }
    if (i+1==(int) radio_key_selected) {
      buttonsizex=config_menu.config_radio_main_window_icon_sizex;
      buttonsizey=config_menu.config_radio_main_window_icon_sizey;
    } else {
      buttonsizex=config_menu.config_radio_main_window_icon_sizex-5;
      buttonsizey=config_menu.config_radio_main_window_icon_sizey-5;
    }
    if (stack[i+sofset].textureId) {
      // radio default icon
      glPushMatrix();
      // is the radio station online
      // if NOT show faded colors
      if (stack[i+sofset].online) glColor4f(1.0f, 1.0f, 1.0f,1.0f); else glColor4f(.3f, .3f, .3f, 1.0f);
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); 
      if ((i+1==(int) radio_key_selected)) {
        glBindTexture(GL_TEXTURE_2D,onlineradio_selected);
      } else {
        glBindTexture(GL_TEXTURE_2D,onlineradio_empty);
      }
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
      glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsizey, 0.0);
      glTexCoord2f(1, 1); glVertex3f( xof+buttonsizex, yof+buttonsizey , 0.0);
      glTexCoord2f(1, 0); glVertex3f( xof+buttonsizex,yof , 0.0);
      glEnd();
      glPopMatrix();
      glPushMatrix();
      // is the radio station online
      // if NOT show faded colors
      if (stack[i+sofset].online) glColor4f(1.0f, 1.0f, 1.0f,1.0f); else glColor4f(.3f, .3f, .3f, 1.0f);
      // indside draw radio station icon
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_SRC0_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBlendEquation(GL_FUNC_ADD);
      //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      glBindTexture(GL_TEXTURE_2D,stack[i+sofset].textureId);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(100+i+sofset);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
      glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
      glTexCoord2f(1, 1); glVertex3f( xof+buttonsizex-10, yof+buttonsizey-20 , 0.0);
      glTexCoord2f(1, 0); glVertex3f( xof+buttonsizex-10, yof+10 , 0.0);
      glEnd();
      glPopMatrix();
    } else {
      // default icon
      glPushMatrix();
      if (stack[i+sofset].online) glColor4f(1.0f, 1.0f, 1.0f,1.0f); else glColor4f(.3f, .3f, .3f, 1.0f);
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      if ((i+1==(int) radio_key_selected)) {
        glBindTexture(GL_TEXTURE_2D,onlineradio_selected);
      } else {
        glBindTexture(GL_TEXTURE_2D,onlineradio);
      }
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(100+i+sofset);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
      glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsizey, 0.0);
      glTexCoord2f(1, 1); glVertex3f( xof+buttonsizex, yof+buttonsizey , 0.0);
      glTexCoord2f(1, 0); glVertex3f( xof+buttonsizex,yof , 0.0);
      glEnd();
      glPopMatrix();
    }
    // draw radio station contry flags
    if (stack[i+sofset].land>0) {
      // gfxlandemask mask
      if (gfxlande[stack[i+sofset].land]) {
        glBindTexture(GL_TEXTURE_2D,gfxlande[stack[i+sofset].land]);       //

        //glBindTexture(GL_TEXTURE_2D,gfxlande[i+sofset]);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(10+ xof, yof+10 , 0.0);
        glTexCoord2f(0, 1); glVertex3f(10+ xof,yof+30+10, 0.0);
        glTexCoord2f(1, 1); glVertex3f(10+ xof+40, yof+30+10 , 0.0);
        glTexCoord2f(1, 0); glVertex3f(10+ xof+40,yof +10, 0.0);
        glEnd();
      } else {
        //if (debugmode & 1024) printf("Contry kode %d missing flag, File name %s\n",stack[i+sofset]->land,gfxlande[stack[i+sofset]->land]);
        // write debug log
        sprintf(debuglogdata,"Contry code %d missing flag, File name %d",stack[i+sofset].land,gfxlande[stack[i+sofset].land]);
        if (gfxlande[stack[i+sofset].land]==0) {
          if (show_all_kode_errors==false) write_logfile(logfile,(char *) debuglogdata);
        } else if (show_all_kode_errors==false) write_logfile(logfile,(char *) debuglogdata);
      }
    }
    // print radios station name
    // strcpy(temptxt,stack[i+sofset]->station_name);        // radio station navn
    std::string temptxt1;
    temptxt1 = fmt::format("{:^24}",stack[i+sofset].station_name);
    temptxt1.resize(24);
    drawText(temptxt1.c_str(), xof+2, yof-18, 0.4f,1);
    xof=xof+buttonsizex+6;
    i++;
  }
  radio_oversigt_antal=radiooversigt.radioantal();
  if (i==0) {
    // show error message
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_ONE, GL_ONE);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D,_textureIdloading);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f((orgwinsizex/3), 200 , 0.0);
    glTexCoord2f(0, 1); glVertex3f((orgwinsizex/3), 200+150, 0.0);
    glTexCoord2f(1, 1); glVertex3f((orgwinsizex/3)+450, 200+150 , 0.0);
    glTexCoord2f(1, 0); glVertex3f((orgwinsizex/3)+450, 200 , 0.0);
    glEnd();
    drawText("Error no radio stations load", (orgwinsizex/3)+30, 275.0f, 0.4f,15);
    write_logfile(logfile,(char *) "Error no radio stations load.");
  }
  glPopMatrix();
  show_all_kode_errors=true;                                                  // stop loging.
  return(radio_pictureloaded);
}


*/





// All new ********************************************************************************************************************************************
//
//


// ****************************************************************************************
//
// draw rectangle outline
//
// ****************************************************************************************

void drawRect(int x, int y, int w, int h, Color2 c) {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_LINE_LOOP);
  glVertex2i(x,     y);
  glVertex2i(x + w, y);
  glVertex2i(x + w, y + h);
  glVertex2i(x,     y + h);
  glEnd();
}



// ****************************************************************************************
//
// draw cover gfx
//
// ****************************************************************************************

void drawcover(int x, int y, int w, int h, GLuint textureId ,  GLuint textureId2,int id,Color2 c) {
  glEnable(GL_TEXTURE_2D);
  glColor4f(c.r, c.g, c.b, c.a);
  // draw actual cover
  glBindTexture(GL_TEXTURE_2D, textureId);
  glLoadName(id);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex2i(x + 10,          y + 10);
  glTexCoord2f(1, 0); glVertex2i(x + 10 + w - 20, y + 10);
  glTexCoord2f(1, 1); glVertex2i(x + 10 + w - 20 ,y + h - 10);
  glTexCoord2f(0, 1); glVertex2i(x + 10,          y + h - 10);
  glEnd();
  // icon  
  glBindTexture(GL_TEXTURE_2D, textureId2);
  glLoadName(id);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex2i(x,     y);
  glTexCoord2f(1, 0); glVertex2i(x + w, y);
  glTexCoord2f(1, 1); glVertex2i(x + w, y + h);
  glTexCoord2f(0, 1); glVertex2i(x,     y + h);
  glEnd();
}


// ****************************************************************************************
//
// draw single radio item
//
// ****************************************************************************************

void radiostation_class::draw_radio_item(int x, int y,int ii,GLuint normal_icon,GLuint empty_icon, int radio_key_selected) {
  // Baggrund
  static float sinh=0.0;
  std::string temprgtxt;
  std::string gfxfilename;
  GLuint texture;
  Color2 highcolor={0.30f, 0.50f, 0.90f, 1.0f};
  Color2 normalcolor={0.15f, 0.15f, 0.15f, 1.0f};
  // Cover
  gfxfilename = "/opt/mythtv-controller/images/radiostations/";
  gfxfilename = gfxfilename + stack[ii].gfxfilename;
  gfxfilename = gfxfilename + ".png";
  if (stack[ii].gfxfilename.length()>0) {
    // load texture if not loaded
    if (stack[ii].textureId == 0) {
      if ((file_exists(gfxfilename.c_str())) && (stack[ii].noiconloaded==false)) {
        stack[ii].textureId = loadTexture((char *) gfxfilename.c_str());
        if (stack[ii].textureId==0) stack[ii].noiconloaded=true; else stack[ii].noiconloaded=false;
      } else stack[ii].gfxfilename="";
    }
  }
  // Titel
  // temprgtxt = fmt::format("{:^38}",stack[ii].station_name);
  temprgtxt = stack[ii].station_name;
  // temprgtxt.resize(20);
  if (stack[ii].textureId ) texture = stack[ii].textureId; else texture = empty_icon;
  if (stack[ii].textureId ) {
    if (ii == selected_icon_in_view-1) {
      drawcover(x + 18, y + 18, 164 + sin(sinh)*4, 164 + sin(sinh)*4, texture , onlineradio_selected ,ii+100,highcolor);
      drawLinesOfText(temprgtxt, x + 18, y + 4, 1.0f, 22, 3, 2, true);
      sinh = sinh + 0.08f;
      if (sinh>(M_PI*2)) sinh=0.0f;
    } else {
      drawcover(x + 20, y + 20, 160, 160, texture , onlineradio_empty ,ii+100,normalcolor);
      drawLinesOfText(temprgtxt, x + 18, y + 4, 1.0f, 22, 3, 15, true);
    }
  } else {
    if (ii == selected_icon_in_view-1) {                                                                       // old if (ii == radio_key_selected-1) {
      drawcover(x + 18, y + 18, 164 + sin(sinh)*4, 164 + sin(sinh)*4, texture , onlineradio_selected ,ii+100,highcolor);
      drawLinesOfText(temprgtxt, x + 18, y + 4, 1.0f, 22, 3, 2, true);
      sinh = sinh + 0.08f;
      if (sinh>(M_PI*2)) sinh=0.0f;
    } else {
      drawcover(x + 20, y + 20, 160, 160, texture , onlineradio ,ii+100,normalcolor);
      drawLinesOfText(temprgtxt, x + 18, y + 4, 1.0f, 22, 3, 15, true);
    }
  }
}



// ****************************************************************************************
//
// draw single search radio item
//
// ****************************************************************************************


void radiostation_class::draw_radio_search_item(int x, int y,int ii,GLuint normal_icon,GLuint empty_icon, int radio_key_selected) {
  static float sinh=0.0;
  // Baggrund
  std::string temprgtxt;
  std::string gfxfilename;
  GLuint texture;
  Color2 highcolor={0.30f, 0.50f, 0.90f, 1.0f};
  Color2 normalcolor={0.15f, 0.15f, 0.15f, 1.0f};
  // Cover
  gfxfilename = "/opt/mythtv-controller/images/radiostations/";
  gfxfilename = gfxfilename + stack[ii].gfxfilename;
  if (stack[ii].gfxfilename.length()>0) {
    // load texture if not loaded
    if (stack[ii].textureId == 0) {
      if (file_exists(gfxfilename.c_str())) {
        stack[ii].textureId = loadTexture((char *) gfxfilename.c_str());
      } else stack[ii].gfxfilename="";
    }
  }
  // Titel
  // temprgtxt = fmt::format("{:^38}",stack[ii].station_name);
  temprgtxt = stack[ii].station_name;
  // temprgtxt.resize(20);
  if (stack[ii].textureId ) texture = stack[ii].textureId; else texture = empty_icon;
  if (stack[ii].textureId ) {
    if (ii == selected_icon_in_view-1) {                                                                           // old if (ii == radio_key_selected-1) {
      drawcover(x + 18, y + 18, 164 + sin(sinh)*2, 164 + sin(sinh)*2, texture , onlineradio_selected ,ii+100,highcolor);
      drawLinesOfText(temprgtxt, x + 18, y + 4,   1.0f, 22, 3, 2, true);
      sinh = sinh + 0.2f;
      if (sinh>(M_PI*2)) sinh=0.0f;
    } else {
      drawcover(x + 20, y + 20, 160, 160, texture , onlineradio_empty ,ii+100,normalcolor);
      drawLinesOfText(temprgtxt, x + 18, y + 4, 1.0f, 22, 3, 15, true);
    }
  } else {
    if (ii == selected_icon_in_view-1) {                                                                       // old if (ii == radio_key_selected-1) {
      if (y<search_startY-30) {
        drawcover(x + 18, y + 18 , 164 + sin(sinh)*2, 164 + sin(sinh)*2, texture , onlineradio_selected ,ii+100,highcolor);
        drawLinesOfText(temprgtxt, x + 18, y + 4 , 1.0f, 22, 3, 2, true);        
      }
      sinh = sinh + 0.1f;
      if (sinh>(M_PI*2)) sinh=0.0f;
    } else {
      if (y<search_startY-30) {
        drawcover(x + 20, y + 20, 160, 160, texture , onlineradio ,ii+100,normalcolor);
        drawLinesOfText(temprgtxt, x + 18, y + 4, 1.0f, 22, 3, 15, true);
      }
    }
  }
}




// ****************************************************************************************
//
// get text width
//
// ****************************************************************************************


float radio_getTextWidth(const std::string& text, float scale) {
  float width = 0.0f;
  for (char c : text){
    Character ch = characters[c];
    width += ch.advance * scale;  // glyph advance
  }
  return width;
}


// ****************************************************************************************
//
// show radio stations overview new version 2
//
// ****************************************************************************************


bool radiostation_class::show_radio_oversigt(GLuint normal_icon,GLuint normal_icon_mask,GLuint back_icon,GLuint dirplaylist_icon,int _mangley) {
  static bool cursor;
  float yof_top=orgwinsizey-(rowHeight*1)+20;                               // start ypos
  float xof_top=((orgwinsizex-itemWidth)/2)-(1200/2);
  // ---- KINETIC SCROLL ---------------------------------------
  scrollVel *= friction;
  scrollPos += scrollVel;
  if (fabs(scrollVel) < 0.01f) scrollVel = 0;
  int totalRows   = (int)ceil((float)stack.size() / itemsPerRow);
  int visibleRows = viewHeight / rowHeight;
  float maxScroll = std::max(0.0f, (float)(totalRows - visibleRows) * rowHeight);
  if (scrollPos < 0) {
      scrollPos = 0;
      scrollVel = 0;
  } else if (scrollPos > maxScroll) {
      scrollPos = maxScroll;
      scrollVel = 0;
  }
  // ---- CALC --------------------------------------------------
  int firstRow   = (int)(scrollPos / rowHeight);
  float subOff   = fmod(scrollPos, rowHeight);
  int ssofset     = firstRow * itemsPerRow;
  int screenTop = startY;                                 // start position for first item y 
  int xof = startX;                                       // start position for first item x
  int visibleItems = (visibleRows + 2) * itemsPerRow;
  if (strlen(keybuffer)>0) {
    screenTop=search_startY;
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,tidal_big_search_bar_artist);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(0);
    glBegin(GL_QUADS); 
    glTexCoord2f(0, 0); glVertex3f( xof_top+10, yof_top+10, 0.0);
    glTexCoord2f(0, 1); glVertex3f( xof_top+10,yof_top+rowHeight-20, 0.0);
    glTexCoord2f(1, 1); glVertex3f( xof_top+1200-10, yof_top+rowHeight-20 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( xof_top+1200-10, yof_top+10 , 0.0);
    glEnd();
    // show seach string
    if (strcmp(keybuffer,"")!=0) {
      drawText(font24, keybuffer, 300.0f, 980.0f, 1.0f, 0);
      float textWidth = radio_getTextWidth(keybuffer, 1.0f);
      if (cursor) drawText(font24, "_", 300.0f+textWidth, 980.0f, 1.0f, 0);
    }
    printf("Keybuffer = %s \n",keybuffer);
    // ---- RENDER -----------------------------------------------
    for (int i = 0; i < visibleItems && (ssofset + i) < stack.size(); ++i) {
      int index = ssofset + i;
      int col = i % itemsPerRow;
      int row = i / itemsPerRow;
      int x = xof + col * itemWidth + 40;
      int y = screenTop - (row * rowHeight) + subOff - 40;    
      draw_radio_search_item( x, y, index, normal_icon, dirplaylist_icon, radio_key_selected);
    }
  } else {
    // ---- RENDER -----------------------------------------------
    for (int i = 0; i < visibleItems && (ssofset + i) < stack.size(); ++i) {
      int index = ssofset + i;
      int col = i % itemsPerRow;
      int row = i / itemsPerRow;
      int x = xof + col * itemWidth + 40;
      int y = screenTop - (row * rowHeight) + subOff - 40;
      draw_radio_item( x, y, index, normal_icon, dirplaylist_icon, radio_key_selected);
    }
  }
  return(true);
}



// ****************************************************************************************
//
// skal vi opdatere sort type oversigt første gang
//
// ****************************************************************************************

void radiostation_class::show_radio_options() {
  int i;  
  std::string sqlselect_str;
  // mysql vars
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char tmptxt[80];
  float sizex=4.0f;
  float sizey=3.0f;
  int winsizx=1200;
  int winsizy=800;
  int xpos=0;
  int ypos=0;
  // background
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBindTexture(GL_TEXTURE_2D, radiooptions);					// _textureId18);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_QUADS); //Begin quadrilateral coordinates
  // draw  front box
  glTexCoord2f(0, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2)) , 0.0);
  glEnd();
  glPopMatrix();
  i=0;
  if (hentradioart==false) {
    hentradioart=true;
    sqlselect_str ="SELECT typename,radiotypes.art,count(radio_stations.art) FROM `radiotypes`,radio_stations where radiotypes.art=radio_stations.art or radiotypes.art=0 and radio_stations.online=1 group by (radiotypes.art)";
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, dbname, 0, NULL, 0)) {
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      mysql_query(conn,sqlselect_str.c_str());
      res = mysql_store_result(conn);
      while (((row = mysql_fetch_row(res)) != NULL) && (i<radiooptionsmax)) {
        strcpy(radiosortopt[i].radiosortopt,row[0]);
        radiosortopt[i].radiosortoptart=atoi(row[1]);
        radiosortopt[i].antal=atoi(row[2]);
        i++;
      }
    }
  }
  drawText(font12, "Sort options.", 410.0f, 770.0f, 0.8f,1);
  i=0;
  while ((strcmp(radiosortopt[i].radiosortopt,"")!=0) && (i<40)) {
    if (i!=radiooptionsselect) drawText(font12, radiosortopt[i].radiosortopt, 500.0f, 700-(i*20.0f), 0.4f,1);
    else drawText(font12, radiosortopt[i].radiosortopt, 500.0f, 700-(i*20.0f), 0.4f,2);
    sprintf(tmptxt,"%5d",radiosortopt[i].antal);
    if (i!=radiooptionsselect) drawText(font12, tmptxt, 1000.0f, 700-(i*20.0f), 0.4f,1);
    else drawText(font12, tmptxt, 1000.0f, 700-(i*20.0f), 0.4f,2);
    i++;
  }
}




// ****************************************************************************************
//
// opdatere list set numbers of aflytninger
//
// ****************************************************************************************

int radiostation_class::set_radio_popular(int stationid) {
  char sqlselect[512];
  MYSQL *conn;
  MYSQL_RES *res;
  // write debug log
  write_logfile(logfile,(char *) "Update played radio station.");
  sprintf(sqlselect,"update radio_stations set popular=popular+1,lastplayed=now() where intnr=%ld",stack[stationid].intnr);  
  conn=mysql_init(NULL);
  // Connect to database
  if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, dbname, 0, NULL, 0)) {
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    mysql_close(conn);
    return(1);
  } else write_logfile(logfile,(char *) "Error update radiostation lastplayed in db.");
  return(0);
}




// ****************************************************************************************
//
// set online/offline status on radio station (stationid)
//
// ****************************************************************************************

int radiostation_class::set_radio_online(int stationid,bool onoff) {
  char sqlselect[512];
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  write_logfile(logfile,(char *) "Update played radio station online.");
  if (onoff) sprintf(sqlselect,"update radio_stations set online=1 where intnr=%ld",stack[stationid].intnr);
    else sprintf(sqlselect,"update radio_stations set online=0 where intnr=%ld",stack[stationid].intnr);
  conn=mysql_init(NULL);
  // Connect to database
  if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, dbname, 0, NULL, 0)) {
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        ;
      }
    }
    mysql_close(conn);
    return(1);
  }
  stack[stationid].online=onoff;
  return(0);
}

// ****************************************************************************************
//
// get radio online flag
//
// ****************************************************************************************

int radiostation_class::get_radio_online(int stationid) {
  if (((unsigned int) stationid<(unsigned int) antal)) {
      return(stack[stationid].online);
  } else {
      return(-1);
  }
}

// ****************************************************************************************
//
// set radio station online flag internal use
//
// ****************************************************************************************

int radiostation_class::set_radio_intonline(int arraynr) {
  if (((unsigned int) arraynr<(unsigned int) antal)) stack[arraynr].online=true; else return(0);
  return(1);
}

// ****************************************************************************************
//
// check if radio station is ofline in database and return true if redio station exist in db
//
// ****************************************************************************************

bool radiostation_class::check_radio_online_bool() {
  char sqlselect[512];
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  try {
    conn=mysql_init(NULL);
    strcpy(sqlselect,"select intnr from radio_stations where online=0 order by intnr limit 100");
    if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, dbname, 0, NULL, 0)) {
      mysql_query(conn,"set NAMES 'utf8'");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      mysql_close(conn);
      return(1);
    } else return(0);		// we are done check all radio stations in database
  }
  catch (...) {
    fprintf(stdout,"Error connect to mysql db.\n");
    write_logfile(logfile,(char *) "Error connect to mysql radio station db.");
  }
  return(0);
}


// ****************************************************************************************
//
//
//
// ****************************************************************************************

int radiostation_class::set_radio_aktiv(int stationid,bool onoff) {
//    stack[stationid]->intnr;
  stack[stationid].aktiv=onoff;
  return(0);
}

// ****************************************************************************************
//
//
//
// ****************************************************************************************


int init_sockaddr (struct sockaddr_in *name,const char *hostname,uint16_t port) {
  int error=0;
  struct hostent *hostinfo;
  name->sin_family = AF_INET;
  name->sin_port = htons (port);
  hostinfo = gethostbyname (hostname);
  if (hostinfo == NULL) {
    fprintf (stderr, "Unknown host %s.\n", hostname);
    error=1;
  }
  if (error==0) name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
  return(error);
}

// ****************************************************************************************
//
// returm port nr from url if exist in url or 0
//
// ****************************************************************************************

int get_url_data(char *url,char *ipadd) {
  char *sted;
  char *langurl=NULL;
  int destport=0;
  sted=(char*) strrchr(url, ':');		               // find : fra start af url
  if (sted) {
    if (strncmp(sted,"://",3)!=0) {		             // er vi fra start af url
      destport=atoi(sted+1);		                   // hent port nr
      if (strncmp(url,"http",4)==0) {
        *sted='\0';
        strcpy(ipadd,url+7); 		                   // get rest of url
      } else if (strncmp(url,"mms",3)==0) {
        *sted='\0';
        strcpy(ipadd,url+6); 		                   // get rest of url
      }
      langurl=strchr(ipadd, '/');		               // find first /
      if  (langurl) *langurl='\0';
    } else {
      if (strncmp(url,"http",4)==0) {
        strcpy(ipadd,url+7);		                   // get rest of url
      }
      if (strncmp(url,"mms",3)==0) {
        strcpy(ipadd,url+6);                       // get rest of url
      }
      langurl=strchr(ipadd, '/');		               // find first /
      if  (langurl) *langurl='\0';
    }
  }
  return(destport);
}



// ****************************************************************************************
//
// check if radio station is ofline in database and set it online again if it is back online
// return where we are ind the database
// if start recordnr>0 do it
//
// ****************************************************************************************

bool check_radio_online_switch=true;

unsigned long radiostation_class::check_radio_online(unsigned int radioarrayid) {
  short int port=0;
  int sock;
  struct sockaddr_in servername;
  char hostname[1024];
  char ipadresse[1024];
  char st_name[1024];
  int error=0;
  unsigned long radiostation=0;
  char sqlselect[512];
  char sqlselect1[512];
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  int nn;
  bool nfundet;
  bool radiook=false;
  bool cerror;
  struct timeval tv;
  std::string debuglogdata;
  fd_set myset;
  if (check_radio_online_switch) {
    conn=mysql_init(NULL);
    strcpy(sqlselect,"select name,aktiv,intnr,stream_url from radio_stations where online=1 and aktiv=1 order by popular desc,name limit 1");
    if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, dbname, 0, NULL, 0)) {
      mysql_query(conn,"set NAMES 'utf8'");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          write_logfile(logfile,(char *) "Checking radio stations.");
          // port=80;
          strncpy(hostname,row[3],1000);
          strncpy(st_name,row[0],1000);
          if (strcmp(hostname,"")!=0) {
            // get port and ip
            port=get_url_data(hostname,ipadresse);
            // write debug log
            debuglogdata=fmt::format("Checking Station : {} - hostname : {} port {}",row[0],hostname,port);
            write_logfile(logfile,(char *) debuglogdata.c_str());
            sock=socket(PF_INET, SOCK_STREAM, 0);
            if (sock) {
              //fcntl(sock, F_SETFL, O_NONBLOCK);
              tv.tv_sec = 5;
              tv.tv_usec = 0;
              FD_ZERO(&myset);
              FD_SET(sock, &myset);
              error=(init_sockaddr(&servername,ipadresse,port));
              if ((error==0) && (cerror=connect(sock,(struct sockaddr *) &servername,sizeof (servername)))) {
                if (cerror==0) {
                  write_logfile(logfile,(char *) "Station OK.");
                  radiook=true;
                } else radiook=false;
              } else {
                write_logfile(logfile,(char *) "Station BAD.");
                radiook=false;
              }
              close (sock);
            }
            radiostation=atol(row[2]);
            // find radio station og disable
            nn=0;
            // find radio station
            nfundet=false;
            while ((nn<antal) && (nfundet==false)) {
              if  (stack[nn].station_name) {
                // if found set active again
                if (strcmp(stack[nn].station_name,st_name)==0) {
                  if (radiook) stack[nn].online=1; else stack[nn].online=0;
                  nfundet=true;
                } else nn++;
              } else nn++;
            }
          }
        }
      }
      if (nfundet) printf("***** Radio station %ld fundet.\n",radiostation); else printf("***** Radio station not fundet.\n");
      if ((conn) && (radiostation)) {
        if ((radiook) && (nfundet)) {
          sprintf(sqlselect1,"update radio_stations set online=1 where intnr=%ld \n",radiostation);
        } else {
          sprintf(sqlselect1,"update radio_stations set online=0,aktiv=0 where intnr=%ld \n",radiostation);
        }
        if (conn) {
          mysql_query(conn,sqlselect1);
          res = mysql_store_result(conn);
          write_logfile(logfile,(char *) "Set acive flag for radio station.");
          write_logfile(logfile,(char *) sqlselect1);
        }
      }
    }
    if (conn) mysql_close(conn);
  }
  printf("radiostation=%ld\n",radiostation);
  return(radiostation);		// we are done check all radio stations in database
}

