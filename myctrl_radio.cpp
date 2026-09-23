#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <mysql.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <fmt/format.h>
#include <curl/curl.h>                  // lib curl
// json parser
#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>

#include "renderer.h"
#include "myctrl_readwebfile.h"
#include "myctrl_radio.h"
#include "utility.h"
#include "myth_ttffont.h"
#include "utility.h"
#include "readjpg.h"
#include "loadpng.h"
#include "myth_config.h"


extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];
extern char localuserhomedir[4096];                                    // user homedir set in main

extern Font myfont;
extern Font myfont2;

extern int radiooversigt_antal;
extern bool do_zoom_radio_cover;
extern Renderer renderer;
extern bool do_zoom_tidal_cover;
extern bool do_zoom_spotify_cover;
extern bool do_zoom_music_cover;
extern GLuint _textureradioplayer;
extern GLuint onlineradio_empty;
extern GLuint playing_tidal_icon_texture;

extern class config_icons config_menu; // config icons used in menu

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
  const char *database = (char *) "mythtvcontroller";
  char sqlselect[512];
  // mysql vars
  MYSQL *conn;
  MYSQL_RES *res;
  sprintf(sqlselect,"update radio_stations set gfx_link='%s' where intnr=%d",gfxpath,nr);
  conn=mysql_init(NULL);
  // Connect to database
  try {
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
    }
    if (conn) mysql_close(conn);
  }
  catch (...) {
    fprintf(stdout,"Error update radio station db.\n");
    // write_logfile(logfile,(char *) "Error update radio station db.");
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
    while(i<radioantal()) {
      tmpfilename = "/opt/mythtv-controller/images/radiostations/";	// hent path
      gfxfilename = get_station_gfxfile(i);
      tmpfilename = tmpfilename + gfxfilename;			// add filename to path
      if ((gfxfilename.length() > 0) && (file_exists(tmpfilename.c_str()))) {	// den har et navn samt gfx filen findes.
        texture=loadTexture ((char *) tmpfilename.c_str());					// load texture
        set_texture(i,texture);							// save it in radio station struct to show
      } else if (gfxfilename.length() == 0) {
        // check hvis ikke noget navn om der findes en fil med radio station navnet *.png/jpg
        // hvis der gør load denne fil.
        tmpfilename = "/opt/mythtv-controller/images/radiostations/";
        tmpfilename = tmpfilename + get_station_name(i);
        tmpfilename = tmpfilename + ".png";      
        if (file_exists(tmpfilename.c_str())) {		// den har et navn samt gfx filen findes.
          texture=loadTexture ((char *) tmpfilename.c_str());                                 // load texture
          set_texture(i,texture);                         		            // save it in radio station struct
          stack[i].gfxfilename=(char *) get_station_name(i);      // update station gfxfilename to station name
          stack[i].gfxfilename = stack[i].gfxfilename + ".png";
          opdatere_radiostation_gfx(stack[i].intnr,(char *) stack[i].gfxfilename.c_str());           // and update db filename
        } else {
          tmpfilename = "/opt/mythtv-controller/images/radiostations/";
          tmpfilename = tmpfilename + get_station_name(i);
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
    // write_logfile(logfile,(char *) "Error loading radio station graphic icons.");
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
  // write_logfile(logfile,(char *) "Update played radio station.");
  sprintf(sqlselect,"update radio_stations set popular=popular+1,lastplayed=now() where intnr=%ld",stack[stationid].intnr);  
  conn=mysql_init(NULL);
  // Connect to database
  if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0)) {
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    mysql_close(conn);
    return(1);
  } else {
    // write_logfile(logfile,(char *) "Error update radiostation lastplayed in db.");
  }
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
  // write_logfile(logfile,(char *) "Update played radio station online.");
  if (onoff) sprintf(sqlselect,"update radio_stations set online=1 where intnr=%ld",stack[stationid].intnr);
    else sprintf(sqlselect,"update radio_stations set online=0 where intnr=%ld",stack[stationid].intnr);
  conn=mysql_init(NULL);
  // Connect to database
  if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0)) {
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
  stack.at(stationid).online=onoff;
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
    if (mysql_real_connect(conn, configmysqlhost, configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0)) {
      mysql_query(conn,"set NAMES 'utf8'");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      mysql_close(conn);
      return(1);
    } else return(0);		// we are done check all radio stations in database
  }
  catch (...) {
    fprintf(stdout,"Error connect to mysql db.\n");
    // write_logfile(logfile,(char *) "Error connect to mysql radio station db.");
  }
  return(0);
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
    if (cfgfile.is_open()) {
      cfgfile >> cfg_root;
    } else {
      std::cout << "Could not open json file!\n";
      return 0;
    }
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
            } else {
              // new_radio_record.gfxfilename="";
              new_radio_record.gfxfilename=downloadfilenamelong2;
            }
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
    cfgfile.close();
    std::cout << std::endl;
    std::cout << "Done parsing json file." << std::endl;
    mysql_close(conn1);
  }
  if (antal>0) return(antal); else return(0);
}


// ****************************************************************************************
//
// OVERLOAD opdatere_radio_oversigt
//
// ****************************************************************************************

int radiostation_class::opdatere_radio_oversigt() {
  const char *database = (char *) "mythtvcontroller";  
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
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
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
      load_radio_stations_gfx();
      return(antal-1);
    } else {
      fprintf(stderr,"\nFailed to update radiodb, can not connect to database: mythtvcontroller Error: %s\n",mysql_error(conn));
    //  exit(0);
    }
    if (conn) mysql_close(conn);
  }
  catch (...) {
    fprintf(stdout,"Error connect to radio station db.\n");
    // write_logfile(logfile,(char *) "Error connect to mysql radio station db.");
  }
  return(0);
}


// ****************************************************************************************
//
// search radio station in db after searchtxt
// OVERLOADED
//
// ****************************************************************************************

int radiostation_class::opdatere_radio_oversigt(char *searchtxt) {
    const char *database = (char *) "mythtvcontroller";  
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
      if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
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
      // write_logfile(logfile,(char *) "Error connect to mysql radio station db.");
    }
    return(0);
}



// ****************************************************************************************
//
// Opdatere liste efter sort order (radiosortorder)
// OVERLOADED
//
// ****************************************************************************************

int radiostation_class::opdatere_radio_oversigt(int radiosortorder) {
    const char *database = (char *) "mythtvcontroller";  
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
    bool do_sqlite=false;
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
      if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
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
        load_radio_stations_gfx();
        return(antal-1);
      } else {
        fprintf(stderr,"Failed to update radiodb, can not connect to database: mythtvcontroller Error: %s\n",mysql_error(conn));
      }
      if (conn) mysql_close(conn);
    }
    catch (...) {
      fprintf(stdout,"Error connect to mysql radio station db.\n");
      // write_logfile(logfile,(char *) "Error connect to mysql radio station db.");
    }    
    return(0);
}



// *************************************************************************************
//
// draw cover
//
// *************************************************************************************

void radiostation_class::drawcover(int x, int y, int w, int h, GLuint textureId,int id) {
  renderer.AddTextureRect(id,onlineradio_empty, x, y, w, h,1,1,1,1);
  renderer.AddTextureRect(id,textureId,x+5, y+5, w-10, h-10,1,1,1,1);
  if ((id-99)==playingstationnr) {
    renderer.AddTextureRect(id,playing_tidal_icon_texture, x+w-40, y+h-40, 40, 40,1,1,1,1);
  }
};

// *************************************************************************************
//
// draw all items for radio
//
// *************************************************************************************


void radiostation_class::draw_radio_item(int x, int y,int ii,GLuint normal_icon,GLuint empty_icon, int stream_key_selected) {
  std::string temprgtxt;
  GLuint texture;
  int yof_top=850;
  int xof_top=250;
  int buttonsizex=170;
  int buttonsizey=170;
  int totalRows   = (int)ceil((float)stack.size() / itemsPerRow);
  int visibleRows = viewHeight / rowHeight;
  int visibleItems = (visibleRows + 2) * itemsPerRow;
  std::string gfxfilename = stack[ii].gfxfilename;
  if (gfxfilename.size() > 0) {
    // load texture if not loaded before
    if ((stack[ii].textureId == 0) && (stack[ii].gfx_loaded==false)) {
      if (file_exists(gfxfilename.c_str())) {
        stack[ii].textureId = loadTexture((char *) gfxfilename.c_str());
        stack[ii].gfx_loaded=true;
        printf("Loading texture \n");
      }
    }
  }
  if (stack[ii].textureId)
    texture=stack[ii].textureId;
  else 
    texture=normal_icon;
  temprgtxt = fmt::format("{:^20}",stack[ii].station_name);
  if (ii == selected_icon_in_view) {
    drawcover(x+20, y+20, buttonsizex+10, buttonsizey+10, texture,ii+100);
    renderer.AddText(&myfont,x + 20,y+(buttonsizey+30) ,temprgtxt,1,1,1,1);
  } else {
    drawcover(x+20, y+20, buttonsizex, buttonsizey, texture,ii+100);
    renderer.AddText(&myfont,x + 20 ,y+(buttonsizey+40) ,temprgtxt,1,1,1,1);
  }
}


// *********************************************************************************************
//
// Show radio overview
//
// *********************************************************************************************


bool radiostation_class::show_radio_oversigt(GLuint normal_icon,GLuint normal_icon_mask,GLuint back_icon,GLuint dirplaylist_icon,int _mangley) {
  int buttonsizex=164;  
  int buttonsizey=164;  
  // ---- KINETIC SCROLL ---------------------------------------
  scrollVel *= friction;
  scrollPos += scrollVel;
  if (fabs(scrollVel) < 0.01f) scrollVel = 0;
  int totalRows   = (int)ceil((float)stack.size() / itemsPerRow);
  int visibleRows = viewHeight / rowHeight;
  float maxScroll = std::max(0.0f, (float)(totalRows - visibleRows) * rowHeight);
  scrollPos = std::clamp(scrollPos, 0.0f, maxScroll);
  // ---- CALC --------------------------------------------------
  int firstRow   = (int)(scrollPos / rowHeight);
  float subOff   = fmod(scrollPos, rowHeight);
  int sofset     = firstRow * itemsPerRow;
  int screenTop = startY;
  int screenTop_search = startY_search_view;
  int xof = startX;
  int visibleItems = (visibleRows + 2) * itemsPerRow;
  int yof=0;
  int searchtype=0; // 0 album, 1 artist, 2 track
  int yof_top=950;
  int xof_top=250;
  int ii=0;
  std::string gfxfilename;
  for (int i = 0; i < visibleItems && (sofset+i) < stack.size();i++) {
    int index = sofset + i;
    int col = i % itemsPerRow;
    int row = i / itemsPerRow;
    int x = startX + col * itemWidth + 40;
    int y = startY + row * rowHeight - subOff - 20;
    draw_radio_item(x, y, index , normal_icon, normal_icon, playingstationnr);
  }
  if (stack.empty()) {
    renderer.AddTextureRect(0,normal_icon, 400, 400, 800, 200,1,1,1,1);
  }
  return(1);
}