#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>
#include <mysql.h>
#include <GL/glc.h>
#include <pthread.h>                   // multi thread support
#include <libxml/parser.h>
#include <sys/stat.h>
#include <time.h>

#include "myctrl_stream.h"
#include "utility.h"
#include "myth_ttffont.h"
#include "utility.h"
#include "readjpg.h"
#include "loadpng.h"
// web file loader
#include "myctrl_readwebfile.h"

//
// text render is glcRenderString for freetype font support
//

extern float configdefaultstreamfontsize;
extern int tema;
extern char *dbname;                                           // internal database name in mysql (music,movie,radio)
extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];
extern int configmythtvver;
extern int screen_size;
extern int screensizey;
extern int screeny;
                                                                // debug mode
                                                                // 1  = wifi net
                                                                // 2  = music
                                                                // 4  = stream
                                                                // 8  = keyboard/mouse move
                                                                // 16 = movie
                                                                // 32 = searcg
extern int debugmode;                                           // 64 = radio station land icon loader
                                                                // 128= stream search
                                                                // 256 = tv program stuf
                                                                // 512 = media importer
                                                                // 1024 = flag loader
extern unsigned int musicoversigt_antal;                        //
extern int do_stream_icon_anim_icon_ofset;                      //
extern GLuint radiooptions,radiooptionsmask;			              //
extern GLuint _textureIdback;  					                        // back icon
extern GLuint newstuf_icon;                                     //
extern int fonttype;
extern fontctrl aktivfont;
extern int orgwinsizey,orgwinsizex;
extern int _sangley;
extern GLuint _textureIdloading,_textureIdloading1;
//extern GLuint _textureIdloading_mask;


// stream mask
extern GLuint onlinestreammask;
extern GLuint onlinestreammaskicon;		// icon mask on web icon
extern stream_class streamoversigt;
extern GLint cur_avail_mem_kb;
extern bool stream_loadergfx_started;
extern bool stream_loadergfx_started_done;
extern bool stream_loadergfx_started_break;


// constructor
stream_class::stream_class() : antal(0) {
    int i;
    for(i=0;i<maxantal;i++) stack[i]=0;
    stream_optionselect=0;							// selected line in stream options
    stream_oversigt_loaded=false;
    stream_oversigt_loaded_nr=0;
    antal=0;
    gfx_loaded=false;			      // gfx loaded
    stream_is_playing=false;    // is we playing any media
    stream_is_pause=false;      // is player on pause
}


// destructor
stream_class::~stream_class() {
    clean_stream_oversigt();
}


// return the name
char *stream_class::get_stream_name(int nr) {
  if (nr<antal) return (stack[nr]->feed_name); else return (NULL);
}


// return the description
char *stream_class::get_stream_desc(int nr) {
  if (nr<antal) return (stack[nr]->feed_desc); else return (NULL);
}

// clean up number of created

void stream_class::clean_stream_oversigt() {
    startup_loaded=false;				// set radio station loaded in
    for(int i=1;i<antal;i++) {
        if (stack[i]->textureId) glDeleteTextures(1, &stack[i]->textureId);	// delete stream texture
        //if (stack[i]) delete stack[i];							// delete radio station
    }
    antal=0;
    stream_oversigt_loaded=false;			// set load icon texture again
    stream_oversigt_loaded_nr=0;
    stream_oversigt_nowloading=0;
}


// set en stream icon image
void stream_class::set_texture(int nr,GLuint idtexture) {
    stack[nr]->textureId=idtexture;
}

//
// vlc player interface
//

// default player
// stop playing stream sound or video

void stream_class::stopstream() {
  if ((vlc_in_playing()) && (stream_is_playing)) vlc_controller::stopmedia();
  stream_is_playing=false;
}

// ****************************************************************************************
//
// vlc stop player
//
// ****************************************************************************************

void stream_class::softstopstream() {
  if ((vlc_in_playing()) && (stream_is_playing)) vlc_controller::stopmedia();
  stream_is_playing=false;
}


// get length on stream
//
// ****************************************************************************************

unsigned long stream_class::get_length_in_ms() {
  vlc_controller::get_length_in_ms();
}


// jump in player
//
// ****************************************************************************************

float stream_class::jump_position(float ofset) {
    ofset=vlc_controller::jump_position(ofset);
    return(ofset);
}

// to play streams from web
//vlc_m = libvlc_media_new_location(vlc_inst, "http://www.ukaff.ac.uk/movies/cluster.avi");


// pause stream
//
// ****************************************************************************************

int stream_class::pausestream(int pause) {
    //stream_is_playing=true;
    vlc_controller::pause(1);
    if (!(stream_is_pause)) stream_is_pause=true; else stream_is_pause=false;
    return(1);
}



// ****************************************************************************************
// start playing movie by vlclib
//
// ****************************************************************************************

int stream_class::playstream(int nr) {
    char path[PATH_MAX];                                  // max path length from os
    strcpy(path,"");
    strcat(path,get_stream_url(nr));
    stream_is_playing=true;
    vlc_controller::playmedia(path);
    return(1);
}

// ****************************************************************************************
//
// ****************************************************************************************

int stream_class::playstream_url(char *path) {
    stream_is_playing=true;
    vlc_controller::playwebmedia(path);
    return(1);
}

// ****************************************************************************************
//
// ****************************************************************************************

float stream_class::getstream_pos() {
    return(vlc_controller::get_position());
}


// ****************************************************************************************
// update nr of view on podcast
//
// ****************************************************************************************

void stream_class::update_rss_nr_of_view(char *url) {
  // mysql vars
  char sqlinsert[32768];
  MYSQL *conn;
  MYSQL_RES *res,*res1;
  MYSQL_ROW row;
  char *database = (char *) "mythtvcontroller";
  conn=mysql_init(NULL);
  // get homedir
  try {
    if (conn) {
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      sprintf(sqlinsert,"update mythtvcontroller.internetcontentarticles set time=time+1 where mediaURL like '%s'",url);
      mysql_query(conn,sqlinsert);
      res = mysql_store_result(conn);
      mysql_free_result(res);
      mysql_close(conn);
    }
  }
  catch (...) {
    printf("Error open mysql connection.\n");
  }
}


// ****************************************************************************************
//
// used to download rss file from web to db info (url is flag for master rss file (mediaURL IS NULL))
// in db if mediaURL have url this is the rss feed loaded from rss file
// updaterssfile bool is do it now (u key in overview)
//
// ****************************************************************************************


int stream_class::loadrssfile(bool updaterssfile) {
  bool haveupdated=false;
  char sqlselect[2048];
  char sqlinsert[32768];
  char totalurl[2048];
  char parsefilename[2048];
  char homedir[2048];
  char baseicon[2048];
  unsigned int recantal;
  MYSQL *conn;
  MYSQL_RES *res,*res1;
  MYSQL_ROW row;
  time_t timenow;
  char *database = (char *) "mythtvcontroller";
  struct stat attr;
  const int updateinterval=86400;
  time(&timenow);
  conn=mysql_init(NULL);
  // get homedir
  getuserhomedir(homedir);
  strcat(homedir,"/rss");
  if (!(file_exists(homedir))) mkdir(homedir,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  strcat(homedir,"/images");
  if (!(file_exists(homedir))) mkdir(homedir,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  // Connect to database
  //strcpy(sqlselect,"select internetcontent.name,internetcontentarticles.path,internetcontentarticles.title,internetcontentarticles.description,internetcontentarticles.url,internetcontent.thumbnail,count(internetcontentarticles.feedtitle),internetcontent.thumbnail from internetcontentarticles left join internetcontent on internetcontentarticles.feedtitle=internetcontent.name group by internetcontentarticles.feedtitle");
  //strcpy(sqlselect,"select * from internetcontentarticles");
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    strcpy(sqlselect,"select count(mediaURL) from internetcontentarticles where mediaURL IS NULL");
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        recantal=atoi(row[0]);
      }
      mysql_free_result(res);
    }
    stream_rssparse_nowloading=0;
    strcpy(sqlselect,"select * from internetcontentarticles where mediaURL is NULL");
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    // go to all record have the url in xml files to download
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        stream_rssparse_nowloading++;
        //if (debugmode & 4) printf("Get rss file on stream title %10s \n",row[0]);
        if ((row[3]) && (strcmp(row[3],"")!=0)) {
          getuserhomedir(homedir);                                          // get user homedir
          strcpy(totalurl,"wget -U Netscape --timeout=10 '");
          if (row[7]) strcat(totalurl,row[7]); else if (row[3]) strcat(totalurl,row[3]);
          strcat(totalurl,"' -o '");
          strcat(totalurl,homedir);                                         // add user homedir
          strcat(totalurl,"/rss/wget.log'");                                // log file
          strcat(totalurl," -O '");
          strcat(totalurl,homedir);                                         // add user homedir
          strcat(totalurl,"/rss/");                                         // add output filename
          if (row[3]) strcat(totalurl,row[3]);
          strcat(totalurl,".rss'");
          strcpy(parsefilename,homedir);                                    // copy user homedir
          strcat(parsefilename,"/rss/");
          strcat(parsefilename,row[3]);
          strcat(parsefilename,".rss");
          strcpy(baseicon,"");
          stat(parsefilename, &attr);
          if ((file_exists(parsefilename)) && (attr.st_mtime+updateinterval<timenow)) {
            // download rss file
            system(totalurl);
            // parse file
            strcpy(parsefilename,homedir);
            strcat(parsefilename,"/rss/");
            strcat(parsefilename,row[3]);
            strcat(parsefilename,".rss");
            // if podcast is rss
            // if title ok and not podcast bud real rss feed
            if ((strcmp(row[3],"")!=0) && (!(row[23]))) {
              // parse downloaded xmlfile now (create db records)
              parsexmlrssfile(parsefilename,baseicon);
            }
          } else if ((!(file_exists(parsefilename))) || (updaterssfile)) {
            // download rss file
            system(totalurl);
            // parse file
            strcpy(parsefilename,homedir);
            strcat(parsefilename,"/rss/");
            strcat(parsefilename,row[3]);
            strcat(parsefilename,".rss");
            // if podcast is rss
            // if title ok and not podcast bud real rss feed
            if ((strcmp(row[3],"")!=0) && (!(row[23]))) {
              // parse downloaded xmlfile now (create db records)
              // and get base image from funccall (baseicon (url to image))
              parsexmlrssfile(parsefilename,baseicon);
            } else {
              printf("XML FILE is missing/not working on %s file.\n",row[3]);
            }
          }
          // update master icon if none
          if ((strcmp(row[0],"")!=0) && (strcmp(baseicon,"")!=0)) {
            sprintf(sqlinsert,"UPDATE internetcontentarticles set paththumb='%s' where feedtitle like '%s' and paththumb IS NULL",baseicon,row[0]);
            mysql_query(conn,sqlinsert);
            res1 = mysql_store_result(conn);
            haveupdated=true;
          }
          // if podcast is not rss and title ok
          if ((strcmp(row[3],"")!=0) && (row[23])) {
            if (atoi(row[23])==1) {
              sprintf(sqlinsert,"UPDATE internetcontentarticles set mediaURL=url where podcast=1 and feedtitle like '%s'",row[0]);
              mysql_query(conn,sqlinsert);
              res1 = mysql_store_result(conn);
              haveupdated=true;
            }
          }
        }
      }
      mysql_free_result(res);
      stream_rssparse_nowloading=0;
    }
    mysql_close(conn);
  } else return(-1);
  if (haveupdated) return(1); else return(0);
}


// ****************************************************************************************
//
// ****************************************************************************************


char search_and_replace2(char *text) {
  int n=0;
  int nn=0;
  char newtext[2048];
  strcpy(newtext,"");
  while(n<strlen(text)) {
    if (text[n]=='"') {
      strcat(newtext,"'");
      n++;
    } else {
      newtext[nn]=text[n];
      newtext[nn+1]='\0';                               // null terminate string
      nn++;
      n++;
    }
  }
  newtext[n]=0;
  strcpy(text,newtext);
}


// ****************************************************************************************
//
// xml parser
//
// ****************************************************************************************


int stream_class::parsexmlrssfile(char *filename,char *baseiconfile) {
  xmlChar *tmpdat;
  xmlDoc *document;
  xmlNode *root, *first_child, *node, *node1 ,*subnode,*subnode2,*subnode3;
  xmlChar *xmlrssid;
  xmlChar *content;
  char rssprgtitle[2048];
  char rssprgfeedtitle[2048];
  char rssprgdesc[2048];
  char rssprgimage[2048];
  char rssprgimage1[2048];
  char rssvideolink[2048];
  char rssprgpubdate[256];
  char rsstime[2048];
  char rssauthor[2048];
  char rssduration[2048];
  int rssepisode;
  int rssseason;
  char result[2048+1];
  char sqlinsert[32768];
  char sqlselect[32768];
  char *database = (char *) "mythtvcontroller";
  bool recordexist=false;
  time_t raw_tid;
  struct tm *opret_dato;
  char rssopretdato[200];
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  strcpy(rssprgtitle,"");
  strcpy(rssprgfeedtitle,"");
  strcpy(rssprgdesc,"");
  strcpy(rssvideolink,"");
  strcpy(rsstime,"");
  strcpy(rssauthor,"");
  strcpy(rssprgimage,"");
  strcpy(rssprgimage1,"");
  if (check_zerro_bytes_file(filename)>0) {
    conn=mysql_init(NULL);
    document = xmlReadFile(filename, NULL, 0);            // open xml file
    // if exist do all the parse and update db
    // it use REPLACE in mysql to create/update records if changed in xmlfile
    raw_tid=time(NULL);
    opret_dato=localtime(&raw_tid);
    strftime(rssopretdato,28,"%F %T",opret_dato);
    if ((document) && (conn)) {
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      if (conn) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
      }
      root = xmlDocGetRootElement(document);
      first_child = root->children;
      for (node = first_child; node; node = node->next) {
        if (node->type==XML_ELEMENT_NODE) {
          if ((strcmp((char *) node->name,"channel")==0) || (strcmp((char *) node->name,"feed")==0)) {
            content = xmlNodeGetContent(node);
            if (content) {
              //strcpy(result,(char *) content);
            }
            subnode=node->xmlChildrenNode;
            while(subnode) {
              xmlrssid=xmlGetProp(subnode,( xmlChar *) "title");
              content = xmlNodeGetContent(subnode);
              if ((content) && (strcmp((char *) subnode->name,"title")==0)) {
                content = xmlNodeGetContent(subnode);
                strcpy(rssprgtitle,(char *) content);
              }
              // images
              // rssprgimage
              // paththumb
              //
              if ((content) && (strcmp((char *) subnode->name,"image")==0)) {
                content = xmlNodeGetContent(subnode);
                tmpdat=xmlGetProp(subnode,( xmlChar *) "href");
                if (tmpdat) {
                  strcpy(rssprgimage,(char *) tmpdat);
                  strcpy(baseiconfile,(char *) tmpdat);
                  xmlFree(tmpdat);
                }
              }
              if ((content) && (strcmp((char *) subnode->name,"thumbnail")==0)) {
                content = xmlNodeGetContent(subnode);
                tmpdat=xmlGetProp(subnode,( xmlChar *) "href");
                if (tmpdat) {
                  strcpy(rssprgimage,(char *) tmpdat);
                  xmlFree(tmpdat);
                }
              }
              // item type element
              if ((content) && (strcmp((char *) subnode->name,"item")==0)) {
                subnode2=subnode->xmlChildrenNode;
                while(subnode2) {
                  if ((content) && (strcmp((char *) subnode2->name,"title")==0)) {
                    content = xmlNodeGetContent(subnode2);
                    strcpy(rssprgfeedtitle,(char *) content);
                  }
                  // get play url
                  if ((content) && (strcmp((char *) subnode2->name,"enclosure")==0)) {
                    content = xmlNodeGetContent(subnode2);
                    tmpdat=xmlGetProp(subnode2,( xmlChar *) "url");
                    if (tmpdat) {
                      strcpy(rssvideolink,(char *) tmpdat);
                      xmlFree(tmpdat);
                    }
                  }
                  // rssprgpubdate
                  if ((content) && (strcmp((char *) subnode2->name,"pubDate")==0)) {
                    content = xmlNodeGetContent(subnode2);
                    strcpy(rssprgpubdate,(char *) content);
                  }
                  // get length
                  if ((content) && (strcmp((char *) subnode2->name,"duration")==0)) {
                    content = xmlNodeGetContent(subnode2);
                    if (content) strcpy(rsstime,(char *) content);
                  }
                  // get episode
                  if ((content) && (strcmp((char *) subnode2->name,"episode")==0)) {
                    content = xmlNodeGetContent(subnode2);
                    if (content) rssepisode=atoi((char *) content);
                  }
                  // get season
                  if ((content) && (strcmp((char *) subnode2->name,"season")==0)) {
                    content = xmlNodeGetContent(subnode2);
                    if (content) rssseason=atoi((char *) content);
                  }
                  // get author
                  if ((content) && (strcmp((char *) subnode2->name,"author")==0)) {
                    content = xmlNodeGetContent(subnode2);
                    if (content) strcpy(rssauthor,(char *) content);
                  }
                  // get description
                  if ((content) && (strcmp((char *) subnode2->name,"description")==0)) {
                    content = xmlNodeGetContent(subnode2);
                    if (content) {
                      xmlrssid=xmlGetProp(subnode2,( xmlChar *) "description");
                      if (xmlrssid) strcpy(rssprgdesc,(char *) content);
                      xmlFree(xmlrssid);
                    }
                  }
                  subnode2=subnode2->next;
                }
                recordexist=false;
                // check if record exist
                if (strcmp(rssvideolink,"")!=0) {
                  sprintf(sqlinsert,"select feedtitle from internetcontentarticles where (feedtitle like '%s' and mediaURL like '%s')",rssprgtitle,rssvideolink);
                  mysql_query(conn,sqlinsert);
                  res = mysql_store_result(conn);
                  if (res) {
                    while ((row = mysql_fetch_row(res)) != NULL) {
                      recordexist=true;
                    }
                  }
                  // creoate record if not exist
                  if (!(recordexist)) {
                    search_and_replace2(rssprgtitle);
                    search_and_replace2(rssprgfeedtitle);
                    search_and_replace2(rssprgdesc);
                    if (debugmode & 4) printf("\t Update title %-20s Date %s\n",rssprgtitle,rssprgpubdate);
                    sprintf(sqlinsert,"REPLACE into internetcontentarticles(feedtitle,mediaURL,title,episode,season,author,path,description,paththumb,date,time) values(\"%s\",'%s',\"%s\",%d,%d,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d)",rssprgtitle,rssvideolink,rssprgfeedtitle,rssepisode,rssseason,rssauthor,"",rssprgdesc,rssprgimage,rssopretdato,0);
                    if (mysql_query(conn,sqlinsert)!=0) {
                      printf("mysql REPLACE table error. %s\n",sqlinsert);
                    }
                    res = mysql_store_result(conn);
                  }
                }
              }
              subnode=subnode->next;
            }
            // end while loop
          }
          // youtube type
          // get title
          if (strcmp((char *) node->name,"title")==0) {
            strcpy(rssprgtitle,"");
            content = xmlNodeGetContent(node);
            if (content) strcpy(rssprgtitle,(char *) content);
          }
          if ((strcmp((char *) node->name,"link")==0) || (strcmp((char *) node->name,"entry")==0)) {
            // reset for earch element loading
            strcpy(rssvideolink,"");
            strcpy(rssprgfeedtitle,"");
            rssepisode=0;
            rssseason=0;
            strcpy(rssauthor,"");
            strcpy(rssprgdesc,"");
            strcpy(rssprgimage1,"");
            strcpy(rssprgimage,"");
            if (strcmp((char *) node->name,"entry")==0) {
              subnode2=node->xmlChildrenNode;
              while(subnode2) {
                strcpy(rssprgimage,"");
                if ((content) && (strcmp((char *) subnode2->name,"title")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  strcpy(rssprgfeedtitle,(char *) content);
                }
                // get play url
                if ((content) && (strcmp((char *) subnode2->name,"link")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  tmpdat=xmlGetProp(subnode2,( xmlChar *) "href");
                  if (tmpdat) {
                    strcpy(rssvideolink,(char *) tmpdat);
                    xmlFree(tmpdat);
                  }
                }
                if ((content) && (strcmp((char *) subnode2->name,"group")==0)) {
                  subnode3=subnode2->xmlChildrenNode;
                  while(subnode3) {
                    if ((content) && (strcmp((char *) subnode2->name,"title")==0)) {
                      content = xmlNodeGetContent(subnode2);
                      strcpy(rssprgtitle,(char *) content);
                    }
                    if ((content) && (strcmp((char *) subnode2->name,"description")==0)) {
                      content = xmlNodeGetContent(subnode2);
                      strcpy(rssprgdesc,(char *) content);
                    }
                    // get icon gfx
                    if ((content) && (strcmp((char *) subnode3->name,"thumbnail")==0)) {
                      content = xmlNodeGetContent(subnode3);
                      tmpdat=xmlGetProp(subnode3,( xmlChar *) "url");
                      if (tmpdat) {
                        //if (debugmode & 4) printf("Get image url %s \n",tmpdat);
                        strcpy(rssprgimage1,(char *) tmpdat);
                        xmlFree(tmpdat);
                      }
                    }
                    subnode3=subnode3->next;
                  }
                }
                subnode2=subnode2->next;
              }
              recordexist=false;
              sprintf(sqlinsert,"SELECT feedtitle from internetcontentarticles where (feedtitle like '%s' mediaURL like '%s' and title like '%s')",rssprgtitle,rssvideolink,rssprgfeedtitle);
              mysql_query(conn,sqlinsert);
              res = mysql_store_result(conn);
              if (res) {
                while ((row = mysql_fetch_row(res)) != NULL) {
                  recordexist=true;
                }
              }
              if (!(recordexist)) {
                search_and_replace2(rssprgtitle);
                search_and_replace2(rssprgfeedtitle);
                search_and_replace2(rssprgdesc);
                if (debugmode & 4) printf("\t Update title %-20s Date %s\n",rssprgtitle,rssprgpubdate);
                sprintf(sqlinsert,"REPLACE into internetcontentarticles(feedtitle,mediaURL,title,episode,season,author,path,description,paththumb,date,time) values(\"%s\",'%s',\"%s\",%d,%d,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d)",rssprgtitle,rssvideolink,rssprgfeedtitle,rssepisode,rssseason,rssauthor,"",rssprgdesc,rssprgimage1,rssopretdato,0);
                //sprintf(sqlinsert,"REPLACE into internetcontentarticles(feedtitle,mediaURL,title,episode,season,author,path,description,paththumb) values('%s','%s','%s',%d,%d,'%s','%s','%s','%s')",rssprgtitle,rssvideolink,rssprgfeedtitle,rssepisode,rssseason,rssauthor,"",rssprgdesc,rssprgimage1);
                if (mysql_query(conn,sqlinsert)!=0) {
                  printf("mysql REPLACE table error. %s\n",sqlinsert);
                }
                res = mysql_store_result(conn);
              }
            }
          }
        }
      }
      xmlFreeDoc(document);
      mysql_close(conn);
    }
  } else {
    if (debugmode & 4) printf("Read error on %s xmlfile downloaded to rss dir \n",filename);
  }
  return(1);
}





// ****************************************************************************************
//
// get nr of rss feed
//
// ****************************************************************************************


int stream_class::get_antal_rss_feeds_sources(MYSQL *conn) {
  char sqlselect[4096];
  MYSQL_RES *res;
  MYSQL_ROW row;
  int antal=0;
  //printf("*************************************************************************************\n");
  if (conn) {
    sprintf(sqlselect,"SELECT count(name) from mythtvcontroller.internetcontent");
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        antal=atoi(row[0]);
      }
    }
  }
  return(antal);
}




// ****************************************************************************************
//
// get antal podcast af type
//
// ****************************************************************************************

int get_podcasttype_antal(char *typedata) {
  char sqlselect[4096];
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char *database = (char *) "mythtvcontroller";
  int nrofrec=0;
  if (typedata) {
    sprintf(sqlselect,"SELECT count(title) from internetcontentarticles where feedtitle like '%s'",typedata);
    conn=mysql_init(NULL);
    if (conn) {
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      if (mysql_query(conn,sqlselect)!=0) {
        printf("mysql select count error. on title %s\n",typedata);
      }
      res = mysql_store_result(conn);
      while ((res) && (row = mysql_fetch_row(res)) != NULL) {
        nrofrec=atoi(row[0]);
      }
      mysql_free_result(res);
      mysql_close(conn);
    }
  }
  return(nrofrec);
}






// ****************************************************************************************
//
// check if title exist
//
// ****************************************************************************************

int check_rss_feed_exist(MYSQL *conn,char *rssname) {
  bool recexist=false;
  char sqlselect[2048];
  MYSQL_RES *res;
  MYSQL_ROW row;
  if (conn) {
    sprintf(sqlselect,"SELECT feedtitle from mythtvcontroller.internetcontentarticles where feedtitle like '%s' limit 1",rssname);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        recexist=true;
      }
    }
  }
  return(recexist);
}



// ****************************************************************************************
//
// opdate show liste in view (The view)
//
// load felt 7 = mythtv gfx icon
// fpath=stream path
// atr = stream name
//
// ****************************************************************************************


int stream_class::opdatere_stream_oversigt(char *art,char *fpath) {
    char sqlselect[2048];
    char tmpfilename[1024];
    char lasttmpfilename[1024];
    char downloadfilename[1024];
    char downloadfilename1[1024];
    char downloadfilenamelong[1024];
    char homedir[1024];
    // mysql vars
    bool rss_update=false;
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *database = (char *) "mythtvcontroller";
    bool online;
    int getart=0;
    bool loadstatus=true;
    bool dbexist=false;
    antal=0;
    conn=mysql_init(NULL);
    // Connect to database
    if (conn) {
      if (mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0)==0) {
        dbexist=false;
      }
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      // test about rss table exist
      mysql_query(conn,"SELECT feedtitle from mythtvcontroller.internetcontentarticles limit 1");
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          dbexist=true;
        }
      }
      //
      // create db if not exist
      // and dump some default rss feeed in
      //
      if (!(dbexist)) {
        printf("Creating/Update RSS/PODCAST for new rss feed\n");
        // thumbnail   = name of an local image file
        // commandline = Program to fetch content with
        // updated     = Time of last update
        if ((!(dbexist)) && (mysql_query(conn,"CREATE database if not exist mythtvcontroller")!=0)) {
          printf("mysql db create error.\n");
        }
        res = mysql_store_result(conn);
        // create db
        sprintf(sqlselect,"CREATE TABLE IF NOT EXISTS mythtvcontroller.internetcontentarticles(feedtitle varchar(255),path text,paththumb text,title varchar(255),season smallint(5) DEFAULT 0,episode smallint(5) DEFAULT 0,description text,url text,type smallint(3),thumbnail text,mediaURL text,author varchar(255),date datetime,time int(11),rating varchar(255),filesize bigint(20),player varchar(255),playerargs text,download varchar(255),downloadargs text,width smallint(6),height smallint(6),language varchar(128),podcast tinyint(1),downloadable tinyint(1),customhtml tinyint(1),countries varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=60 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql create table error.\n");
          printf("SQL : %s\n",sqlselect);
        }
        res = mysql_store_result(conn);
        // create db
        sprintf(sqlselect,"CREATE TABLE IF NOT EXISTS mythtvcontroller.internetcontent(name varchar(255),thumbnail varchar(255),type smallint(3),author varchar(128),description text,commandline text,version double,updated datetime,search tinyint(1),tree tinyint(1),podcast tinyint(1),download tinyint(1),host varchar(128),id int NOT NULL AUTO_INCREMENT PRIMARY KEY,INDEX Idx (name (15),thumbnail (15),type, author (15),  description (15),commandline (15),version,updated,search ,tree,podcast,download,host (15))) ENGINE=MyISAM AUTO_INCREMENT=60 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql create table error.\n");
          printf("SQL : %s\n",sqlselect);
        }
        if (!(dbexist)) {
          // create index
          sprintf(sqlselect,"CREATE INDEX `internetcontentarticles_feedtitle`  ON `mythtvcontroller`.`internetcontentarticles` (feedtitle) COMMENT '' ALGORITHM DEFAULT LOCK DEFAULT");
          if (mysql_query(conn,sqlselect)!=0) {
            printf("mysql create index error.\n");
          }
        }
      }
      res = mysql_store_result(conn);
      // ok Aftenshowet
      // dr have removed the rss feed
      if (check_rss_feed_exist(conn,(char *) "Aftenshowet")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Aftenshowet',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Aftenshowet.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Aftenshowet',NULL,NULL,'Aftenshowet',0,0,NULL,'https://www.dr.dk/mu/Feed/aftenshowet-9.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Aftenshowet.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // ok Anders Lund Madsen
      if (check_rss_feed_exist(conn,(char *) "Anders Lund Madsen i Den Yderste By")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Anders Lund Madsen i Den Yderste By',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Anders Lund Madsen.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Anders Lund Madsen i Den Yderste By',NULL,NULL,'Anders lund massen i den udereste by',0,0,NULL,'http://www.dr.dk/mu/Feed/anders-lund-madsen-i-den-yderste-by.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Anders Lund Madsen i Den Yderste By.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // ok AK 24syv
      if (check_rss_feed_exist(conn,(char *) "AK 24syv")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('AK 24syv',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error AK 24syv.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('AK 24syv',NULL,NULL,'AK 24syv',0,0,NULL,'https://arkiv.radio24syv.dk/audiopodcast/channel/3843145',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error AK 24syv.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Best of YouTube
      if (check_rss_feed_exist(conn,(char *) "Best of YouTube (video)")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Best of YouTube (video)',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Best of YouTube.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Best of YouTube (video)',NULL,NULL,'Best of YouTube (video)',0,0,NULL,'http://feeds.feedburner.com/boyt',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Best of YouTube (video).\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Bonderøven
      if (check_rss_feed_exist(conn,(char *) "Bonderøven")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Bonderøven',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Bonderøven.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Bonderøven',NULL,NULL,'Bonderøven',0,0,NULL,'https://www.dr.dk/mu/Feed/bonderoven-alle.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Bonderøven.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Bonderøven.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // virker ikke
      if (check_rss_feed_exist(conn,(char *) "CNET")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('CNET',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error CNET.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('CNET',NULL,NULL,'CNET',0,0,NULL,'http://feed.cnet.com/feed/podcast/all/hd.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error CNET.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Droner og kanoner
      if (check_rss_feed_exist(conn,(char *) "Droner og kanoner")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Droner og kanoner',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Droner og kanoner\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Droner og kanoner',NULL,NULL,'Droner og kanoner',0,0,NULL,'https://www.dr.dk/mu/Feed/droner-og-kanoner.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Droner og kanoner.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok icon wrong
      if (check_rss_feed_exist(conn,(char *) "Dynamo")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Dynamo',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Dynamo.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Dynamo',NULL,NULL,'Dynamo',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UC7SDsqJba5428-EOBZWOn3w',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Dynamo.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok So ein Ding
      if (check_rss_feed_exist(conn,(char *) "So ein Ding")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('So ein Ding',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error So ein Ding.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('So ein Ding',NULL,NULL,'So ein Ding',0,0,NULL,'https://www.dr.dk/mu/Feed/so-ein-ding?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error So ein Ding.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Verdens Bedste Filmklub
      if (check_rss_feed_exist(conn,(char *) "Verdens Bedste Filmklub")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Verdens Bedste Filmklub',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Verdens Bedste Filmklub.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Verdens Bedste Filmklub',NULL,NULL,'Verdens Bedste Filmklub',0,0,NULL,'http://lytbar.dk/wordpress/?feed=rss2&cat=7',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Verdens Bedste Filmklub.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok I seng med NOVA
      if (check_rss_feed_exist(conn,(char *) "I seng med NOVA")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('I seng med NOVA',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error I seng med NOVA.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('I seng med NOVA',NULL,NULL,'I seng med NOVA',0,0,NULL,'https://tools.planetradio.co.uk/core/podcasts/rss.php?name=i-seng-med-nova',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error I seng med NOVA.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Krager og Drager - Dansk Game of Thrones Podcast
      if (check_rss_feed_exist(conn,(char *) "Krager og Drager - Dansk Game of Thrones Podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Krager og Drager - Dansk Game of Thrones Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Krager og Drager.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Krager og Drager - Dansk Game of Thrones Podcast',NULL,NULL,'Krager og Drager - Dansk Game of Thrones Podcast',0,0,NULL,'http://www.kragerogdrager.dk/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Krager og Drager - Dansk Game of Thrones Podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok The Verge
      if (check_rss_feed_exist(conn,(char *) "The Verge")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('The Verge',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error The Verge.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Verge',NULL,NULL,'The Verge',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCddiUEpeqJcYeBxX1IVBKvQ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error The Verge.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Tech Talk Today
      if (check_rss_feed_exist(conn,(char *) "Tech Talk Today")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Tech Talk Today',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Tech Talk Today.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Tech Talk Today',NULL,NULL,'Tech Talk Today',0,0,NULL,'http://techtalk.today/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Tech Talk Today.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Kontant
      if (check_rss_feed_exist(conn,(char *) "Kontant")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Kontant',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Kontant.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Kontant',NULL,NULL,'Kontant',0,0,NULL,'https://www.dr.dk/mu/Feed/kontant-2.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Kontant.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Movieclips Coming Soon
      if (check_rss_feed_exist(conn,(char *) "Movieclips Coming Soon")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Movieclips Coming Soon',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Movieclips Coming Soon.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Movieclips Coming Soon',NULL,NULL,'Movieclips Coming Soon',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCkR0GY0ue02aMyM-oxwgg9g',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Movieclips Coming Soon.\n");
        rss_update=true;
      }
      // ok The Rocket Ranch
      if (check_rss_feed_exist(conn,(char *) "The Rocket Ranch")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('The Rocket Ranch',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error The Rocket Ranch.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Rocket Ranch',NULL,NULL,'The Rocket Ranch',0,0,NULL,'https://www.nasa.gov/rss/dyn/Rocket-Ranch.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error The Rocket Ranch.\n");
        rss_update=true;
      }
      // ok Troldspejlet Podcast
      if (check_rss_feed_exist(conn,(char *) "Troldspejlet Podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Troldspejlet Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Troldspejlet Podcast.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Troldspejlet Podcast',NULL,NULL,'Troldspejlet Podcast',0,0,NULL,'https://www.dr.dk/mu/feed/troldspejlet-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Troldspejlet Podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // virker ikke (youtube)
      if (check_rss_feed_exist(conn,(char *) "GameSpot")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('GameSpot',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error GameSpot.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('GameSpot',NULL,NULL,'GameSpot',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCbu2SsF-Or3Rsn3NxqODImw',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error GameSpot.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok The Story from The Guardian
      if (check_rss_feed_exist(conn,(char *) "The Story from The Guardian")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('The Story from The Guardian',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error The Story from The Guardian.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Story from The Guardian',NULL,NULL,'The Story from The Guardian',0,0,NULL,'https://www.theguardian.com/news/series/the-story/podcast.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error The Story from The Guardian.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error The Story from The Guardian.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok DEN GRÅ SIDE
      if (check_rss_feed_exist(conn,(char *) "DEN GRÅ SIDE")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('DEN GRÅ SIDE',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('DEN GRÅ SIDE',NULL,NULL,'DEN GRÅ SIDE',0,0,NULL,'http://www.spreaker.com/show/2108328/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error DEN GRÅ SIDE.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok TechSNAP
      if (check_rss_feed_exist(conn,(char *) "TechSNAP")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('TechSNAP',NULL,1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error TechSNAP.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('TechSNAP',NULL,NULL,'TechSNAP',0,0,NULL,'http://techsnap.systems/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error TechSNAP.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Bitch Sesh: A Real Housewives Breakdown
      if (check_rss_feed_exist(conn,(char *) "Bitch Sesh: A Real Housewives Breakdown")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Bitch Sesh: A Real Housewives Breakdown',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Bitch Sesh.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Bitch Sesh: A Real Housewives Breakdown',NULL,NULL,'Bitch Sesh: A Real Housewives Breakdown',0,0,NULL,'http://rss.earwolf.com/bitch-sesh',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Bitch Sesh: A Real Housewives Breakdown.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok 2 Dope Queens
      if (check_rss_feed_exist(conn,(char *) "2 Dope Queens")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('2 Dope Queens',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error 2 Dope Queens.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('2 Dope Queens',NULL,NULL,'2 Dope Queens',0,0,NULL,'http://feeds.wnyc.org/2dopequeens',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error 2 Dope Queens.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Ramajetterne
      if (check_rss_feed_exist(conn,(char *) "Ramajetterne")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Ramajetterne',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Ramajetterne.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Ramajetterne',NULL,NULL,'Ramajetterne',0,0,NULL,'https://www.dr.dk/mu/Feed/ramajetterne-2.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Ramajetterne.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok De Sorte Spejdere - podcast
      if (check_rss_feed_exist(conn,(char *) "De Sorte Spejdere - podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('De Sorte Spejdere - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error De Sorte Spejdere.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('De Sorte Spejdere - podcast',NULL,NULL,'De Sorte Spejdere - podcast',0,0,NULL,'http://www.dr.dk/mu/Feed/de-sorte-spejdere-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error De Sorte Spejdere - podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Nak & æd
      if (check_rss_feed_exist(conn,(char *) "Nak & æd")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Nak & æd',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Nak & æd.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Nak & æd',NULL,NULL,'Nak & æd',0,0,NULL,'http://www.dr.dk/mu/Feed/nak-og-aed-alle.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Nak & æd.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Ubådssagen
      if (check_rss_feed_exist(conn,(char *) "Ubådssagen")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Ubådssagen',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Ubådssagen.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Ubådssagen',NULL,NULL,'Ubådssagen',0,0,NULL,'http://www.dr.dk/mu/Feed/ubadssagen.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Ubådssagen.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Videnskabens Verden
      if (check_rss_feed_exist(conn,(char *) "Videnskabens Verden")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Videnskabens Verden',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Videnskabens Verden.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles(feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Videnskabens Verden',NULL,NULL,'Ubådssagen',0,0,NULL,'http://www.dr.dk/mu/Feed/videnskabens-verden.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Videnskabens Verden.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Videnskabens Verden Brainstorm
      if (check_rss_feed_exist(conn,(char *) "Brainstorm")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Brainstorm',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Videnskabens Verden.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles(feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Brainstorm',NULL,NULL,'Brainstorm',0,0,NULL,'https://audioboom.com/channels/4985564.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Videnskabens Verden.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Gametest Fan Uploads
      if (check_rss_feed_exist(conn,(char *) "Gametest Fan Uploads")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Gametest Fan Uploads',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Gametest Fan Uploads.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Gametest Fan Uploads',NULL,NULL,'Gametest Fan Uploads',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCuX8EWTbH--tyha5FUoBSrg',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Gametest Fan Uploads.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Gravity Assist
      if (check_rss_feed_exist(conn,(char *) "Gravity Assist")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Gravity Assist',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Gravity Assist.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Gravity Assist',NULL,NULL,'Gravity Assist',0,0,NULL,'https://www.nasa.gov/rss/dyn/Gravity-Assist.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Gravity Assist.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok TED
      if (check_rss_feed_exist(conn,(char *) "TED")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('TED',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error TED.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('TED',NULL,NULL,'TED',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCAuUUnT6oDeKwE6v1NGQxug',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error TED.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Hva så?! med Christian Fuhlendorff
      if (check_rss_feed_exist(conn,(char *) "Hva så?! med Christian Fuhlendorff")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Hva så?! med Christian Fuhlendorff',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Hva så?! med Christian Fuhlendorff.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Hva så?! med Christian Fuhlendorff',NULL,NULL,'Hva så?! med Christian Fuhlendorff',0,0,NULL,'http://hva-saa.podomatic.com/rss2.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Hva så?! med Christian Fuhlendorff.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Mads & Monopolet
      if (check_rss_feed_exist(conn,(char *) "Mads & Monopolet - podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Mads & Monopolet - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Mads & Monopolet.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Mads & Monopolet - podcast',NULL,NULL,'Mads & Monopolet - podcast',0,0,NULL,'https://www.dr.dk/mu/feed/mads-monopolet-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Mads & Monopolet - podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Det røde rum
      if (check_rss_feed_exist(conn,(char *) "Det røde rum")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Det røde rum',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Det røde rum.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Det røde rum',NULL,NULL,'Det røde rum',0,0,NULL,'https://www.dr.dk/mu/Feed/det-roede-rum-radio.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Det røde rum.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Den Korte Weekendavis
      if (check_rss_feed_exist(conn,(char *) "Den Korte Weekendavis")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Den Korte Weekendavis',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Den Korte Weekendavis.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Den Korte Weekendavis',NULL,NULL,'Den Korte Weekendavis',0,0,NULL,'http://arkiv.radio24syv.dk/audiopodcast/channel/10839671',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Den Korte Weekendavis.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // virker ikke
      // title skifter i rss file
      if (check_rss_feed_exist(conn,(char *) "Radio24syv Nyheder")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Radio24syv Nyheder',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Radio24syv Nyheder.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Radio24syv Nyheder',NULL,NULL,'Radio24syv Nyheder',0,0,NULL,'http://arkiv.radio24syv.dk/audiopodcast/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Radio24syv Nyheder.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Selvsving
      if (check_rss_feed_exist(conn,(char *) "Selvsving")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Selvsving',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Selvsving.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Selvsving',NULL,NULL,'Selvsving',0,0,NULL,'https://www.dr.dk/mu/Feed/selvsving?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Selvsving.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Lille Nørd
      if (check_rss_feed_exist(conn,(char *) "Lille Nørd")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Lille Nørd',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Lille Nørd.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Lille Nørd',NULL,NULL,'Lille Nørd',0,0,NULL,'https://www.dr.dk/mu/Feed/lille-noerd?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Lille Nørd.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Sofus
      if (check_rss_feed_exist(conn,(char *) "Sofus")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Sofus',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Sofus.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Sofus',NULL,NULL,'Sofus',0,0,NULL,'https://www.dr.dk/mu/Feed/sofus-2.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Sofus.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok CBS This Morning
      if (check_rss_feed_exist(conn,(char *) "CBS This Morning")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('CBS This Morning',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error CBS This Morning.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('CBS This Morning',NULL,NULL,'CBS This Morning',0,0,NULL,'http://cbsradionewsfeed.com/rss.php?id=149&ud=12',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error CBS This Morning.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok CBS Evening News
      if (check_rss_feed_exist(conn,(char *) "CBS Evening News -- Full Audio")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('CBS Evening News -- Full Audio',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error CBS Evening News.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('CBS Evening News -- Full Audio',NULL,NULL,'CBS Evening News -- Full Audio',0,0,NULL,'http://cbsradionewsfeed.com/rss.php?id=126&ud=12',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error CBS Evening News -- Full Audio.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // virker ikke
      if (check_rss_feed_exist(conn,(char *) "Vergecast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Vergecast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Vergecast.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Vergecast',NULL,NULL,'Vergecast',0,0,NULL,'http://feeds.feedburner.com/ThisIsMyNextPodcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Vergecast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Elektronista
      if (check_rss_feed_exist(conn,(char *) "Elektronista")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Elektronista',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Elektronista.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Elektronista',NULL,NULL,'Elektronista',0,0,NULL,'https://arkiv.radio24syv.dk/audiopodcast/channel/3843152',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Elektronista.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Linux Action News
      if (check_rss_feed_exist(conn,(char *) "Linux Action News")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Linux Action News',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Linux Action News.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Linux Action News',NULL,NULL,'Linux Action News',0,0,NULL,'http://linuxactionnews.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Linux Action News.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // virker ikke
      if (check_rss_feed_exist(conn,(char *) "Linux Action Show")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Linux Action Show',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Linux Action Show.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Linux Action Show',NULL,NULL,'Linux Action Show',0,0,NULL,'http://feeds.feedburner.com/linuxashd?format=xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Linux Action Show.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // virker ikke
      if (check_rss_feed_exist(conn,(char *) "GNU World Order")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('GNU World Order',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error GNU World Order.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('GNU World Order',NULL,NULL,'GNU World Order',0,0,NULL,'http://gnuworldorder.info/ogg.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error GNU World Order.\n");
          printf("SQL: %s\n",sqlselect);
        }
        res = mysql_store_result(conn);
        mysql_free_result(res);
        rss_update=true;
      }
      // ok The Command Line Podcast
      if (check_rss_feed_exist(conn,(char *) "The Command Line Podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('The Command Line Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Command Line Podcast',NULL,NULL,'The Command Line Podcast',0,0,NULL,'https://thecommandline.net/cmdln_free',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error The Command Line Podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Her går det godt
      if (check_rss_feed_exist(conn,(char *) "Her går det godt")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Her går det godt',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Her går det godt.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Her går det godt',NULL,NULL,'Her går det godt',0,0,NULL,'http://www.spreaker.com/show/2093919/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Her går det godt.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // virker ikke The RC Newb Podcast
      if (check_rss_feed_exist(conn,(char *) "The RC Newb Podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('he RC Newb Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error The RC Newb Podcast.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The RC Newb Podcast',NULL,NULL,'The RC Newb Podcast',0,0,NULL,'https://rcnewb.com/feed/podcast/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error The RC Newb Podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok P7 MIX Maraton - podcast
      if (check_rss_feed_exist(conn,(char *) "P7 MIX Maraton - podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('P7 MIX Maraton - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error P7 MIX Maraton.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('P7 MIX Maraton - podcast',NULL,NULL,'P7 MIX Maraton',0,0,NULL,'https://www.dr.dk/mu/feed/p7-maraton.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error P7 MIX Maraton - podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok going linux
      if (check_rss_feed_exist(conn,(char *) "going linux")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('going linux',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error going linux.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('going linux',NULL,NULL,'going linux',0,0,NULL,'http://goinglinux.com/mp3podcast.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error going linux.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Fire forførende radiofortællinger
      if (check_rss_feed_exist(conn,(char *) "Fire forførende radiofortællinger")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Fire forførende radiofortællinger',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Fire forførende radiofortællinger.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Fire forførende radiofortællinger',NULL,NULL,'Fire forførende radiofortællinger',0,0,NULL,'https://www.dr.dk/mu/feed/fire-forfoerende-radiofortaellinger.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Fire forførende radiofortællinger.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Videnskab.dk Podcast
      if (check_rss_feed_exist(conn,(char *) "Videnskab.dk Podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Videnskab.dk Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Videnskab.dk Podcast.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Videnskab.dk Podcast',NULL,NULL,'Videnskab.dk Podcast',0,0,NULL,'http://feeds.soundcloud.com/users/soundcloud:users:4378703/sounds.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Videnskab.dk Podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Den nye stil
      if (check_rss_feed_exist(conn,(char *) "Den nye stil - historien om dansk rap")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Den nye stil - historien om dansk rap',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Den nye stil.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Den nye stil - historien om dansk rap',NULL,NULL,'Historien-om-dansk-rap',0,0,NULL,'https://www.dr.dk/mu/feed/den-nye-stil-historien-om-dansk-rap.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Den nye stil - historien om dansk rap.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // virker ikke
      if (check_rss_feed_exist(conn,(char *) "Giant Gnome Productions")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Giant Gnome Productions',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Giant Gnome Productions.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Giant Gnome Productions',NULL,NULL,'Giant Gnome Productions',0,0,NULL,'https://giantgnome.com/feed/podcast/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Giant Gnome Productions.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok META
      if (check_rss_feed_exist(conn,(char *) "META")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('META',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error META.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('META',NULL,NULL,'META',0,0,NULL,'https://www.spreaker.com/show/2663274/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error META.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Voksen ABC podcast
      if (check_rss_feed_exist(conn,(char *) "Voksen ABC podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Voksen ABC podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Voksen ABC podcast.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Voksen ABC podcast',NULL,NULL,'Voksen ABC podcast',0,0,NULL,'http://voksenabc.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Voksen ABC podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Third Ear
      if (check_rss_feed_exist(conn,(char *) "Third Ear")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Third Ear',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Third Ear.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Third Ear',NULL,NULL,'Third Ear',0,0,NULL,'https://thirdear.podbean.com/feed.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Third Ear.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Sex Nerd Sandra
      if (check_rss_feed_exist(conn,(char *) "Sex Nerd Sandra")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Sex Nerd Sandra',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Sex Nerd Sandra.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Sex Nerd Sandra',NULL,NULL,'Sex Nerd Sandra',0,0,NULL,'http://sexnerdsandra.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Sex Nerd Sandra.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Skru op for sex
      if (check_rss_feed_exist(conn,(char *) "Skru op for sex")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Skru op for sex',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Skru op for sex.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Skru op for sex',NULL,NULL,'Skru op for sex',0,0,NULL,'https://www.spreaker.com/show/3006683/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Skru op for sex.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Techsistens
      if (check_rss_feed_exist(conn,(char *) "Techsistens")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Techsistens',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Techsistens.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Techsistens',NULL,NULL,'Techsistens',0,0,NULL,'http://feeds.soundcloud.com/users/soundcloud:users:56822074/sounds.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Techsistens.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok beginner audiophile
      if (check_rss_feed_exist(conn,(char *) "beginner audiophile | hifi | gear reviews | stereo | hi-end audio")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('beginner audiophile | hifi | gear reviews | stereo | hi-end audio',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error beginner audiophile.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('beginner audiophile | hifi | gear reviews | stereo | hi-end audio',NULL,NULL,'beginner audiophile | hifi | gear reviews | stereo | hi-end audio',0,0,NULL,'http://beginneraudiophile.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error beginner audiophile.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // What Hi-Fi
      if (check_rss_feed_exist(conn,(char *) "What Hi-Fi? Podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('What Hi-Fi? Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error What Hi-Fi.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('What Hi-Fi? Podcast',NULL,NULL,'What Hi-Fi? Podcast',0,0,NULL,'http://feeds.soundcloud.com/users/soundcloud:users:425082507/sounds.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error What Hi-Fi? Podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // NASACast Video
      if (check_rss_feed_exist(conn,(char *) "NASACast Video")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('NASACast Video',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error NASACast.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('NASACast Video',NULL,NULL,'NASACast Video',0,0,NULL,'https://www.nasa.gov/rss/dyn/NASAcast_vodcast.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error NASACast Video.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // Inside Dynaudio
      if (check_rss_feed_exist(conn,(char *) "Inside Dynaudio")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Inside Dynaudio',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Inside Dynaudio.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Inside Dynaudio',NULL,NULL,'Inside Dynaudio',0,0,NULL,'https://www.nasa.gov/rss/dyn/NASAcast_vodcast.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Inside Dynaudio.\n");
          printf("SQL: %s\n",sqlselect);
        }
        res = mysql_store_result(conn);
        mysql_free_result(res);
        rss_update=true;
      }
      // need check
      if (check_rss_feed_exist(conn,(char *) "Converge with Casey Newton")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Converge with Casey Newton',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Hacker News Nation.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Converge with Casey Newton',NULL,NULL,'Converge with Casey Newton',0,0,NULL,'http://feeds.feedburner.com/ConvergeWithCaseyNewton',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Hacker News Nation (Video).\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok TED Talk daily
      if (check_rss_feed_exist(conn,(char *) "TED Talks Daily")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('TED Talks Daily',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('TED Talks Daily',NULL,NULL,'TED Talks Daily ',0,0,NULL,'http://feeds.feedburner.com/TEDTalks_audio',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Hacker News Nation (Video).\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok RISE podcast
      if (check_rss_feed_exist(conn,(char *) "RISE podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('RISE podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error. %s \n","RISE podcast");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('RISE podcast',NULL,NULL,'RISE podcast ',0,0,NULL,'http://feeds.soundcloud.com/users/soundcloud:users:300950004/sounds.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Hacker News Nation (Video).\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok StarTalk Radio
      if (check_rss_feed_exist(conn,(char *) "StarTalk Radio")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('StarTalk Radio',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error. StarTalk Radio");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('StarTalk Radio',NULL,NULL,'StarTalk Radio ',0,0,NULL,'https://rss.art19.com/startalk-radio',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error StarTalk Radio.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // ok Stuff You Should Know
      if (check_rss_feed_exist(conn,(char *) "Stuff You Should Know")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Stuff You Should Know',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error. %s \n","Stuff You Should Know");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Stuff You Should Know',NULL,NULL,'Stuff You Should Know',0,0,NULL,'https://feeds.megaphone.fm/stuffyoushouldknow',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Hacker News Nation (Video).\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // do not work
      // GeekBeat.TV
      if (check_rss_feed_exist(conn,(char *) "GeekBeat.TV")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('GeekBeat.TV',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error. %s \n","GeekBeat.TV");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('GeekBeat.TV',NULL,NULL,'GeekBeat.TV',0,0,NULL,'http://feeds.feedburner.com/caliextralarge?format=xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error GeekBeat.TV.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // OK tilbage til mars dr1 podcast
      if (check_rss_feed_exist(conn,(char *) "Tilbage til Mars")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Tilbage til Mars',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Tilbage til Mars.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Tilbage til Mars',NULL,NULL,'Tilbage til Mars',0,0,NULL,'https://www.dr.dk/mu/feed/tilbage-til-mars.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Tilbage til Mars.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // OK Love + Radio
      if (check_rss_feed_exist(conn,(char *) "Love + Radio")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Love + Radio',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Love + Radio.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Love + Radio',NULL,NULL,'Love + Radio',0,0,NULL,'http://feed.loveandradio.org/loveplusradio',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Love + Radio.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // OK Barometeret
      if (check_rss_feed_exist(conn,(char *) "Barometeret")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Barometeret',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Barometeret.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Barometeret',NULL,NULL,'Barometeret',0,0,NULL,'https://www.dr.dk/mu/Feed/barometeret.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Barometeret.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      //  Et langsomt mord
      if (check_rss_feed_exist(conn,(char *) "Et langsomt mord")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Et langsomt mord',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Et langsomt mord.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Et langsomt mord',NULL,NULL,'Et langsomt mord',0,0,NULL,'https://www.dr.dk/mu/feed/et-stille-mord.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Et langsomt mord.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // Virker ikke Swingercast
      if (check_rss_feed_exist(conn,(char *) "Swingercast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Swingercast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Swingercast.");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Swingercast',NULL,NULL,'Swingercast',0,0,NULL,'http://feeds.feedburner.com/Swingercast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Swingercast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // OK FONK! Det er podcast
      if (check_rss_feed_exist(conn,(char *) "FONK! Det er podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('FONK! Det er podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Tilbage til FONK! Det er podcast.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('FONK! Det er podcast',NULL,NULL,'FONK! Det er podcast',0,0,NULL,'https://www.dr.dk/mu/feed/fonk-det-er-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Tilbage til FONK! Det er podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // OK Filmland
      if (check_rss_feed_exist(conn,(char *) "Filmland")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Filmland',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Tilbage til Filmland.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Filmland',NULL,NULL,'Filmland',0,0,NULL,'https://www.dr.dk/mu/feed/filmland.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Tilbage til Filmland.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // Do not work Radio Information
      if (check_rss_feed_exist(conn,(char *) "Radio Information")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Radio Information',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Radio Information.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Radio Information',NULL,NULL,'Radio Information',0,0,NULL,'https://radioinformation.podbean.com/feed.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Radio Information.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // OK kino.dk podcast
      if (check_rss_feed_exist(conn,(char *) "kino.dk podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('kino.dk podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error kino.dk podcast.");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('kino.dk podcast',NULL,NULL,'kino.dk podcast',0,0,NULL,'http://kinodk.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error kino.dk podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        res = mysql_store_result(conn);
        mysql_free_result(res);
        rss_update=true;
      }
      // OK tændt på p3
      if (check_rss_feed_exist(conn,(char *) "Tændt")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Tændt',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error tændt på p3.");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Tændt',NULL,NULL,'Tændt',0,0,NULL,'https://www.dr.dk/mu/feed/taendt.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error tændt på p3.\n");
          printf("SQL: %s\n",sqlselect);
        }
        res = mysql_store_result(conn);
        mysql_free_result(res);
        rss_update=true;
      }
      // OK Sound of Cinema
      if (check_rss_feed_exist(conn,(char *) "Sound of Cinema")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Sound of Cinema',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Sound of Cinema.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Sound of Cinema',NULL,NULL,'Sound of Cinema',0,0,NULL,'http://podcasts.files.bbci.co.uk/b03bzf92.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Sound of Cinema.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // OK P1 podcaster
      if (check_rss_feed_exist(conn,(char *) "P1 podcaster")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('P1 podcaster',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Tilbage til Mars.");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('P1 podcaster',NULL,NULL,'P1 podcaster',0,0,NULL,'https://www.dr.dk/mu/feed/p1-podcaster.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Tilbage til Mars.\n");
          printf("SQL: %s\n",sqlselect);
        }
        res = mysql_store_result(conn);
        mysql_free_result(res);
        rss_update=true;
      }
      // OK Guys We F****d
      if (check_rss_feed_exist(conn,(char *) "Guys We F****d")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Guys We F****d',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert Guys We F****d.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Guys We F****d',NULL,NULL,'Guys We F****d',0,0,NULL,'https://feeds.megaphone.fm/LM9233046886',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Guys We F****d.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // OK bollemusik
      if (check_rss_feed_exist(conn,(char *) "bollemusik")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('bollemusik',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert bollemusik.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('bollemusik',NULL,NULL,'bollemusik',0,0,NULL,'https://heartbeats.dk/feed/podcast/bollemusik',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error bollemusik.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // OK danskerbongo
      if (check_rss_feed_exist(conn,(char *) "Danskerbingo")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Danskerbingo',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert Danskerbingo.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Danskerbingo',NULL,NULL,'Danskerbingo',0,0,NULL,'https://www.dr.dk/mu/feed/danskerbingo-radio.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Danskerbingo.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // Uafbrudt Samleje &#8211; Heartbeats.dk
      if (check_rss_feed_exist(conn,(char *) "Uafbrudt Samleje &#8211; Heartbeats.dk")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Uafbrudt Samleje &#8211; Heartbeats.dk',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert Uafbrudt Samleje &#8211; Heartbeats.dk.");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Uafbrudt Samleje &#8211; Heartbeats.dk',NULL,NULL,'Uafbrudt Samleje &#8211; Heartbeats.dk',0,0,NULL,'https://heartbeats.dk/series/uafbrudt-samleje/feed/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Tilbage til Uafbrudt Samleje &#8211; Heartbeats.dk.\n");
          printf("SQL: %s\n",sqlselect);
        }
        res = mysql_store_result(conn);
        mysql_free_result(res);
        rss_update=true;
      }
      // OK Mdma
      if (check_rss_feed_exist(conn,(char *) "Alt om Emma - en serie om MDMA")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Alt om Emma - en serie om MDMA',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert Mdma.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Alt om Emma - en serie om MDMA',NULL,NULL,'Alt om Emma - en serie om MDMA',0,0,NULL,'https://www.dr.dk/mu/Feed/alt-om-emma.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Mdma.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // OK Do not work Radio24syv Dokumentar
      if (check_rss_feed_exist(conn,(char *) "24syv Dokumentar")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('24syv Dokumentar',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert Radio24syv Dokumentar.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('24syv Dokumentar',NULL,NULL,'24syv Dokumentar',0,0,NULL,'https://arkiv.radio24syv.dk/audiopodcast/channel/3887302',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Radio24syv Dokumentar.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }
      // Radio24syv Hadprædikanterne
      if (check_rss_feed_exist(conn,(char *) "24syv Hadprædikanterne")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('24syv Hadprædikanterne',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert 24syv Hadprædikanterne.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('24syv Hadprædikanterne',NULL,NULL,'24syv Hadprædikanterne',0,0,NULL,'https://arkiv.radio24syv.dk/audiopodcast/channel/38971368',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error 24syv Hadprædikanterne.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      //  Radio24syv
      if (check_rss_feed_exist(conn,(char *) "24syv Den Korte Radioavis")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('24syv Den Korte Radioavis',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert 24syv Den Korte Radioavis.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('24syv Den Korte Radioavis',NULL,NULL,'24syv Den Korte Radioavis',0,0,NULL,'https://arkiv.radio24syv.dk/audiopodcast/channel/10839671',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error 24syv Den Korte Radioavis.\n");
          printf("SQL: %s\n",sqlselect);
        }
        mysql_free_result(res);
        rss_update=true;
      }

      // OK Radiofortællinger
      if (check_rss_feed_exist(conn,(char *) "Radiofortællinger")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Radiofortællinger',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert Radiofortællinger.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Radiofortællinger',NULL,NULL,'Radiofortællinger',0,0,NULL,'https://www.dr.dk/mu/feed/radiofortaellinger.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Radiofortællinger.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Shitstorm dainsh podcast
      if (check_rss_feed_exist(conn,(char *) "Shitstorm")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Shitstorm',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert Shitstorm.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Shitstorm',NULL,NULL,'Shitstorm',0,0,NULL,'https://www.dr.dk/mu/feed/shitstorm.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Shitstorm.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // podcast
      if (check_rss_feed_exist(conn,(char *) "For Adults Only | Sexy Hot Stories Erotic from the Street")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('For Adults Only | Sexy Hot Stories Erotic from the Street',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert For Adults Only.");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('For Adults Only | Sexy Hot Stories Erotic from the Street',NULL,NULL,'For Adults Only | Sexy Hot Stories Erotic from the Street',0,0,NULL,'http://adultstories.fastforward.libsynpro.com/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error For For Adults Only.\n");
          printf("SQL: %s\n",sqlselect);
        }
        res = mysql_store_result(conn);
        mysql_free_result(res);
        rss_update=true;
      }


      // OK FONK! Det er podcast
      if (check_rss_feed_exist(conn,(char *) "FONK! Det er podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('FONK! Det er podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert FONK! Det er podcast.");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('FONK! Det er podcast',NULL,NULL,'FONK! Det er podcast',0,0,NULL,'https://www.dr.dk/mu/feed/fonk-det-er-podcast.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error For FONK! Det er podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        res = mysql_store_result(conn);
        mysql_free_result(res);
        rss_update=true;
      }

      // OK Security Central
      if (check_rss_feed_exist(conn,(char *) "Security Central")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Security Central',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert Security Central.");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Security Central',NULL,NULL,'Security Central',0,0,NULL,'https://www.spreaker.com/show/3577866/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error For Security Central .\n");
          printf("SQL: %s\n",sqlselect);
        }
        res = mysql_store_result(conn);
        mysql_free_result(res);
        rss_update=true;
      }

      // OK Det, vi taler om
      if (check_rss_feed_exist(conn,(char *) "Det, vi taler om")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Det, vi taler om',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert Det, vi taler om.");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Det, vi taler om',NULL,NULL,'Det, vi taler om',0,0,NULL,'http://arkiv.radio24syv.dk/audiopodcast/channel/10327258',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error For Det, vi taler om.\n");
          printf("SQL: %s\n",sqlselect);
        }
        res = mysql_store_result(conn);
        mysql_free_result(res);
        rss_update=true;
      }

      // OK Stuff You Should Know
      if (check_rss_feed_exist(conn,(char *) "Stuff You Should Know")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Stuff You Should Know',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert Stuff You Should Know.");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Stuff You Should Know',NULL,NULL,'Stuff You Should Know,0,0,NULL,'https://feeds.megaphone.fm/stuffyoushouldknow',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error For Stuff You Should Know.\n");
          printf("SQL: %s\n",sqlselect);
        }
        res = mysql_store_result(conn);
        mysql_free_result(res);
        rss_update=true;
      }


      // No not work Blackout - pigen på billedet
      if (check_rss_feed_exist(conn,(char *) "Blackout - pigen på billedet")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Blackout - pigen på billedet',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert Blackout - pigen på billedet.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Blackout - pigen på billedet',NULL,NULL,'Blackout - pigen på billedet',0,0,NULL,'https://www.dr.dk/mu/feed/blackout-pigen-paa-billedet.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error For Blackout - pigen på billedet.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Virker ikke Hjernen på overarbejde
      if (check_rss_feed_exist(conn,(char *) "Hjernen på overarbejde")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Hjernen på overarbejde',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert Hjernen på overarbejde.");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Hjernen på overarbejde',NULL,NULL,'Hjernen på overarbejde',0,0,NULL,'https://www.spreaker.com/show/3188722/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error For Hjernen på overarbejde.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Østrogenial
      if (check_rss_feed_exist(conn,(char *) "Østrogenial")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Østrogenial',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error insert Østrogenial.");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Østrogenial',NULL,NULL,'Østrogenial',0,0,NULL,'https://www.spreaker.com/show/3484630/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Østrogenial .\n");
          printf("SQL: %s\n",sqlselect);
        }
        res = mysql_store_result(conn);
        mysql_free_result(res);
        rss_update=true;
      }


      // P6 BEAT elsker - podcast
      if (check_rss_feed_exist(conn,(char *) "P6 BEAT elsker - podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('P6 BEAT elsker - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error insert P6 BEAT elsker - podcast.");
        }
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('P6 BEAT elsker - podcast',NULL,NULL,'P6 BEAT elsker - podcast',0,0,NULL,'https://www.dr.dk/mu/feed/p6-beat-elsker.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error P6 BEAT elsker - podcast .\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Wirker ikke Kid-N-Drag Gaming
      if (check_rss_feed_exist(conn,(char *) "Kid-N-Drag Gaming")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Kid-N-Drag Gaming',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Kid-N-Drag Gaming.\n");
        }
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Kid-N-Drag Gaming',NULL,NULL,'Kid-N-Drag Gaming',0,0,NULL,'https://podcast.kidndrag.chat/feed/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Kid-N-Drag Gaming.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Godnathistorien
      if (check_rss_feed_exist(conn,(char *) "Godnathistorien")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Godnathistorien',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Godnathistorien.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Godnathistorien',NULL,NULL,'Godnathistorien',0,0,NULL,'https://rss.podplaystudio.com/1060.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Godnathistorien.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Beslutningen
      if (check_rss_feed_exist(conn,(char *) "Beslutningen")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Beslutningen',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Beslutningen.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Beslutningen',NULL,NULL,'Beslutningen',0,0,NULL,'https://rss.podplaystudio.com/1052.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Beslutningen.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // NextM Podcast
      if (check_rss_feed_exist(conn,(char *) "NextM Podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('NextM Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error NextM Podcast.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('NextM Podcast',NULL,NULL,'NextM Podcast',0,0,NULL,'https://rss.podplaystudio.com/1040.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error NextM Podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Det er bare en overgang
      if (check_rss_feed_exist(conn,(char *) "Det er bare en overgang")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Det er bare en overgang',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Det er bare en overgang.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Det er bare en overgang',NULL,NULL,'Det er bare en overgang',0,0,NULL,'https://rss.podplaystudio.com/1069.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Det er bare en overgang.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Sex om Dagen
      if (check_rss_feed_exist(conn,(char *) "Sex om Dagen")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Sex om Dagen',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Sex om Dagen.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Sex om Dagen',NULL,NULL,'Sex om Dagen',0,0,NULL,'https://rss.podplaystudio.com/1075.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Sex om Dagen.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Varigt vægttab
      if (check_rss_feed_exist(conn,(char *) "Varigt vægttab")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Varigt vægttab',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Varigt vægttab.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Varigt vægttab',NULL,NULL,'Varigt vægttab',0,0,NULL,'https://rss.podplaystudio.com/1006.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Varigt vægttab.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Nattens dronning
      if (check_rss_feed_exist(conn,(char *) "Nattens dronning")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Nattens dronning',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Nattens dronning.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Nattens dronning',NULL,NULL,'Nattens dronning',0,0,NULL,'https://www.dr.dk/mu/feed/nattens-dronning.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Nattens dronning.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // The Heart
      if (check_rss_feed_exist(conn,(char *) "The Heart")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('The Heart',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error The Heart.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Heart',NULL,NULL,'The Heart',0,0,NULL,'http://feeds.theheartradio.org/TheHeartRadio',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error The Heart.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // Curlingklubben
      if (check_rss_feed_exist(conn,(char *) "Curlingklubben")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Curlingklubben',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Curlingklubben.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Curlingklubben',NULL,NULL,'Curlingklubben',0,0,NULL,'https://www.dr.dk/mu/feed/p3-med-christian-og-maria.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Curlingklubben.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // Geek Beat
      if (check_rss_feed_exist(conn,(char *) "Geek Beat")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Geek Beat',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Geek Beat.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Geek Beat',NULL,NULL,'Geek Beat',0,0,NULL,'https://feeds.feedburner.com/geekbeatblog',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Geek Beat.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }



      // The WIRED Podcast
      if (check_rss_feed_exist(conn,(char *) "The WIRED Podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('The WIRED Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error The WIRED Podcast.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The WIRED Podcast',NULL,NULL,'The WIRED Podcast',0,0,NULL,'https://www.wired.co.uk/rss/podcast/wired-podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error The WIRED Podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Ear Hustle
      if (check_rss_feed_exist(conn,(char *) "Ear Hustle")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Ear Hustle',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Ear Hustle.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Ear Hustle',NULL,NULL,'Ear Hustle',0,0,NULL,'http://feeds.earhustlesq.com/earhustlesq',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Ear Hustle.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }



      // War on the Rocks
      if (check_rss_feed_exist(conn,(char *) "War on the Rocks")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('War on the Rocks',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error War on the Rocks.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('War on the Rocks',NULL,NULL,'War on the Rocks',0,0,NULL,'https://warontherocks.com/feed/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error War on the Rocks.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // The Joe Rogan Experience
      if (check_rss_feed_exist(conn,(char *) "The Joe Rogan Experience")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('The Joe Rogan Experience',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error The Joe Rogan Experience.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Joe Rogan Experience',NULL,NULL,'The Joe Rogan Experience',0,0,NULL,'http://joeroganexp.joerogan.libsynpro.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error The Joe Rogan Experience.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // heardit
      if (check_rss_feed_exist(conn,(char *) "heardit")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('heardit',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error heardit.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('heardit',NULL,NULL,'heardit',0,0,NULL,'https://feeds.buzzsprout.com/480832.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error heardit.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Threat Wire
      if (check_rss_feed_exist(conn,(char *) "Threat Wire")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Threat Wire',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Threat Wire.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Threat Wire',NULL,NULL,'Threat Wire',0,0,NULL,'https://feed.podbean.com/shannonmorse/feed.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Threat Wire.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // Replay
      if (check_rss_feed_exist(conn,(char *) "Replay")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Replay',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Replay.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Replay',NULL,NULL,'Replay',0,0,NULL,'https://podcasts.files.bbci.co.uk/p06qbzmj.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Replay.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // Marsfall
      if (check_rss_feed_exist(conn,(char *) "Marsfall")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Marsfall',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Marsfall.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Marsfall',NULL,NULL,'Marsfall',0,0,NULL,'http://marsfallpodcast.com/feed/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Marsfall.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // Do not work PARADOXICA
      if (check_rss_feed_exist(conn,(char *) "PARADOXICA")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('PARADOXICA',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error PARADOXICA.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('PARADOXICA',NULL,NULL,'PARADOXICA',0,0,NULL,'https://arsparadoxica.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error PARADOXICA.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // OK StarTripper!!
      if (check_rss_feed_exist(conn,(char *) "StarTripper!!")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('StarTripper!!',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error StarTripper!!.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('StarTripper!!',NULL,NULL,'StarTripper!!',0,0,NULL,'https://startripper.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error StarTripper!!.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // OK The Big Loop
      if (check_rss_feed_exist(conn,(char *) "The Big Loop")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('The Big Loop',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error The Big Loop.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Big Loop',NULL,NULL,'The Big Loop',0,0,NULL,'https://thebiglooppodcast.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error The Big Loop.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // OK Janus Descending
      if (check_rss_feed_exist(conn,(char *) "Janus Descending")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Janus Descending',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Janus Descending.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Janus Descending',NULL,NULL,'Janus Descending',0,0,NULL,'https://janusdescending.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Janus Descending.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // OK Wolf 359
      if (check_rss_feed_exist(conn,(char *) "Wolf 359")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Wolf 359',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Wolf 359.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Wolf 359',NULL,NULL,'Wolf 359',0,0,NULL,'https://wolf359radio.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Wolf 359.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // OK The First Stop For Security News
      if (check_rss_feed_exist(conn,(char *) "The First Stop For Security News")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('The First Stop For Security News',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error The First Stop For Security News.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The First Stop For Security News',NULL,NULL,'The First Stop For Security News',0,0,NULL,'https://threatpost.com/category/podcasts/feed/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error  The First Stop For Security News.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // OK Threat Wire
      if (check_rss_feed_exist(conn,(char *) "Threat Wire")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Threat Wire',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Threat Wire.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Threat Wire',NULL,NULL,'Threat Wire',0,0,NULL,'https://feed.podbean.com/shannonmorse/feed.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Threat Wire.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Reply All
      if (check_rss_feed_exist(conn,(char *) "Reply All")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Reply All',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Reply All.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Reply All',NULL,NULL,'Reply All',0,0,NULL,'https://feeds.megaphone.fm/replyall',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Reply All\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // This Week in Linux
      if (check_rss_feed_exist(conn,(char *) "This Week in Linux")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('This Week in Linux',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error This Week in Linux.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('This Week in Linux',NULL,NULL,'This Week in Linux',0,0,NULL,'https://tuxdigital.com/feed/thisweekinlinux-mp3',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error This Week in Linux\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Destination Linux
      if (check_rss_feed_exist(conn,(char *) "Destination Linux")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Destination Linux',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Destination Linux.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Destination Linux',NULL,NULL,'Destination Linux',0,0,NULL,'https://destinationlinux.org/feed/mp3/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Destination Linux\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Digitalt
      if (check_rss_feed_exist(conn,(char *) "Digitalt")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Digitalt',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Digitalt.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Digitalt',NULL,NULL,'Digitalt',0,0,NULL,'https://www.dr.dk/mu/feed/digitalt.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Digitalt\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Jordforbindelsen der forsvandt
      if (check_rss_feed_exist(conn,(char *) "Jordforbindelsen der forsvandt")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Jordforbindelsen der forsvandt',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Jordforbindelsen der forsvandt.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Jordforbindelsen der forsvandt',NULL,NULL,'Jordforbindelsen der forsvandt',0,0,NULL,'https://www.dr.dk/mu/feed/jordforbindelsen-der-forsvandt.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Jordforbindelsen der forsvandt\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // I virkeligheden
      if (check_rss_feed_exist(conn,(char *) "I virkeligheden")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('I virkeligheden',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error I virkeligheden.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('I virkeligheden',NULL,NULL,'I virkeligheden',0,0,NULL,'https://www.dr.dk/mu/feed/i-virkeligheden-radio.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error  I virkeligheden.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Jazzklubben
      if (check_rss_feed_exist(conn,(char *) "Jazzklubben")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Jazzklubben',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Jazzklubben.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Jazzklubben',NULL,NULL,'Jazzklubben',0,0,NULL,'https://www.dr.dk/mu/feed/jazzklubben.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error  Jazzklubben.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Line Kirsten Giftekniv
      if (check_rss_feed_exist(conn,(char *) "Line Kirsten Giftekniv")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Line Kirsten Giftekniv',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Line Kirsten Giftekniv.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Line Kirsten Giftekniv',NULL,NULL,'Line Kirsten Giftekniv',0,0,NULL,'https://www.dr.dk/mu/feed/line-kirsten-giftekniv.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error  Line Kirsten Giftekniv.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // Mod det uendelige univers
      if (check_rss_feed_exist(conn,(char *) "Mod det uendelige univers")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Mod det uendelige univers',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Line Kirsten Giftekniv.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Mod det uendelige univers',NULL,NULL,'Mod det uendelige univers',0,0,NULL,'https://www.dr.dk/mu/feed/mod-det-uendelige-univers.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Mod det uendelige univers.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Min yndlingsmusik
      if (check_rss_feed_exist(conn,(char *) "Min yndlingsmusik")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Min yndlingsmusik',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Line Kirsten Giftekniv.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Min yndlingsmusik',NULL,NULL,'Min yndlingsmusik',0,0,NULL,'https://www.dr.dk/mu/feed/min-yndlingsmusik.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Min yndlingsmusik.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // Verdens bedste lyttere på date
      if (check_rss_feed_exist(conn,(char *) "Verdens bedste lyttere på date")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Verdens bedste lyttere på date',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Line Kirsten Giftekniv.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Verdens bedste lyttere på date',NULL,NULL,'Verdens bedste lyttere på date',0,0,NULL,'https://www.dr.dk/mu/feed/verdens-bedste-lyttere-pa-date-podcast.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Verdens bedste lyttere på date.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Wikibeats - podcast
      if (check_rss_feed_exist(conn,(char *) "Wikibeats - podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Wikibeats - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Wikibeats - podcast.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Wikibeats - podcast',NULL,NULL,'Wikibeats - podcast',0,0,NULL,'https://www.dr.dk/mu/feed/wikibeats-podcast.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Wikibeats - podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      //Absolute Absolute Music
      if (check_rss_feed_exist(conn,(char *) "Absolute Absolute Music")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Absolute Absolute Music',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Absolute Absolute Music.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Absolute Absolute Music',NULL,NULL,'Absolute Absolute Music',0,0,NULL,'https://www.dr.dk/mu/feed/absolute-absolute-music.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Absolute Absolute Music.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // The Infinite Monkey Cage
      if (check_rss_feed_exist(conn,(char *) "The Infinite Monkey Cage")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('The Infinite Monkey Cage',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error The Infinite Monkey Cage.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Infinite Monkey Cage',NULL,NULL,'The Infinite Monkey Cage',0,0,NULL,'https://podcasts.files.bbci.co.uk/b00snr0w.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error The Infinite Monkey Cage.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Putting Science to Work
      if (check_rss_feed_exist(conn,(char *) "Putting Science to Work")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Putting Science to Work',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Putting Science to Work.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Putting Science to Work',NULL,NULL,'Putting Science to Work',0,0,NULL,'https://podcasts.files.bbci.co.uk/b06t1y3z.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Putting Science to Work.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // 13 Minutes to the Moon
      if (check_rss_feed_exist(conn,(char *) "13 Minutes to the Moon")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('13 Minutes to the Moon',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error 13 Minutes to the Moon.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('13 Minutes to the Moon',NULL,NULL,'13 Minutes to the Moon',0,0,NULL,'https://podcasts.files.bbci.co.uk/w13xttx2.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error 13 Minutes to the Moon.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // Jazz Library
      if (check_rss_feed_exist(conn,(char *) "Jazz Library")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Jazz Library',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error  Jazz Library.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Jazz Library',NULL,NULL,'Jazz Library',0,0,NULL,'https://podcasts.files.bbci.co.uk/b006x41z.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error  Jazz Library.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Det Tomme Magasin
      if (check_rss_feed_exist(conn,(char *) "Det Tomme Magasin")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Det Tomme Magasin',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Det Tomme Magasin.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Det Tomme Magasin',NULL,NULL,'Det Tomme Magasin',0,0,NULL,'https://www.dr.dk/mu/feed/det-tomme-magasin.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Det Tomme Magasin.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // Sonic Erotica.com - Free Erotic Audio &#187; Podcast Feed
      if (check_rss_feed_exist(conn,(char *) "Sonic Erotica.com &#8211; Free Erotic Audio &#187; Podcast Feed")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Sonic Erotica.com &#8211; Free Erotic Audio &#187; Podcast Feed',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Sonic Erotica.com &#8211; Free Erotic Audio &#187; Podcast Feed.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Sonic Erotica.com &#8211; Free Erotic Audio Podcast Feed',NULL,NULL,'Sonic Erotica.com &#8211; Free Erotic Audio  Podcast Feed',0,0,NULL,'https://sonicerotica.com/feed/podcast/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Sonic Erotica.com &#8211; Free Erotic Audio &#187; Podcast Feed.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }



      // Erotic Audio
      if (check_rss_feed_exist(conn,(char *) "Erotic Audio")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Erotic Audio',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Den sorte boks - podcast.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Erotic Audio','Erotic Audio',0,0,NULL,'https://feeds.buzzsprout.com/687557.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Den sorte boks - podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      //
      if (check_rss_feed_exist(conn,(char *) "Den sorte boks - podcast")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Den sorte boks - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Den sorte boks - podcast.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Den sorte boks - podcast','Den sorte boks - podcast',0,0,NULL,'https://www.dr.dk/mu/feed/den-sorte-boks-podcast.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Den sorte boks - podcast.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }



      // Tech fra toppen
      if (check_rss_feed_exist(conn,(char *) "Tech fra toppen")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Tech fra toppen',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Tech fra toppen\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Tech fra toppen','Tech fra toppen',0,0,NULL,'http://www.spreaker.com/show/2832975/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Tech fra toppen.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // Brittas bedrag
      if (check_rss_feed_exist(conn,(char *) "Brittas bedrag")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Brittas bedrag',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Brittas bedrag.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Brittas bedrag',NULL,NULL,'Brittas bedrag',0,0,NULL,'https://www.dr.dk/mu/feed/britta-vs-staten.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Brittas bedrag.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }


      // Stegelmanns score
      if (check_rss_feed_exist(conn,(char *) "Stegelmanns score")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Stegelmanns score',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Stegelmanns score.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Stegelmanns score',NULL,NULL,'Stegelmanns score',0,0,NULL,'https://www.dr.dk/mu/feed/stegelmanns-scores.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Stegelmanns score.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // https://friesb4guyspodcast.libsyn.com/rss
      if (check_rss_feed_exist(conn,(char *) "Fries before guys")==0) {
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Fries before guys',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error Fries before guys.\n");
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Fries before guys',NULL,NULL,'Fries before guys ',0,0,NULL,'https://friesb4guyspodcast.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql insert error Fries before guys.\n");
          printf("SQL: %s\n",sqlselect);
        }
        rss_update=true;
      }

      // close mysql
      if (conn) mysql_close(conn);
      // download new rrs files we just insert in db
      // and update db from rss file
      if (!(dbexist)) loadrssfile(1);
    }
    //if (debugmode & 4) printf("* art = %s fpath=%s *\n",art,fpath);
    clean_stream_oversigt();                // clean old list
    strcpy(lasttmpfilename,"");    					// reset
    if (debugmode & 4) {
      printf("loading rss/stream data.\n");
      printf("art = %s tpath = %s \n",art,fpath);
    }
    // find records after type to find
    if ((strcmp(art,"")==0) && (strcmp(fpath,"")==0)) {
      // select internetcontentarticles.feedtitle,
//      sprintf(sqlselect,"select ANY_VALUE(internetcontentarticles.feedtitle) as feedtitle,ANY_VALUE(internetcontentarticles.path) as path,ANY_VALUE(internetcontentarticles.title) as title,ANY_VALUE(internetcontentarticles.description) as description,ANY_VALUE(internetcontentarticles.url) as url,ANY_VALUE(internetcontent.thumbnail),count(internetcontentarticles.feedtitle) as counter,ANY_VALUE(internetcontent.thumbnail) as thumbnail,ANY_VALUE(internetcontentarticles.time) as nroftimes,ANY_VALUE(internetcontentarticles.paththumb) from internetcontentarticles left join internetcontent on internetcontentarticles.feedtitle=internetcontent.name where mediaURL is NOT NULL group by (internetcontent.name) ORDER BY feedtitle,title DESC");
      sprintf(sqlselect,"select (internetcontentarticles.feedtitle) as feedtitle,(internetcontentarticles.path) as path,(internetcontentarticles.title) as title,(internetcontentarticles.description) as description,(internetcontentarticles.url) as url,(internetcontent.thumbnail),count(internetcontentarticles.feedtitle) as counter,(internetcontent.thumbnail) as thumbnail,(internetcontentarticles.time) as nroftimes,(internetcontentarticles.paththumb) from internetcontentarticles left join internetcontent on internetcontentarticles.feedtitle=internetcontent.name where mediaURL is NOT NULL group by (internetcontent.name) ORDER BY feedtitle,title DESC");
      getart=0;
    } else if ((strcmp(art,"")!=0) && (strcmp(fpath,"")==0)) {
      sprintf(sqlselect,"select (feedtitle) as feedtitle,(path) as path,(title) as title,(description),(url),(thumbnail),count(path),(paththumb),(mediaURL),(time) as nroftimes,(id) as id from internetcontentarticles where mediaURL is NOT NULL and feedtitle like '");
      strcat(sqlselect,art);
      strcat(sqlselect,"' GROUP BY title ORDER BY id");
      getart=1;
    } else if ((strcmp(art,"")!=0) && (strcmp(fpath,"")!=0)) {
      sprintf(sqlselect,"select ANY_VALUE(feedtitle) as feedtitle,(path) as path,(title) as title,(description),(url),(thumbnail),(paththumb),(time) as nroftimes,(id) as id from internetcontentarticles where mediaURL is NULL and feedtitle like '");
      strcat(sqlselect,art);
      strcat(sqlselect,"' AND path like '");
      strcat(sqlselect,fpath);
      strcat(sqlselect,"' ORDER BY abs(title) desc"); // ASC
      getart=2;
    }
    this->type=getart;					// husk sql type
    if (debugmode & 4) printf("RSS stream loader started... \n");
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
      antalrss_feeds=get_antal_rss_feeds_sources(conn);
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      if (mysql_query(conn,sqlselect)!=0) {
        printf("mysql insert error.\n");
        printf("SQL %s \n",sqlselect);
      }
      res = mysql_store_result(conn);
      if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (antal<maxantal)) {
          //if (debugmode & 4) printf("Get info from stream nr %d %-20s\n",antal,row[2]);
          if (antal<maxantal) {
            stack[antal]=new (struct stream_oversigt_type);
            if (stack[antal]) {
              strcpy(stack[antal]->feed_showtxt,"");          	            // show name
              strcpy(stack[antal]->feed_name,"");		                        // mythtv db feedtitle
              strcpy(stack[antal]->feed_desc,"");                           // desc
              strcpy(stack[antal]->feed_path,"");                           // mythtv db path
              strcpy(stack[antal]->feed_gfx_url,"");
              strcpy(stack[antal]->feed_gfx_mythtv,"");
              strcpy(stack[antal]->feed_streamurl,"");
              stack[antal]->feed_group_antal=0;
              stack[antal]->feed_path_antal=0;
              stack[antal]->textureId=0;
              stack[antal]->intnr=0;
              stack[antal]->nyt=false;
              // top level
              if (getart==0) {
                strncpy(stack[antal]->feed_showtxt,row[0],feed_pathlength);
                strncpy(stack[antal]->feed_name,row[0],feed_namelength);
                if (row[1]) strcpy(stack[antal]->feed_path,row[1]);
                if (row[0]) stack[antal]->feed_group_antal=get_podcasttype_antal(row[0]);        // get antal
                else stack[antal]->feed_group_antal=0;
                if (row[3]) strncpy(stack[antal]->feed_desc,row[3],feed_desclength);
                if (row[7]) strncat(tmpfilename,row[7],20);                                     // get image path
                strcpy(stack[antal]->feed_gfx_mythtv,tmpfilename);            	       		      // icon file URL
                if (row[9]) strcpy(tmpfilename,row[9]);
                get_webfilenamelong(downloadfilename,tmpfilename);                              // get file name from url
                // check filename
                strcpy(downloadfilename1,downloadfilename);                 // back name before change
                int mmm=0;
                while(mmm<strlen(downloadfilename)) {
                  if ((downloadfilename[mmm]=='?') || (downloadfilename[mmm]=='&') || (downloadfilename[mmm]=='=')) downloadfilename[mmm]='_';
                  mmm++;
                }
                getuserhomedir(homedir);                                                  // get homedir
                strcpy(downloadfilenamelong,homedir);
                strcat(downloadfilenamelong,"/rss/images/");
                strcat(downloadfilenamelong,downloadfilename);
                if (!(file_exists(downloadfilenamelong))) {
                  // download gfx file and use as icon
                  if (debugmode & 4) printf("Loading1 image %s realname %s \n",tmpfilename,downloadfilenamelong);
                  if (get_webfile2(tmpfilename,downloadfilenamelong)==-1) {
                    printf("Download error \n");
                  } else strcpy(tmpfilename,"");
                }
                // tmpfilename is now the bame of the icon
                strncpy(stack[antal]->feed_gfx_mythtv,tmpfilename,200);	                // mythtv icon file
                antal++;
              } else {
                // if first creat back button
                if (antal==0) {
                  stack[antal]->textureId=_textureIdback;			                    // back icon
                  strcpy(stack[antal]->feed_showtxt,"BACK");
                  if (row[0]) strncpy(stack[antal]->feed_name,row[0],feed_namelength);
                  if (row[1]) strncpy(stack[antal]->feed_path,row[1],feed_pathlength);
                  //strcpy(stack[antal]->feed_streamurl,row[4]);
                  if (row[3]) strncpy(stack[antal]->feed_desc,row[3],feed_desclength);
                  if (row[5]) strncpy(stack[antal]->feed_gfx_url,row[5],feed_url);            // feed (db link) url
                  stack[antal]->feed_group_antal=0;
                  stack[antal]->intnr=0;	                               					// intnr=0 = back button type
                  antal++;
                }
                // alloc new element in array
                if (stack[antal]==NULL) stack[antal]=new (struct stream_oversigt_type);
                if (stack[antal]) {
                  stack[antal]->intnr=1;
                  if (row[5]) strncpy(stack[antal]->feed_gfx_url,row[5],feed_url);
                  if (getart==1) {
                    // old ver used path from mysql
                    //if (row[1]) strncpy(stack[antal]->feed_showtxt,row[1],feed_pathlength);
                    if (row[2]) strncpy(stack[antal]->feed_showtxt,row[2],feed_pathlength);
                  } else {
                    if (row[2]) strncpy(stack[antal]->feed_showtxt,row[2],feed_pathlength);
                  }
                  if (row[1]) strncpy(stack[antal]->feed_path,row[1],feed_pathlength);		// path
                  if (row[2]) strncpy(stack[antal]->feed_name,row[0],feed_namelength);		// feedtitle
                  // old ver
                  //if (row[4]) strncpy(stack[antal]->feed_streamurl,row[4],feed_url);		  // save play url
                  if (row[8]) strncpy(stack[antal]->feed_streamurl,row[8],feed_url);		  // save play url
                  switch(getart) {
                    case 0: if (row[9]) strcpy(tmpfilename,row[9]);
                            break;
                    case 1: if (row[7]) strcpy(tmpfilename,row[7]);
                            break;
                    case 2: if (row[5]) strcpy(tmpfilename,row[5]);
                            break;
                  }
                  // test antal afspillinger this 0 set som new rss
                  if (row[9]) {
                    if (atoi(row[9])==0) stack[antal]->nyt=true; else stack[antal]->nyt=false;
                  }
                  if (strlen(tmpfilename)>7) {
                    if (strncmp(tmpfilename,"%SHAREDIR%",10)==0) {
                      strcpy(tmpfilename,"/usr/share/mythtv");                            // mythtv path
                      if (strlen(row[7])>10) {
                        if (getart==1) {
                          if (row[7]) strncat(tmpfilename,row[7]+10,100);
                        }
                        if (getart==2) {
                          if (row[6]) strncat(tmpfilename,row[6]+10,100);
                        }
                      }
                    } else {
                      // rss download
                      // downloadfilename = name on file, from tmpfilename = full web url
                      get_webfilenamelong(downloadfilename,tmpfilename);          // get file name from url
                      // check filename fro ? or = and replace to _
                      strcpy(downloadfilename1,downloadfilename);                 // back name before change
                      int mmm=0;
                      while(mmm<strlen(downloadfilename)) {
                        if ((downloadfilename[mmm]=='?') || (downloadfilename[mmm]=='&') || (downloadfilename[mmm]=='=')) downloadfilename[mmm]='_';
                        mmm++;
                      }
                      strcpy(lasttmpfilename,tmpfilename);			              // husk file name
                      // save file in  user homedir rss/
                      getuserhomedir(homedir);                                  // get homedir
                      strcpy(downloadfilenamelong,homedir);
                      strcat(downloadfilenamelong,"/rss/images/");
                      strcat(downloadfilenamelong,downloadfilename);
                      if (!(file_exists(downloadfilenamelong))) {
                        if (debugmode & 4) printf("Loading2 image %s realname %s \n",tmpfilename,downloadfilenamelong);
                        // download gfx file and use as icon
                        if (get_webfile2(tmpfilename,downloadfilenamelong)==-1) {
                          printf("Download error \n");
                        } else strcpy(tmpfilename,"");
                      }
                      strcpy(tmpfilename,downloadfilenamelong);
                    }
                  } else strcpy(tmpfilename,"");
                  strncpy(stack[antal]->feed_gfx_mythtv,tmpfilename,200);	                // save icon file path in stack struct
  //                          strcpy(stack[antal]->feed_streamurl,row[4]);	// stream url
                  if (row[3]) strncpy(stack[antal]->feed_desc,row[3],feed_desclength);
                  // no texture now
                  stack[antal]->textureId=0;
                  // opdatere group antal
                  if (getart==1) stack[antal]->feed_group_antal=atoi(row[6]); else stack[antal]->feed_group_antal=0;
                  antal++;
                }
              }
            }
          }
        }
        mysql_close(conn);
      } else {
        if (debugmode & 4) printf("No stream data loaded \n");
      }
      //load_stream_gfx();
      //
      // load all the data in phread datainfoloader
      // web gfx file loader in phread
      pthread_t loaderthread;           // the load
      int rc=pthread_create(&loaderthread,NULL,loadweb,NULL);
      if (rc) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
      }
      return(antal-1);
    } else printf("Failed to update feed stream db, can not connect to database: %s Error: %s\n",dbname,mysql_error(conn));
    if (debugmode & 4) printf("RSS/PODCAST loader done... \n");
    return(0);
}



// ****************************************************************************************
//
// thread web loader (loading all icons)
//
// ****************************************************************************************


void *loadweb(void *data) {
  if (debugmode & 4) printf("Start web icon loader thread\n");
  streamoversigt.loadweb_stream_iconoversigt();
  if (debugmode & 4) printf("End/Stop web icon loader thread\n");
}



// ****************************************************************************************
// in use
// downloading all rss podcast icon gfx
//
// ****************************************************************************************

int stream_class::loadweb_stream_iconoversigt() {
  int antal;
  int nr=0;
  int loadstatus;
  char tmpfilename[2000];
  char downloadfilename[2900];
  char downloadfilenamelong[5000];
  char homedir[200];
  antal=this->streamantal();
  this->gfx_loaded=false;
  if (debugmode & 4) printf("rss stream gfx download start \n");
  while(nr<antal) {
    if (strcmp(stack[nr]->feed_gfx_mythtv,"")!=0) {
      loadstatus=0;
      // return downloadfilename from stack[nr]->feed_gfx_mythtv
      strcpy(tmpfilename,stack[nr]->feed_gfx_mythtv);
      if (strncmp(tmpfilename,"http://",7)==0) {
        // download file from web
        // return dowloadfilebame = file downloaded name no path
        get_webfilename(downloadfilename,tmpfilename);
        // add download path
        // add download filename to class opbject
        getuserhomedir(downloadfilenamelong);
        strcat(downloadfilenamelong,"/rss/images/");
        strcat(downloadfilenamelong,downloadfilename);
        if ((!(file_exists(downloadfilenamelong))) && (check_zerro_bytes_file(downloadfilenamelong)==0))  {
          if (debugmode & 4) printf("nr %3d Downloading : %s \n",nr,tmpfilename);
          loadstatus=get_webfile(tmpfilename,downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
        } else {
          if (!(file_exists(downloadfilenamelong))) loadstatus=get_webfile2(tmpfilename,downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
            //printf("File exist %s then set filename \n",downloadfilenamelong);
        }
      } else if (strncmp(tmpfilename,"https://",8)==0) {
        //strcpy(lastfile,downloadfilename);
        get_webfilename(downloadfilename,tmpfilename);
        getuserhomedir(downloadfilenamelong);
        // build path
        strcat(downloadfilenamelong,"/rss/images/");
        strcat(downloadfilenamelong,downloadfilename);
        if ((!(file_exists(downloadfilenamelong))) && (check_zerro_bytes_file(downloadfilenamelong)==0)) {
          if (debugmode & 4) printf("nr %3d Downloading : %s \n",nr,tmpfilename);
          loadstatus=get_webfile2(tmpfilename,downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
        } else {
          if (!(file_exists(downloadfilenamelong))) loadstatus=get_webfile2(tmpfilename,downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
        }
      }
      // set recordnr loaded info to update users view
      this->stream_oversigt_nowloading=nr;
    }
    nr++;
  }
  if (nr>0) this->gfx_loaded=true; else this->gfx_loaded=false;
  if (debugmode & 4) {
    if (gfx_loaded) printf("rss stream gfx download end ok. \n");
    else printf("rss stream gfx download error. \n");
  }
  return(1);
}


// ****************************************************************************************
// load all streams gfx and save it
// NOT IN USE
// used as thread
//
// ****************************************************************************************


void *load_all_stream_gfx(void *data) {
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res,*res1;
    MYSQL_ROW row,row1;
    char *database = (char *) "mythtvcontroller";
    char sqlselect[1024];
    char sqlselect1[1024];
    char downloadfilename[1024];
    char downloadfilenamelong[1024];
    char tmpfilename[1024];
    char lastfile[1024];
    char homedir[2048];
    bool online;
    int getart=0;
    bool loadstatus;
    bool filechange;
    int antal=0;
    int total_antal=0;
    int nr;
    if (debugmode & 4) printf("Start gfx thread loader\n ");
    strcpy(lastfile,"");
    strcpy(sqlselect,"select ANY_VALUE(internetcontent.name),ANY_VALUE(internetcontentarticles.path),count(internetcontentarticles.feedtitle),ANY_VALUE(internetcontent.thumbnail) from internetcontentarticles left join internetcontent on internetcontentarticles.feedtitle=internetcontent.name group by internetcontentarticles.feedtitle");
    conn=mysql_init(NULL);
    // Connect to database
    try {
      if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
          mysql_query(conn,"set NAMES 'utf8'");
          res = mysql_store_result(conn);
          mysql_query(conn,sqlselect);
          res = mysql_store_result(conn);
          if (debugmode & 4) printf("\n\nLoading RSS gfx..\n\n");
          if (res) {
              while ((row = mysql_fetch_row(res)) != NULL) {
                sprintf(sqlselect1,"select feedtitle,path,title,description,url,thumbnail,path,paththumb from internetcontentarticles where feedtitle like '%s' order by path,title asc",row[0]);
                mysql_query(conn,"set NAMES 'utf8'");
                res1 = mysql_store_result(conn);
                mysql_query(conn,sqlselect1);
                res1 = mysql_store_result(conn);
                if (res1) {
                  antal=0;
                  if (debugmode & 4) printf("Found : %40s ",row[1]);
                  nr=0;
                  while ((row1 = mysql_fetch_row(res1)) != NULL) {
                    antal++;
                    strcpy(tmpfilename,row1[5]);
                    if (stream_loadergfx_started_break) break;
                    // http
                    if (strncmp(tmpfilename,"http://",7)==0) {
                      strcpy(lastfile,downloadfilename);
                      get_webfilename(downloadfilename,tmpfilename);
                      getuserhomedir(homedir);                                                  // get home dir
                      strcpy(downloadfilenamelong,homedir);
                      strcat(downloadfilenamelong,"/rss/images/");
                      strcat(downloadfilenamelong,downloadfilename);
                      filechange=strcmp(lastfile,downloadfilename);
                      if ((!(file_exists(downloadfilenamelong))) && (filechange)) {
                        if (debugmode & 4) printf("nr %3d Downloading : %s \n",nr,tmpfilename);
                        loadstatus=get_webfile2(tmpfilename,downloadfilenamelong);
                        nr++;
                      } else {
                        if (debugmode & 4) printf("nr %3d exist : %s \n",nr,tmpfilename);
                      }
                      total_antal++;
                    // https
                    } else if (strncmp(tmpfilename,"https://",8)==0) {
                      strcpy(lastfile,downloadfilename);
                      get_webfilename(downloadfilename,tmpfilename);
                      getuserhomedir(homedir);
                      strcpy(downloadfilenamelong,homedir);                                   // get home dir
                      strcat(downloadfilenamelong,"/rss/images/");
                      strcat(downloadfilenamelong,downloadfilename);
                      filechange=strcmp(lastfile,downloadfilename);
                      if ((!(file_exists(downloadfilenamelong))) && (filechange)) {
                        if (debugmode & 4) printf("nr %3d Downloading : %s \n",nr,tmpfilename);
                        loadstatus=get_webfile2(tmpfilename,downloadfilenamelong);
                        nr++;
                      } else {
                        if (debugmode & 4) printf("nr %3d exist : %s \n",nr,tmpfilename);
                      }
                      total_antal++;
                    }
                  }
                }
                if (stream_loadergfx_started_break) break;
              }
              if (stream_loadergfx_started_break==false) stream_loadergfx_started_done=true;
              stream_loadergfx_started_break=false;
          }
          if (conn) mysql_close(conn);
      }
    }
    catch (...) {
      printf("Error open mysql connection.\n");
    }
    if (debugmode & 4) printf("End gfx thread loader\n ");
}



// ****************************************************************************************
//
// play stream by vlc
//
// ****************************************************************************************


void stream_class::playstream(char *url) {
    vlc_controller::playmedia(url);
}


// ****************************************************************************************
//
// ****************************************************************************************


void stream_class::show_stream_oversigt(GLuint normal_icon,GLuint empty_icon,GLuint empty_icon1,int _mangley,int stream_key_selected)

{
    int j,ii,k,pos;
    float buttonsizey=160.0f;                                                   // button size
    float buttonsizex=200.0f;                                                   // button size
    float yof=orgwinsizey-(buttonsizey);                                        // start ypos
    float xof=0.0f;
    int lstreamoversigt_antal=9*6;
    int i=0;                                                                    // data ofset in stack array
    unsigned int sofset=0;
    int bonline=8;                                                              // antal pr linie
    float boffset;
    char gfxfilename[200];
    char downloadfilename[200];
    char downloadfilenamelong[1024];
    char *gfxshortnamepointer;
    char gfxshortname[200];
    char temptxt[200];
    char word[200];
    static char downloadfilename_last[1024];
    int antal_loaded=0;
    static int stream_oversigt_loaded_done=0;
    GLuint texture;
    static GLuint last_texture;
    char *base,*right_margin;
    int length,width;
    int pline=0;

    if (stream_oversigt_loaded_nr==0) strcpy(downloadfilename_last,"");
    if ((this->streamantal()) && (stream_oversigt_loaded==false) && (this->stream_oversigt_loaded_nr<this->streamantal())) {
      if (stack[stream_oversigt_loaded_nr]) strcpy(gfxfilename,stack[stream_oversigt_loaded_nr]->feed_gfx_mythtv);
      else strcpy(gfxfilename,"");
      strcpy(gfxshortname,"");
      gfxshortnamepointer=strrchr(gfxfilename,'.');     // get last char = type of file
      if (gfxshortnamepointer) {
        strcpy(gfxshortname,gfxshortnamepointer);
      }
      // load texture if none loaded
      // get_texture return 0 if not loaded
      if (get_texture(stream_oversigt_loaded_nr)==0) {
        if (strcmp(gfxfilename,"")!=0) {
          // check om der findes en downloaded icon
          strcpy(downloadfilenamelong,"");
          strcat(downloadfilenamelong,gfxfilename);
          if (strcmp(downloadfilename_last,gfxfilename)!=0) {
            // check om filen findes i cache dir eller i mythtv netvision dir
            if (file_exists(gfxfilename)) {
              texture=loadTexture ((char *) gfxfilename);
              if (texture) set_texture(stream_oversigt_loaded_nr,texture);
              last_texture=texture;
              antal_loaded+=1;
            } else if (file_exists(downloadfilenamelong)) {
              // er det ikke samme texture som sidst loaded så load it
              // else set last used
              texture=loadTexture ((char *) downloadfilenamelong);
              if (texture) set_texture(stream_oversigt_loaded_nr,texture);
              last_texture=texture;
              antal_loaded+=1;
            } else texture=0;
          } else {
            if (last_texture) set_texture(stream_oversigt_loaded_nr,last_texture);
            antal_loaded+=1;
          }
          // husk last file name
          strcpy(downloadfilename_last,downloadfilenamelong);
        }
      }
      // down loading ?
      if (stream_oversigt_loaded_nr==this->streamantal()) {
        stream_oversigt_loaded=true;
        stream_oversigt_loaded_done=true;
      } else stream_oversigt_loaded_nr++;
    }

    if (!(gfx_loaded)) {
      stream_oversigt_loaded_nr=0;
      stream_oversigt_loaded=false;
    }
    // calc start pos (ofset)
    sofset=(_sangley/40)*8;
    // draw icons
    while((i<lstreamoversigt_antal) && (i+sofset<antal) && (stack[i+sofset]!=NULL)) {
      if (((i % bonline)==0) && (i>0)) {
        yof=yof-(buttonsizey+20);
        xof=0;
      }
      // selected biger
      if (i+1==(int) stream_key_selected) {
        buttonsizey=170.0f;
        buttonsizex=200.0f;
      } else {
        buttonsizey=160.0f;
        buttonsizex=200.0f;
      }
      if (stack[i+sofset]->textureId) {
        // stream icon
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindTexture(GL_TEXTURE_2D,empty_icon1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsizex-10, yof+buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsizex-10, yof+10 , 0.0);
        glEnd();
        glPushMatrix();
        // indsite draw icon rss gfx
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ONE);
        glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        if (tema==5) {
          glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsizex-10, yof+buttonsizey-20 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsizex-10, yof+10 , 0.0);
        } else {
          glTexCoord2f(0, 0); glVertex3f( xof+20, yof+20, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+20,yof+buttonsizey-30, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsizex-20, yof+buttonsizey-30 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsizex-20, yof+20 , 0.0);
        }
        glEnd();
        // show nyt icon note
        if (stack[i+sofset]->nyt) {
          glBindTexture(GL_TEXTURE_2D,newstuf_icon);
          //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( xof+10+130, yof+10, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+10+130,yof+66-20, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+66-10+130, yof+66-20 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+66-10+130, yof+10 , 0.0);
          glEnd();
        }
        glPopMatrix();
      } else {
        // no draw default icon
        glPushMatrix();
        // indsite draw radio station icon
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindTexture(GL_TEXTURE_2D,empty_icon1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsizex-10, yof+buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsizex-10, yof+10 , 0.0);
        glEnd();
        // show nyt icon note
        if (stack[i+sofset]->nyt) {
          glBindTexture(GL_TEXTURE_2D,newstuf_icon);
          glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
          glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( xof+10+130, yof+10, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+10+130,yof+66-20, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+66-10+130, yof+66-20 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+66-10+130, yof+10 , 0.0);
          glEnd();
        }
        glPopMatrix();
      }
      // draw numbers in group
      if (stack[i+sofset]->feed_group_antal>1) {
        // show numbers in group
        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTranslatef(xof+22,yof+14,0);
        glRasterPos2f(0.0f, 0.0f);
        glScalef(configdefaultstreamfontsize, configdefaultstreamfontsize, 1.0);
        glColor4f(1.0f, 1.0f, 1.0f,1.0f);
        sprintf(temptxt,"Feeds %-4d",stack[i+sofset]->feed_group_antal);
        glcRenderString(temptxt);
        glPopMatrix();
      }
      // show text of element
      glPushMatrix();
      pline=0;
      glTranslatef(xof+20,yof-10,0);
      glDisable(GL_TEXTURE_2D);
      glScalef(configdefaultstreamfontsize, configdefaultstreamfontsize, 1.0);
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glRasterPos2f(0.0f, 0.0f);
      glDisable(GL_TEXTURE_2D);
      strcpy(temptxt,stack[i+sofset]->feed_showtxt);        // text to show
      base=temptxt;
      length=strlen(temptxt);
      width = 20;
      bool stop=false;
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
      glPopMatrix();
      // next button
      i++;
      xof+=(buttonsizex+10);
    }
    // no records loaded error
    if ((i==0) && (antal_rss_streams()==0)) {
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      glBindTexture(GL_TEXTURE_2D,_textureIdloading);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/3), 200 , 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/3), 200+150, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/3)+400, 200+150 , 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/3)+400, 200 , 0.0);
      glEnd();
      glPushMatrix();
      xof=700;
      yof=260;
      glTranslatef(xof, yof ,0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glDisable(GL_TEXTURE_2D);
      glScalef(22.0, 22.0, 1.0);
      glcRenderString("Please wait Loading ...");
      glEnable(GL_TEXTURE_2D);
      glPopMatrix();
    }
}
