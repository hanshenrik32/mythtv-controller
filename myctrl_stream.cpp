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
    gfx_loaded=false;			// gfx loaded
    stream_is_playing=false;
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

// vlc stop player

void stream_class::softstopstream() {
  if ((vlc_in_playing()) && (stream_is_playing)) vlc_controller::stopmedia();
  stream_is_playing=false;
}


// get length on stream

unsigned long stream_class::get_length_in_ms() {
  vlc_controller::get_length_in_ms();
}


// jump in player

float stream_class::jump_position(float ofset) {
    ofset=vlc_controller::jump_position(ofset);
    return(ofset);
}

// to play streams from web
//vlc_m = libvlc_media_new_location(vlc_inst, "http://www.ukaff.ac.uk/movies/cluster.avi");

// start playing movie by vlclib

int stream_class::playstream(int nr) {
    char path[PATH_MAX];                                  // max path length from os
    strcpy(path,"");
    strcat(path,get_stream_url(nr));
    stream_is_playing=true;
    vlc_controller::playmedia(path);
    return(1);
}


int stream_class::playstream_url(char *path) {
    stream_is_playing=true;
    vlc_controller::playwebmedia(path);
    return(1);
}


float stream_class::getstream_pos() {
    return(vlc_controller::get_position());
}


void stream_class::update_rss_nr_of_view(char *url) {
  // mysql vars
  char sqlinsert[32768];
  MYSQL *conn;
  MYSQL_RES *res,*res1;
  MYSQL_ROW row;
  char *database = (char *) "mythtvcontroller";
  conn=mysql_init(NULL);
  // get homedir
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    sprintf(sqlinsert,"update mythtvcontroller.internetcontentarticles set time=time+1 where mediaURL like '%s'",url);
    mysql_query(conn,sqlinsert);
    res = mysql_store_result(conn);
    mysql_free_result(res);
    mysql_close(conn);
  }
}


//
// used to download rss file from web to db info (url is flag for master rss file (mediaURL IS NULL))
// in db if mediaURL have url this is the rss feed loaded from rss file
// updaterssfile bool is do it now (u key in overview)

int stream_class::loadrssfile(bool updaterssfile) {
  // mysql vars
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
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        stream_rssparse_nowloading++;
        if (debugmode & 4) printf("Hent info om stream title %10s \n",row[0]);
        if ((row[3]) && (strcmp(row[3],"")!=0)) {
          getuserhomedir(homedir);
          strcpy(totalurl,"wget '");
          if (row[7]) strcat(totalurl,row[7]); else if (row[3]) strcat(totalurl,row[3]);
          strcat(totalurl,"' -o '");
          strcat(totalurl,homedir);
          strcat(totalurl,"/rss/wget.log'");
          strcat(totalurl," -O '");
          strcat(totalurl,homedir);
          strcat(totalurl,"/rss/");
          if (row[3]) strcat(totalurl,row[3]);
          strcat(totalurl,".rss'");
          strcpy(parsefilename,homedir);
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
            if (debugmode & 4) {
              printf("Create/update podcast from %s url in db\n",row[0]);
            }
            sprintf(sqlinsert,"UPDATE internetcontentarticles set paththumb='%s' where feedtitle like '%s' and paththumb IS NULL",baseicon,row[0]);
            mysql_query(conn,sqlinsert);
            res1 = mysql_store_result(conn);
          }
          // if podcast is not rss and title ok
          if ((strcmp(row[3],"")!=0) && (row[23])) {
            if (atoi(row[23])==1) {
              if (debugmode & 4) {
                printf("Create/update podcast from %s url in db\n",row[0]);
              }
              sprintf(sqlinsert,"UPDATE internetcontentarticles set mediaURL=url where podcast=1 and feedtitle like '%s'",row[0]);
              mysql_query(conn,sqlinsert);
              res1 = mysql_store_result(conn);
            }
          }
        }
      }
      mysql_free_result(res);
      stream_rssparse_nowloading=0;
    }
    mysql_close(conn);
  } else return(-1);
  return(1);
}



char search_and_replace2(char *text) {
  int n=0;
  int nn=0;
  char newtext[2048];
  strcpy(newtext,"");
  while(n<strlen(text)) {
    if (text[n]=='"') {
      strcat(newtext,"'");
      n++;
//      nn+=1;
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

//
// xml parser
//

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
                  if (debugmode & 4) {
                    printf("Create/update podcast %s in db\n",rssprgtitle);
                  }
                  search_and_replace2(rssprgtitle);
                  search_and_replace2(rssprgfeedtitle);
                  search_and_replace2(rssprgdesc);
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
            sprintf(sqlinsert,"select feedtitle from internetcontentarticles where (feedtitle like '%s' mediaURL like '%s' and title like '%s')",rssprgtitle,rssvideolink,rssprgfeedtitle);
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
  } else {
    if (debugmode & 4) printf("Read error on %s xmlfile downloaded to rss dir \n",filename);
  }
}


// get antal podcast af type

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

//
// opdate show liste in view (det vi ser)
//

// load felt 7 = mythtv gfx icon
// fpath=stream path
// atr = stream name

int stream_class::opdatere_stream_oversigt(char *art,char *fpath) {
    char sqlselect[2048];
    char tmpfilename[1024];
    char lasttmpfilename[1024];
    char downloadfilename[1024];
    char downloadfilename1[1024];
    char downloadfilenamelong[1024];
    char homedir[1024];
    // mysql vars
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
      printf("Creating/Update database for new rss feed\n");
      // thumbnail   = name of an local image file
      // commandline = Program to fetch content with
      // updated     = Time of last update
      if ((!(dbexist)) && (mysql_query(conn,"CREATE database if not exist mythtvcontroller")!=0)) {
        printf("mysql db create error. %s \n");
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
      if (!(dbexist)) {
        res = mysql_store_result(conn);
        // create default master rss feed source
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent (name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('i-seng-med-nova',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Aftenshowet',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('CNET',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Anders Lund Madsen i Den Yderste By',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('So ein Ding',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Verdens Bedste Filmklub',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Krager og Drager - Dansk Game of Thrones Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('The Verge',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Dynamo',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Jupiter Broadcasting',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Bonderøven',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Kontant',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Movieclips Coming Soon',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('NASA',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Troldspejlet Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('GameSpot',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('The Story from The Guardian',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Droner og kanoner',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('DEN GRÅ SIDE',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('TechSNAP',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Tech Talk Today',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);


        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('PersonalityHacker',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Bitch Sesh',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('2 Dope Queens',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Ramajetterne',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('De Sorte Spejdere',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Nak & æd',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Ubådssagen',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Videnskabens Verden',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Why You Push That Button',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Gametest',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Gravity Assist',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('TED',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('TV 2 - Tech',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Hva så?! med Christian Fuhlendorff',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Mads & Monopolet',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Det røde rum',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Den Korte Weekendavis',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Radio24syv Nyheder',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Selvsving',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Lille Nørd',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('DR Sofus',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Best of YouTube',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('CBS This Morning',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('CBS Evening News',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Vergecast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Elektronista',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Thirdear',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Linux Action News',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Linux Action Show',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('GNU World Order',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('The Command Line Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Her går det godt',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('he RC Newb Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('NASA What Up',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('P7 MIX Maraton - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('going linux',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Fire forførende radiofortællinger',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Videnskab',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Den nye stil - historien om dansk rap',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Big Audio Drama Adventures',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('META',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Voksen ABC podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Third Ear',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Sex Nerd Sandra',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Skru op for sex',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,host) VALUES ('Techsistens',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);


        //



        //
        // create default master rss feed source
        //

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('i-seng-med-nova',NULL,NULL,'i-seng-med-nova',0,0,NULL,'https://tools.planetradio.co.uk/core/podcasts/rss.php?name=i-seng-med-nova',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Aftenshowet',NULL,NULL,'Aftenshowet',0,0,NULL,'https://www.dr.dk/mu/Feed/aftenshowet.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('CNET',NULL,NULL,'CNET',0,0,NULL,'http://feed.cnet.com/feed/podcast/all/hd.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Anders Lund Madsen i Den Yderste By',NULL,NULL,'Anders lund massen i den udereste by',0,0,NULL,'http://www.dr.dk/mu/Feed/anders-lund-madsen-i-den-yderste-by.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('So ein Ding',NULL,NULL,'So ein Ding',0,0,NULL,'https://www.dr.dk/mu/Feed/so-ein-ding?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Verdens Bedste Filmklub',NULL,NULL,'Verdens Bedste Filmklub',0,0,NULL,'http://lytbar.dk/wordpress/?feed=rss2&cat=7',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Krager og Drager - Dansk Game of Thrones Podcast',NULL,NULL,'Krager og Drager - Dansk Game of Thrones Podcast',0,0,NULL,'http://www.kragerogdrager.dk/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Verge',NULL,NULL,'The Verge',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCddiUEpeqJcYeBxX1IVBKvQ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Dynamo',NULL,NULL,'Dynamo',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UC7SDsqJba5428-EOBZWOn3w',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Tech snap',NULL,NULL,'Tech snap',0,0,NULL,'https://youtube.com/feeds/videos.xml?channel_id=UCHugE6eRhqB9_AZQh4DDbIw',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Bonderøven',NULL,NULL,'Bonderøven',0,0,NULL,'https://www.dr.dk/mu/Feed/bonderoven-alle.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Kontant',NULL,NULL,'Kontant',0,0,NULL,'https://www.dr.dk/mu/Feed/kontant-2.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Movieclips Coming Soon',NULL,NULL,'Movieclips Coming Soon',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCkR0GY0ue02aMyM-oxwgg9g',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Troldspejlet Podcast',NULL,NULL,'Troldspejlet Podcast',0,0,NULL,'https://www.dr.dk/mu/feed/troldspejlet-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('GameSpot',NULL,NULL,'GameSpot',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCbu2SsF-Or3Rsn3NxqODImw',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

  /*
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles VALUES ('ISS Live FEED',NULL,NULL,'ISS Live FEED',0,0,NULL,'https://www.youtube.com/watch?v=RtU_mdL2vBM',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
  */

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('TechSNAP',NULL,NULL,'TechSNAP',0,0,NULL,'http://techsnap.systems/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Tech Talk Today',NULL,NULL,'TechSNAP',0,0,NULL,'http://techtalk.today/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Story from The Guardian',NULL,NULL,'The Story from The Guardian',0,0,NULL,'https://www.theguardian.com/news/series/the-story/podcast.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Droner og kanoner',NULL,NULL,'Droner og kanoner',0,0,NULL,'https://www.dr.dk/mu/Feed/droner-og-kanoner.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('DEN GRÅ SIDE',NULL,NULL,'DEN GRÅ SIDE',0,0,NULL,'http://www.spreaker.com/show/2108328/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('PersonalityHacker',NULL,NULL,'PersonalityHacker',0,0,NULL,'http://feeds.feedburner.com/PersonalityHackerPodcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Bitch Sesh',NULL,NULL,'Bitch Sesh',0,0,NULL,'http://rss.earwolf.com/bitch-sesh',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('2 Dope Queens',NULL,NULL,'2 Dope Queens',0,0,NULL,'http://feeds.wnyc.org/2dopequeens',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Ramajetterne',NULL,NULL,'Ramajetterne',0,0,NULL,'https://www.dr.dk/mu/Feed/ramajetterne-2.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('De Sorte Spejdere',NULL,NULL,'De Sorte Spejdere',0,0,NULL,'http://www.dr.dk/mu/Feed/de-sorte-spejdere-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Nak & æd',NULL,NULL,'Nak & æd',0,0,NULL,'http://www.dr.dk/mu/Feed/nak-og-aed-alle.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Ubådssagen',NULL,NULL,'Ubådssagen',0,0,NULL,'http://www.dr.dk/mu/Feed/ubadssagen.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles(feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Videnskabens Verden',NULL,NULL,'Ubådssagen',0,0,NULL,'http://www.dr.dk/mu/Feed/videnskabens-verden.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Why You Push That Button',NULL,NULL,'Why You Push That Button',0,0,NULL,'http://feeds.feedburner.com/WhydYouPushThatButton',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Gametest',NULL,NULL,'Gametest',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCuX8EWTbH--tyha5FUoBSrg',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Gravity Assist',NULL,NULL,'Gravity Assist',0,0,NULL,'https://www.nasa.gov/rss/dyn/Gravity-Assist.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('TED',NULL,NULL,'TED',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCAuUUnT6oDeKwE6v1NGQxug',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('TV 2 - Tech',NULL,NULL,'TV 2 - Tech',0,0,NULL,'http://feeds.tv2.dk/beep_seneste/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Hva så?! med Christian Fuhlendorff',NULL,NULL,'Hva så?! med Christian Fuhlendorff',0,0,NULL,'http://hva-saa.podomatic.com/rss2.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Mads & Monopolet',NULL,NULL,'Mads & Monopolet',0,0,NULL,'https://www.dr.dk/mu/feed/mads-monopolet-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Det røde rum',NULL,NULL,'Det røde rum',0,0,NULL,'https://www.dr.dk/mu/Feed/det-roede-rum-radio.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Den Korte Weekendavis',NULL,NULL,'Den Korte Weekendavis',0,0,NULL,'http://arkiv.radio24syv.dk/audiopodcast/channel/10839671',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Radio24syv Nyheder',NULL,NULL,'Radio24syv Nyheder',0,0,NULL,'http://arkiv.radio24syv.dk/audiopodcast/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Selvsving',NULL,NULL,'Selvsving',0,0,NULL,'https://www.dr.dk/mu/Feed/selvsving?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Lille Nørd',NULL,NULL,'Lille Nørd',0,0,NULL,'https://www.dr.dk/mu/Feed/lille-noerd?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('DR Sofus',NULL,NULL,'DR Sofus',0,0,NULL,'https://www.dr.dk/mu/Feed/sofus-2.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Best of YouTube',NULL,NULL,'DR Sofus',0,0,NULL,'http://feeds.feedburner.com/boyt',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('CBS This Morning',NULL,NULL,'CBS This Morning',0,0,NULL,'http://cbsradionewsfeed.com/rss.php?id=149&ud=12',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('CBS Evening News',NULL,NULL,'CBS Evening News',0,0,NULL,'http://cbsradionewsfeed.com/rss.php?id=126&ud=12',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Vergecast',NULL,NULL,'Vergecast',0,0,NULL,'http://feeds.feedburner.com/ThisIsMyNextPodcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Elektronista',NULL,NULL,'Elektronista',0,0,NULL,'https://arkiv.radio24syv.dk/audiopodcast/channel/3843152',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Thirdear',NULL,NULL,'Thirdear',0,0,NULL,'https://thirdear.podbean.com/feed/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Linux Action News',NULL,NULL,'Linux Action News',0,0,NULL,'http://linuxactionnews.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('GNU World Order',NULL,NULL,'GNU World Order',0,0,NULL,'http://gnuworldorder.info/ogg.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Linux Action Show',NULL,NULL,'Linux Action Show',0,0,NULL,'http://feeds.feedburner.com/linuxashd?format=xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Command Line Podcast',NULL,NULL,'The Command Line Podcast',0,0,NULL,'https://thecommandline.net/cmdln_free',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);
        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Her går det godt',NULL,NULL,'Her går det godt',0,0,NULL,'http://www.spreaker.com/show/2093919/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The RC Newb Podcast',NULL,NULL,'The RC Newb Podcast',0,0,NULL,'https://rcnewb.com/feed/podcast/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('NASA What Up',NULL,NULL,'NASA What Up',0,0,NULL,'https://www.nasa.gov/rss/dyn/whats_up.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('P7 MIX Maraton - podcast',NULL,NULL,'P7 MIX Maraton',0,0,NULL,'https://www.dr.dk/mu/feed/p7-maraton.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('going linux',NULL,NULL,'going linux',0,0,NULL,'http://goinglinux.com/mp3podcast.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Fire forførende radiofortællinger',NULL,NULL,'Fire forførende radiofortællinger',0,0,NULL,'https://www.dr.dk/mu/feed/fire-forfoerende-radiofortaellinger.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Videnskab',NULL,NULL,'Videnskab',0,0,NULL,'http://feeds.soundcloud.com/users/soundcloud:users:4378703/sounds.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Den nye stil - historien om dansk rap',NULL,NULL,'Historien-om-dansk-rap',0,0,NULL,'https://www.dr.dk/mu/feed/den-nye-stil-historien-om-dansk-rap.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Big Audio Drama Adventures',NULL,NULL,'Big Audio Drama Adventures',0,0,NULL,'https://giantgnome.com/feed/podcast/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('META',NULL,NULL,'META',0,0,NULL,'https://www.spreaker.com/show/2663274/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);


        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Voksen ABC podcast',NULL,NULL,'Voksen ABC podcast',0,0,NULL,'http://voksenabc.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);


        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Third Ear',NULL,NULL,'Third Ear',0,0,NULL,'https://thirdear.podbean.com/feed.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Sex Nerd Sandra',NULL,NULL,'Sex Nerd Sandra',0,0,NULL,'http://sexnerdsandra.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Skru op for sex',NULL,NULL,'Skru op for sex',0,0,NULL,'https://www.spreaker.com/show/3006683/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Techsistens',NULL,NULL,'Techsistens',0,0,NULL,'http://feeds.soundcloud.com/users/soundcloud:users:56822074/sounds.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)");
        if (mysql_query(conn,sqlselect)!=0) printf("mysql insert error.\n");
        res = mysql_store_result(conn);
        mysql_free_result(res);





      }
      if (conn) mysql_close(conn);
      // download new rrs files we just insert in db
      if (!(dbexist)) loadrssfile(1);
    }
    //if (debugmode & 4) printf("* art = %s fpath=%s *\n",art,fpath);
    clean_stream_oversigt();                // clean old list
    strcpy(lasttmpfilename,"");    					// reset

    if (debugmode & 4) {
      printf("loading rss/stream data.\n");
      printf("art = %s tpath = %s \n",art,fpath);
    }

    if ((strcmp(art,"")==0) && (strcmp(fpath,"")==0)) {
      // select internetcontentarticles.feedtitle,
      sprintf(sqlselect,"select ANY_VALUE(internetcontentarticles.feedtitle) as feedtitle,ANY_VALUE(internetcontentarticles.path) as path,ANY_VALUE(internetcontentarticles.title) as title,ANY_VALUE(internetcontentarticles.description) as description,ANY_VALUE(internetcontentarticles.url) as url,ANY_VALUE(internetcontent.thumbnail),count(internetcontentarticles.feedtitle) as counter,ANY_VALUE(internetcontent.thumbnail) as thumbnail,ANY_VALUE(internetcontentarticles.time) as nroftimes,ANY_VALUE(internetcontentarticles.paththumb) from internetcontentarticles left join internetcontent on internetcontentarticles.feedtitle=internetcontent.name where mediaURL is NOT NULL group by (internetcontent.name) ORDER BY feedtitle,title");
      getart=0;
    } else if ((strcmp(art,"")!=0) && (strcmp(fpath,"")==0)) {
      sprintf(sqlselect,"select ANY_VALUE(feedtitle),ANY_VALUE(path),ANY_VALUE(title),ANY_VALUE(description),ANY_VALUE(url),ANY_VALUE(thumbnail),count(path),ANY_VALUE(paththumb),ANY_VALUE(mediaURL),ANY_VALUE(time) as nroftimes from internetcontentarticles where mediaURL is NOT NULL and feedtitle like '");
      strcat(sqlselect,art);
      strcat(sqlselect,"' GROUP BY title ORDER BY length(title),title ASC");
      getart=1;
    } else if ((strcmp(art,"")!=0) && (strcmp(fpath,"")!=0)) {
      sprintf(sqlselect,"select ANY_VALUE(feedtitle),ANY_VALUE(path),ANY_VALUE(title),ANY_VALUE(description),ANY_VALUE(url),ANY_VALUE(thumbnail),ANY_VALUE(paththumb),ANY_VALUE(time) as nroftimes from internetcontentarticles where mediaURL is NULL and feedtitle like '");
      strcat(sqlselect,art);
      strcat(sqlselect,"' AND path like '");
      strcat(sqlselect,fpath);
      strcat(sqlselect,"' ORDER BY length(title),title ASC");
      getart=2;
    }
    this->type=getart;					// husk sql type

    if (debugmode & 4) printf("RSS stream loader started... \n");

    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
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
                          if (row[7]) strncat(tmpfilename,row[7],20);                               //
                          strcpy(stack[antal]->feed_gfx_mythtv,tmpfilename);            	       		// mythtv icon file
                          if (row[9]) strcpy(tmpfilename,row[9]);
                          get_webfilenamelong(downloadfilename,tmpfilename);          // get file name from url
                          // check filename
                          strcpy(downloadfilename1,downloadfilename);                 // back name before change
                          int mmm=0;
                          while(mmm<strlen(downloadfilename)) {
                            if ((downloadfilename[mmm]=='?') || (downloadfilename[mmm]=='=')) downloadfilename[mmm]='_';
                            mmm++;
                          }
                          getuserhomedir(homedir);                                                  // get homedir
                          strcpy(downloadfilenamelong,homedir);
                          strcat(downloadfilenamelong,"/rss/images/");
                          strcat(downloadfilenamelong,downloadfilename);
                          if (!(file_exists(downloadfilenamelong))) {
                            if (debugmode & 4) printf("Downloadloading web file %s realname %s \n",tmpfilename,downloadfilename);
                            // download gfx file and use as icon
                            if (get_webfile2(tmpfilename,downloadfilenamelong)!=0) {
                              printf("Download error \n");
                            } else strcpy(tmpfilename,"");
                          }
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
                                // check filename
                                strcpy(downloadfilename1,downloadfilename);                 // back name before change
                                int mmm=0;
                                while(mmm<strlen(downloadfilename)) {
                                  if ((downloadfilename[mmm]=='?') || (downloadfilename[mmm]=='=')) downloadfilename[mmm]='_';
                                  mmm++;
                                }
                                strcpy(lasttmpfilename,tmpfilename);			              // husk file name
                                // save file in  user homedir rss/
                                getuserhomedir(homedir);                                  // get homedir
                                strcpy(downloadfilenamelong,homedir);
                                strcat(downloadfilenamelong,"/rss/images/");
                                strcat(downloadfilenamelong,downloadfilename);
                                if (!(file_exists(downloadfilenamelong))) {
                                  if (debugmode & 4) printf("Downloadloading web file %s realname %s \n",tmpfilename,downloadfilename);
                                  // download gfx file and use as icon
                                  if (get_webfile2(tmpfilename,downloadfilenamelong)!=0) {
                                    printf("Download error \n");
                                  } else strcpy(tmpfilename,"");
                                }
                                strcpy(tmpfilename,downloadfilenamelong);
                              }
                          } else strcpy(tmpfilename,"");
                          strncpy(stack[antal]->feed_gfx_mythtv,tmpfilename,200);	                // mythtv icon file
//                          strcpy(stack[antal]->feed_streamurl,row[4]);	// stream url
                          if (row[3]) strncpy(stack[antal]->feed_desc,row[3],feed_desclength);
                          stack[antal]->textureId=0;
                          // opdatere group antal
                          if (getart==1) stack[antal]->feed_group_antal=atoi(row[6]); else stack[antal]->feed_group_antal=0;
                          antal++;
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
    if (debugmode & 4) printf("Mythtv stream loader done... \n");
    return(0);
}


//
// thread web loader (loading all icons)
//

void *loadweb(void *data) {
  if (debugmode & 4) printf("Start web icon loader thread\n");
  streamoversigt.loadweb_stream_iconoversigt();
  if (debugmode & 4) printf("Stop web icon loader thread\n");
}



// downloading all web gfx

int stream_class::loadweb_stream_iconoversigt()

{
  int antal;
  int nr=0;
  int loadstatus;
  char tmpfilename[200];
  char downloadfilename[200];
  char downloadfilenamelong[200];
  char homedir[200];
  antal=this->streamantal();
  this->gfx_loaded=false;
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
        if (!(file_exists(downloadfilenamelong))) {
          if (debugmode & 4) printf("nr %3d Downloading : %s \n",nr,tmpfilename);
          loadstatus=get_webfile(tmpfilename,downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
        } else {
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
          //printf("nr %3d File exist %s \n",nr,downloadfilenamelong);
        }
      } else if (strncmp(tmpfilename,"https://",8)==0) {
        //strcpy(lastfile,downloadfilename);
        get_webfilename(downloadfilename,tmpfilename);
        getuserhomedir(downloadfilenamelong);
        // build path
        strcat(downloadfilenamelong,"/rss/images/");
        strcat(downloadfilenamelong,downloadfilename);
        if (!(file_exists(downloadfilenamelong))) {
          if (debugmode & 4) printf("nr %3d Downloading : %s \n",nr,tmpfilename);
          loadstatus=get_webfile2(tmpfilename,downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
        } else {
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
        }
      }
      // set recordnr loaded info to update users view
      this->stream_oversigt_nowloading=nr;
    }
    nr++;
  }
  this->gfx_loaded=true;
  return(1);
}


// load all streams gfx and save it
// used as thread
//

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
                    getuserhomedir(homedir);
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
    if (debugmode & 4) printf("End gfx thread loader\n ");
}



//
// play stream by vlc
//

void stream_class::playstream(char *url) {
//    media_is_playing=true;
    vlc_controller::playmedia(url);
}


void stream_class::show_stream_oversigt(GLuint normal_icon,GLuint empty_icon,GLuint empty_icon1,int _mangley,int stream_key_selected,bool do_update_rss_show)

{
    int j,ii,k,pos;
    int buttonsize=200;                                                         // button size
    float buttonsizey=160.0f;                                                   // button size
    float yof=orgwinsizey-(buttonsizey);                                        // start ypos
    float xof=0.0f;
    int lstreamoversigt_antal=9*6;
    int i=0;
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
      // error is in this IF block
      // stream har et icon in db

      if (i+1==(int) stream_key_selected) buttonsizey=170.0f;
      else buttonsizey=160.0f;

      if (stack[i+sofset]->textureId) {
        // stream icon
//        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,empty_icon1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-10, yof+buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-10, yof+10 , 0.0);
        glEnd();
//        glPopMatrix();

        glPushMatrix();
        // indsite draw icon rss gfx
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ONE);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        if (tema==5) {
          glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-10, yof+buttonsizey-20 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-10, yof+10 , 0.0);
        } else {
          glTexCoord2f(0, 0); glVertex3f( xof+20, yof+20, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+20,yof+buttonsizey-30, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-20, yof+buttonsizey-30 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-20, yof+20 , 0.0);
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
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        //glBindTexture(GL_TEXTURE_2D,normal_icon);
        glBindTexture(GL_TEXTURE_2D,empty_icon1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-10, yof+buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-10, yof+10 , 0.0);
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
        glScalef(14.0, 14.0, 1.0);
        glColor4f(1.0f, 1.0f, 1.0f,1.0f);
        sprintf(temptxt,"Feeds %-4d",stack[i+sofset]->feed_group_antal);
        glcRenderString(temptxt);
        glPopMatrix();
      }

      // show text of element
      float fontsiz=15.0f;
      glPushMatrix();
      pline=0;
      glTranslatef(xof+20,yof-10,0);
      glDisable(GL_TEXTURE_2D);
      glScalef(fontsiz, fontsiz, 1.0);
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glRasterPos2f(0.0f, 0.0f);
      glDisable(GL_TEXTURE_2D);
      strcpy(temptxt,stack[i+sofset]->feed_showtxt);        // text to show
      base=temptxt;
      length=strlen(temptxt);
      width = 20;
      bool stop=false;
      while(*base) {
        if(length <= width) {
          glcRenderString(base);
          pline++;
          glTranslatef(0.0f-(strlen(base)/1.6f)+2,-pline*1.2f,0.0f);
          //puts(base);                                       // display string
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
        glTranslatef(0.0f-(strlen(base)/1.6f)+2,-pline*1.2f,0.0f);
        //puts(base);
        length -= right_margin-base+1;                         // +1 for the space
        base = right_margin+1;
        if (pline>=2) break;
      }
      glPopMatrix();
      i++;
      xof+=(buttonsize+10);
    }
    // show rss file loading status
    if (do_update_rss_show) {
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      glBindTexture(GL_TEXTURE_2D,_textureIdloading1);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(1470+200, 75 , 0.0);
      glTexCoord2f(0, 1); glVertex3f(1470+200, 75+130, 0.0);
      glTexCoord2f(1, 1); glVertex3f(1470+200+250, 75+130 , 0.0);
      glTexCoord2f(1, 0); glVertex3f(1470+200+250, 75 , 0.0);
      glEnd();

      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(1680+6,140,0);
      glScalef(24.0, 24.0, 1.0);
      glColor3f(0.6f, 0.6f, 0.6f);
      sprintf(temptxt,"FEED IS LOADING");
      glcRenderString(temptxt);
      glPopMatrix();


      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(1680+60,95,0);
      glScalef(24.0, 24.0, 1.0);
      glColor3f(0.6f, 0.6f, 0.6f);
      sprintf(temptxt,"%4d",streamoversigt.stream_rssparse_nowloading);
      glcRenderString(temptxt);
      glPopMatrix();
    }

    // show icon gfx loader status
    // do not show while loading new rss
    if ((do_update_rss_show==false) && (stream_oversigt_loaded_nr<streamoversigt.streamantal())) {
      // show radio icon loader status
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      glBindTexture(GL_TEXTURE_2D,_textureIdloading1);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(1470+200, 75 , 0.0);
      glTexCoord2f(0, 1); glVertex3f(1470+200, 75+130, 0.0);
      glTexCoord2f(1, 1); glVertex3f(1470+200+250, 75+130 , 0.0);
      glTexCoord2f(1, 0); glVertex3f(1470+200+250, 75 , 0.0);
      glEnd();

      glPushMatrix();
      glDisable(GL_TEXTURE_2D);

      glTranslatef(1680+20,140,0);
      glScalef(24.0, 24.0, 1.0);
      glColor3f(0.6f, 0.6f, 0.6f);
      sprintf(temptxt,"Loading icons");
      glcRenderString(temptxt);
      glPopMatrix();

      glPushMatrix();
      glTranslatef(1680+20,95,0);
      glScalef(24.0, 24.0, 1.0);
      glColor3f(0.6f, 0.6f, 0.6f);
      sprintf(temptxt,"%4d of %4d ",stream_oversigt_loaded_nr,streamoversigt.streamantal());
      glcRenderString(temptxt);
      glPopMatrix();
    }
    // no records loaded error
    if ((i==0) && (do_update_rss_show==false)) {
      strcpy(temptxt,"No backend ip/hostname ");
      strcat(temptxt,configmysqlhost);
      glPushMatrix();
      xof=700;
      yof=400;
      glTranslatef(xof+10, yof+40 ,0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glDisable(GL_TEXTURE_2D);
      glScalef(20.0, 20.0, 1.0);
      glcRenderString(temptxt);
      glEnable(GL_TEXTURE_2D);
      glPopMatrix();
    }
}
