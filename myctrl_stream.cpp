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

#include "myctrl_stream.h"
#include "utility.h"
#include "myth_ttffont.h"
#include "utility.h"
#include "readjpg.h"
#include "loadpng.h"
// web file loader
#include "myctrl_readwebfile.h"

extern char *dbname;                                           // internal database name in mysql (music,movie,radio)
extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];
extern int configmythtvver;
extern int screen_size;
extern int screensizey;
extern int screeny;
extern int debugmode;
extern unsigned int musicoversigt_antal;
extern int stream_key_selected;
extern int stream_select_iconnr;
extern int do_stream_icon_anim_icon_ofset;
extern GLuint radiooptions,radiooptionsmask;			//
extern GLuint _textureIdback;  					// back icon
extern int fonttype;
extern fontctrl aktivfont;
extern int orgwinsizey,orgwinsizex;

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
}



// destructor
stream_class::~stream_class() {
    clean_stream_oversigt();
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

int stream_class::loadrssfile() {
  // mysql vars
  char sqlselect[2048];
  char totalurl[2048];
  char parsefilename[2048];
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char *database = (char *) "mythconverg";
  conn=mysql_init(NULL);
  // Connect to database
  //strcpy(sqlselect,"select internetcontent.name,internetcontentarticles.path,internetcontentarticles.title,internetcontentarticles.description,internetcontentarticles.url,internetcontent.thumbnail,count(internetcontentarticles.feedtitle),internetcontent.thumbnail from internetcontentarticles left join internetcontent on internetcontentarticles.feedtitle=internetcontent.name group by internetcontentarticles.feedtitle");
  //strcpy(sqlselect,"select * from internetcontentarticles");
  strcpy(sqlselect,"select * from internetcontentarticles where mediaURL is NULL");
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        printf("Hent info om stream title %10s \n",row[0]);
        if (strcmp(row[3],"")!=0) {
          strcpy(totalurl,"wget '");
          if (row[7]) strcat(totalurl,row[7]); else if (row[3]) strcat(totalurl,row[3]);
          strcat(totalurl,"' -o '/home/hans/rss/wget.log");
          strcat(totalurl,"' -O '/home/hans/rss/");
          if (row[3]) strcat(totalurl,row[3]);
          strcat(totalurl,".rss'");
          system(totalurl);
          // parse file
          strcpy(parsefilename,"/home/hans/rss/");
          strcat(parsefilename,row[3]);
          strcat(parsefilename,".rss");
          if (strcmp(row[3],"")!=0) {
            // parse downloaded xmlfile now (create db records)
            parsexmlrssfile(parsefilename);
          }
        }
      }
      mysql_free_result(res);
    }
    mysql_close(conn);
  } else return(-1);
  return(1);
}


//
// xml parser

int stream_class::parsexmlrssfile(char *filename) {
  xmlChar *tmpdat;
  xmlDoc *document;
  xmlNode *root, *first_child, *node, *node1 ,*subnode,*subnode2;
  xmlChar *xmlrssid;
  xmlChar *content;
  char rssprgtitle[2048];
  char rssprgfeedtitle[2048];
  char rssprgdesc[2048];
  char rssvideolink[2048];
  char rsstime[2048];
  char rssauthor[2048];
  int rssepisode;
  int rssseason;
  char result[2048+1];
  char sqlinsert[32768];
  char *database = (char *) "mythconverg";
  bool recordexist=false;
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  strcpy(rssprgtitle,"");
  strcpy(rssprgfeedtitle,"");
  strcpy(rssprgdesc,"");
  strcpy(rssvideolink,"");
  strcpy(rsstime,"");
  strcpy(rssauthor,"");
  conn=mysql_init(NULL);
  document = xmlReadFile(filename, NULL, 0);            // open xml file
  // if exist do all the parse and update db
  // it use REPLACE in mysql to create/update records if changed in xmlfile
  if ((document) && (conn)) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    root = xmlDocGetRootElement(document);
    first_child = root->children;
    for (node = first_child; node; node = node->next) {
      if (node->type==XML_ELEMENT_NODE) {
        if (strcmp((char *) node->name,"channel")==0) {
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
            if ((content) && (strcmp((char *) subnode->name,"item")==0)) {
              subnode2=subnode->xmlChildrenNode;
              while(subnode2) {
                if ((content) && (strcmp((char *) subnode2->name,"title")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  strcpy(rssprgfeedtitle,(char *) content);
                }
                if ((content) && (strcmp((char *) subnode2->name,"enclosure")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  tmpdat=xmlGetProp(subnode2,( xmlChar *) "url");
                  if (tmpdat) strcpy(rssvideolink,(char *) tmpdat);
                }
                if ((content) && (strcmp((char *) subnode2->name,"duration")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  if (content) strcpy(rsstime,(char *) content);
                }

                if ((content) && (strcmp((char *) subnode2->name,"episode")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  if (content) rssepisode=atoi((char *) content);
                }
                if ((content) && (strcmp((char *) subnode2->name,"season")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  if (content) rssseason=atoi((char *) content);
                }

                if ((content) && (strcmp((char *) subnode2->name,"author")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  if (content) strcpy(rssauthor,(char *) content);
                }

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
              // check if exist
              if (strcmp(rssvideolink,"")!=0) {
                recordexist=false;
                sprintf(sqlinsert,"select feedtitle from internetcontentarticles where (feedtitle like '%s' and mediaURL like '%s' and title like '%s' and episode=%d and season=%d and author like '%s' and path like '%s' and description like '%s')",rssprgtitle,rssvideolink,rssprgfeedtitle,rssepisode,rssseason,rssauthor,"",rssprgdesc);
                mysql_query(conn,sqlinsert);
                res = mysql_store_result(conn);
                if (res) {
                  while ((row = mysql_fetch_row(res)) != NULL) {
                    recordexist=true;
                  }
                }
                // creoate record if not exist
                if (!(recordexist)) {
                  sprintf(sqlinsert,"REPLACE into internetcontentarticles(feedtitle,mediaURL,title,episode,season,author,path,description) values('%s','%s','%s',%d,%d,'%s','%s','%s')",rssprgtitle,rssvideolink,rssprgfeedtitle,rssepisode,rssseason,rssauthor,"",rssprgdesc);
                  //printf("sql=%s\n",sqlinsert);
                  mysql_query(conn,sqlinsert);
                  res = mysql_store_result(conn);
                }
              }
            }
            subnode=subnode->next;
          }
          printf("\n");
        }
      }
    }
    xmlFreeDoc(document);
    mysql_close(conn);
  } else {
    printf(" ******************************************* Read error on xmlfile downloaded to rss dir\n");
  }
}




// load felt 7 = mythtv gfx icon

int stream_class::opdatere_stream_oversigt(char *art,char *fpath) {
    char sqlselect[1024];
    char tmpfilename[1024];
    char lasttmpfilename[1024];
    char downloadfilename[1024];
    char downloadfilenamelong[1024];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *database = (char *) "mythconverg";
    bool online;
    int getart=0;
    bool loadstatus=true;
    bool dbexist=false;
    antal=0;
    conn=mysql_init(NULL);
    // Connect to database
    if (conn) {
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      // test fpom musik table exist
      mysql_query(conn,"SELECT feedtitle from internetcontentarticles limit 1");
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          dbexist=1;
        }
      }
      if (!(dbexist)) {
        if (debugmode & 4) printf("Creating database for rss feed\n");
        // thumbnail   = name of an local image file
        // commandline = Program to fetch content with
        // updated     = Time of last update
        sprintf(sqlselect,"create table internetcontentarticles(feedtitle varchar(255),path text,paththumb text,title varchar(255),season smallint(5) DEFAULT 0,episode smallint(5) DEFAULT 0,description text,url text,type smallint(3),thumbnail text,mediaURL text,author varchar(255),date datetime,time int(11),rating varchar(255),filesize bigint(20),player varchar(255),playerargs text,download varchar(255),downloadargs text,width smallint(6),height smallint(6),language  varchar(128),podcast tinyint(1),downloadable tinyint(1),customhtml tinyint(1),countries varchar(255))");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        mysql_free_result(res);

        sprintf(sqlselect,"create table internetcontent(name varchar(255),thumbnail varchar(255),type smallint(3),author varchar(128),description text,commandline text,version double,updated datetime,search  tinyint(1),tree tinyint(1),podcast tinyint(1),download tinyint(1),host varchar(128))");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        mysql_free_result(res);
      }
      mysql_close(conn);
    }

    printf("****************************************************** art = %s fpath=%s \n",art,fpath);

    clean_stream_oversigt();                // clean old list
    strcpy(lasttmpfilename,"");    					// reset
    if (debugmode & 4) printf("loading stream mythtv data.\n");

    if ((strcmp(art,"")==0) && (strcmp(fpath,"")==0)) {
      // select internetcontentarticles.feedtitle,
//      sprintf(sqlselect,"select internetcontent.name,internetcontentarticles.path,internetcontentarticles.title,internetcontentarticles.description,internetcontentarticles.url,internetcontent.thumbnail,count(internetcontentarticles.feedtitle),internetcontentarticles.paththumb from internetcontentarticles left join internetcontent on internetcontentarticles.feedtitle=internetcontent.name group by internetcontentarticles.feedtitle");
      sprintf(sqlselect,"select ANY_VALUE(internetcontentarticles.feedtitle) as feedtitle,ANY_VALUE(internetcontentarticles.path) as path,ANY_VALUE(internetcontentarticles.title) as title,ANY_VALUE(internetcontentarticles.description) as description,ANY_VALUE(internetcontentarticles.url) as url,ANY_VALUE(internetcontent.thumbnail),count(internetcontentarticles.feedtitle) as counter,ANY_VALUE(internetcontent.thumbnail) as thumbnail from internetcontentarticles left join internetcontent on internetcontentarticles.feedtitle=internetcontent.name where mediaURL is NOT NULL group by (internetcontent.name)");
      getart=0;
    }
    if ((strcmp(art,"")!=0) && (strcmp(fpath,"")==0)) {
      sprintf(sqlselect,"select ANY_VALUE(feedtitle),ANY_VALUE(path),ANY_VALUE(title),ANY_VALUE(description),ANY_VALUE(url),ANY_VALUE(thumbnail),count(path),ANY_VALUE(paththumb),ANY_VALUE(mediaURL) from internetcontentarticles where mediaURL is NOT NULL and feedtitle like '");
      strcat(sqlselect,art);
      //strcat(sqlselect,"' group by path order by path,title asc");
      strcat(sqlselect,"' group by title order by title asc");
      getart=1;
    } else if ((strcmp(art,"")!=0) && (strcmp(fpath,"")!=0)) {
      sprintf(sqlselect,"select ANY_VALUE(feedtitle),ANY_VALUE(path),ANY_VALUE(title),ANY_VALUE(description),ANY_VALUE(url),ANY_VALUE(thumbnail),ANY_VALUE(paththumb) from internetcontentarticles where mediaURL is NULL and feedtitle like '");
      strcat(sqlselect,art);
      strcat(sqlselect,"' and path like '");
      strcat(sqlselect,fpath);
      strcat(sqlselect,"' ORDER BY title ASC");
      getart=2;
    }
    this->type=getart;					// husk sql type

    printf("Mythtv stream loader started... \n");

    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        if (res) {
            while (((row = mysql_fetch_row(res)) != NULL) && (antal<maxantal)) {
                printf("Hent info om stream nr %d %-20s\n",antal,row[2]);
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

                        if (getart==0) {
                          strncpy(stack[antal]->feed_showtxt,row[0],feed_pathlength);
                          strncpy(stack[antal]->feed_name,row[0],feed_namelength);
                          if (row[1]) strcpy(stack[antal]->feed_path,row[1]);
                          if (row[6]) stack[antal]->feed_group_antal=atoi(row[6]);				          // antal
                          if (row[3]) strncpy(stack[antal]->feed_desc,row[3],feed_desclength);
                          if (row[7]) strncat(tmpfilename,row[7],20);                               //
                          strcpy(stack[antal]->feed_gfx_mythtv,tmpfilename);            	       		// mythtv icon file
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
                            case 0: if (row[6]) strcpy(tmpfilename,row[6]);
                                    break;
                            case 1: if (row[7]) strcpy(tmpfilename,row[7]);
                                    break;
                            case 2: if (row[5]) strcpy(tmpfilename,row[5]);
                                    break;
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
                                // downloadfilename = name on file, from tmpfilename = full web url
                                printf("download gfx icon data: %s \n",tmpfilename);
                                get_webfilename(downloadfilename,tmpfilename);
                                strcpy(lasttmpfilename,tmpfilename);			// husk file name
                                // save file in  /usr/share/mythtv-controller/images/mythnetvision
                                strcpy(downloadfilenamelong,"/home/hans/rss/");
                                strcat(downloadfilenamelong,downloadfilename);

//                                if ((strcmp(lasttmpfilename,tmpfilename)==0) && (antal>1)) loadstatus=false;

/*


                                if ((loadstatus) && (!(file_exists(downloadfilenamelong)))) {
//	                                printf("Loading web file %s realname %s \n",tmpfilename,downloadfilename);
                                  // download image file to /usr/share/mythtv-controller/images/mythnetvision

                                  if ((strcmp(lasttmpfilename,tmpfilename)==false) && (loadstatus=true)) {
                                    if (loadstatus=get_webfile(tmpfilename,downloadfilenamelong)) {
                                      strcpy(tmpfilename,downloadfilenamelong);
                                    } else {
                                      // fejl load
                                      strcpy(tmpfilename,"");
                                    }
                                  }
                                } else strcpy(tmpfilename,downloadfilenamelong);
                                */


/*
                                // or use wget
                                strcpy(tmpfilename1,"wget ");
                                strcat(tmpfilename1,tmpfilename);
                                strcat(tmpfilename1," tmp/");
                                system(tmpfilename1);
*/
                              }
                          } else strcpy(tmpfilename,"");
                          strncpy(stack[antal]->feed_gfx_mythtv,tmpfilename,200);	// mythtv icon file
//                          strcpy(stack[antal]->feed_streamurl,row[4]);	// stream url
                          if (row[3]) strncpy(stack[antal]->feed_desc,row[3],feed_desclength);
                          stack[antal]->textureId=0;
                          // opdatere group antal
                          if (getart==1) stack[antal]->feed_group_antal=atoi(row[6]); else stack[antal]->feed_group_antal=0;
                          antal++;
                        }


//                        if (debugmode & 4) printf("Gem texture filename %s \n",tmpfilename);
                    }
                }
            }
            mysql_close(conn);
        } else {
            printf("No stream data loaded \n");

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

//        loadweb_stream_iconoversigt();

        return(antal-1);
    } else printf("Failed to update feed stream db, can not connect to database: %s Error: %s\n",dbname,mysql_error(conn));
    printf("Mythtv stream loader done... \n");
    return(0);
}


//
// thread web loader
//

void *loadweb(void *data) {
  printf("Start web loader thread\n");
  streamoversigt.loadweb_stream_iconoversigt();
  printf("Stop web loader thread\n");
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
        strcpy(downloadfilenamelong,"/usr/share/mythtv-controller/images/mythnetvision/");
        strcat(downloadfilenamelong,downloadfilename);
        if (!(file_exists(downloadfilenamelong))) {

          if (debugmode) printf("nr %3d Downloading : %s \n",nr,tmpfilename);

          loadstatus=get_webfile(tmpfilename,downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
        } else {
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
          printf("nr %3d File exist %s \n",nr,downloadfilenamelong);
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
    char *database = (char *) "mythconverg";
    char sqlselect[1024];
    char sqlselect1[1024];
    char downloadfilename[1024];
    char downloadfilenamelong[1024];
    char tmpfilename[1024];
    char lastfile[1024];
    bool online;
    int getart=0;
    bool loadstatus;
    bool filechange;
    int antal=0;
    int total_antal=0;
    int nr;

    printf("Start gfx thread loader\n ");
    strcpy(lastfile,"");
    strcpy(sqlselect,"select ANY_VALUE(internetcontent.name),ANY_VALUE(internetcontentarticles.path),count(internetcontentarticles.feedtitle),ANY_VALUE(internetcontent.thumbnail) from internetcontentarticles left join internetcontent on internetcontentarticles.feedtitle=internetcontent.name group by internetcontentarticles.feedtitle");
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        printf("\n\nLoading RSS gfx..\n\n");
        if (res) {
            while ((row = mysql_fetch_row(res)) != NULL) {
              sprintf(sqlselect1,"select feedtitle,path,title,description,url,thumbnail,path,paththumb from internetcontentarticles where feedtitle like '%s' order by path,title asc",row[0]);
              mysql_query(conn,"set NAMES 'utf8'");
              res1 = mysql_store_result(conn);
              mysql_query(conn,sqlselect1);
              res1 = mysql_store_result(conn);
              if (res1) {
                antal=0;
                printf("Found : %40s ",row[1]);
                nr=0;
                while ((row1 = mysql_fetch_row(res1)) != NULL) {
                  antal++;
                  strcpy(tmpfilename,row1[5]);

                  if (stream_loadergfx_started_break) break;

                  if (strncmp(tmpfilename,"http://",7)==0) {

                    strcpy(lastfile,downloadfilename);

                    get_webfilename(downloadfilename,tmpfilename);
                    strcpy(downloadfilenamelong,"/usr/share/mythtv-controller/images/mythnetvision/");
                    strcat(downloadfilenamelong,downloadfilename);

                    filechange=strcmp(lastfile,downloadfilename);

                    if ((!(file_exists(downloadfilenamelong))) && (filechange)) {
                      printf("nr %3d Downloading : %s \n",nr,tmpfilename);
                      loadstatus=get_webfile(tmpfilename,downloadfilenamelong);
                      nr++;
                    } else {
                      printf("nr %3d exist : %s \n",nr,tmpfilename);
                    }
                    //this->stream_oversigt_nowloading=total_antal;
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
     printf("End gfx thread loader\n ");
}



void stream_class::playstream(char *url) {
    //char path[PATH_MAX];                                  // max path length from os
    //strcpy(path,"");
    //film_is_playing=true;
    //strcat(path,this->filmoversigt[nr].getfilmfilename());
    vlc_controller::playmedia(url);
}


void stream_class::show_stream_oversigt1(GLuint normal_icon,GLuint empty_icon,int _mangley)

{
    int j,ii,k,pos;
    int buttonsize=200;
    float buttonsizey=160.0f;
    float yof=orgwinsizey-(buttonsizey);         // start ypos
    float xof=0.0f;
    int lstreamoversigt_antal=9*6;
    int i=0;
//    float ofs=0.0f;		// used to calc the text length
//    char *lastslash;
//    float xvgaz=0.0f;
//    char temptxt[200];
    unsigned int sofset=0;
    int bonline=8;                // antal pr linie
//    float buttonzoom=0.0f;
    float boffset;
//    int loader_xpos,loader_ypos;
//    char tmpfilename[200];
    char gfxfilename[200];
    char downloadfilename[200];
    char downloadfilenamelong[1024];
    char *gfxshortnamepointer;
    char gfxshortname[200];
    char temptxt[200];
    char word[200];
    int antal_loaded=0;
    static int stream_oversigt_loaded_done=0;
    GLuint texture;
    if ((this->streamantal()) && (stream_oversigt_loaded==false) && (this->stream_oversigt_loaded_nr<this->streamantal())) {

      if (stack[stream_oversigt_loaded_nr]) strcpy(gfxfilename,stack[stream_oversigt_loaded_nr]->feed_gfx_mythtv);
      else strcpy(gfxfilename,"");

      if (debugmode) printf(".");

      strcpy(gfxshortname,"");
      gfxshortnamepointer=strrchr(gfxfilename,'.');     // get last char = type of file
      if (gfxshortnamepointer) {
        strcpy(gfxshortname,gfxshortnamepointer);
      }
      if (strcmp(gfxfilename,"")!=0) {
        // check om der findes en downloaded icon
        strcpy(downloadfilenamelong,"");
        strcat(downloadfilenamelong,gfxfilename);
        // check om filen findes i cache dir eller i mythtv netvision dir
        if (file_exists(gfxfilename)) {
          texture=loadTexture ((char *) gfxfilename);
          if (texture) set_texture(stream_oversigt_loaded_nr,texture);
          antal_loaded+=1;
        } else if (file_exists(downloadfilenamelong)) {
           texture=loadTexture ((char *) downloadfilenamelong);
           if (texture) set_texture(stream_oversigt_loaded_nr,texture);
           antal_loaded+=1;
        } else texture=0;
      }
      if (stream_oversigt_loaded_nr==this->streamantal()) {
        stream_oversigt_loaded=true;
        stream_oversigt_loaded_done=true;
      } else stream_oversigt_loaded_nr++;
    }

    if (!(gfx_loaded)) {
      stream_oversigt_loaded_nr=0;
      stream_oversigt_loaded=false;
    }
    while((i<lstreamoversigt_antal) && (i+sofset<antal) && (stack[i+sofset]!=NULL)) {
      if (((i % bonline)==0) && (i>0)) {
        yof=yof-(buttonsizey+20);
        xof=0;
      }
      // stream har et icon in db
      if (stack[i+sofset]->textureId) {
        // stream icon
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,empty_icon);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-10, yof+buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-10, yof+10 , 0.0);
        glEnd();
        glPopMatrix();

        glPushMatrix();
        // indsite draw icon
        glEnable(GL_TEXTURE_2D);
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof+20, yof+20, 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof+20,yof+buttonsizey-30, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-20, yof+buttonsizey-30 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-20, yof+20 , 0.0);
        glEnd();
        glPopMatrix();
      } else {
/*
        // stream default icon
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,icon_mask);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsizey, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize, yof+buttonsizey , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize,yof , 0.0);
        glEnd();
        glPopMatrix();
*/
        // show default icon
        glPushMatrix();
        // indsite draw radio station icon
        glEnable(GL_TEXTURE_2D);
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,normal_icon);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-10, yof+buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-10, yof+10 , 0.0);
        glEnd();
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

      float fontsiz=16.0f;

      glPushMatrix();
      strcpy(temptxt,stack[i+sofset]->feed_showtxt);        // text to show
      //glScalef(14.0, 14.0, 1.0);
      glTranslatef(xof+20,yof-10,0);
      glDisable(GL_TEXTURE_2D);

      glScalef(fontsiz, fontsiz, 1.0);
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);

      // temp
      glRasterPos2f(0.0f, 0.0f);
      glDisable(GL_TEXTURE_2D);
      temptxt[17]='\0';
      glcRenderString(temptxt);

/*
      if (strlen(temptxt)<=14) {
        glRasterPos2f(0.0f, 0.0f);
        glDisable(GL_TEXTURE_2D);
        glcRenderString(temptxt);
      } else {
        j=0;
        ii=0;
        while(!isspace(temptxt[ii])) {
          if (temptxt[ii]=='\0') break;
          word[j]=temptxt[ii];
          ii++;
          j++;
        }
        word[j]='\0';	// j = word length
        pos=0;
        if (j>13) {		// print char by char
          k=0;
          while(word[k]!='\0') {
            if (pos>=13) {
              glcRenderChar('-');
              //glRasterPos2f(0.0f, 0.0f);
              //glScalef(20.0, 20.0, 1.0);
            }
            glcRenderChar(word[k]);
            pos++;
            k++;
          }
        } else {
          if (pos+j>13) {	// word doesn't fit line
            pos=0;
            //ofs=(int) (strlen(word)/2)*9;
            //glRasterPos2f(0.0f, 0.0f);
            //glScalef(14.0, 14.0, 1.0);
          }
          glcRenderString(word);
          pos+=j;
        }
      }
*/
      glPopMatrix();
      i++;
      xof+=(buttonsize+10);
    }

    // show loader status
    if (stream_oversigt_loaded_nr<streamoversigt.streamantal()) {
      // show radio icon loader status
      glEnable(GL_TEXTURE_2D);
      //glBlendFunc(GL_ONE, GL_ONE);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      glBindTexture(GL_TEXTURE_2D,_textureIdloading);
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
      glTranslatef(1680+20,95,0);
      glScalef(24.0, 24.0, 1.0);
      glColor3f(0.6f, 0.6f, 0.6f);
      sprintf(temptxt,"%4d of %4d ",stream_oversigt_loaded_nr,streamoversigt.streamantal());
      glcRenderString(temptxt);
      glPopMatrix();
    }
    if (i==0) {
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

































































































void stream_class::show_stream_oversigt(GLuint normal_icon,GLuint icon_mask,GLuint empty_icon,int _mangley)

{

  int lstreamoversigt_antal;
  int i=0;
  float ofs=0.0f;		// used to calc the text length
  float yof=0.0f;
  float xof=0.0f;
//  char *lastslash;
  float xvgaz=0.0f;
  char temptxt[200];
  unsigned int sofset=0;
  int bonline;
  float buttonsizex=68.0f;
  float buttonsizey=58.0f;
  float buttonzoom=0.0f;
  float boffset;
  int loader_xpos,loader_ypos;
//  char tmpfilename[200];
  char gfxfilename[200];
  char downloadfilename[200];
  char downloadfilenamelong[1024];
  char *gfxshortnamepointer;
  char gfxshortname[200];
//  char tmp[200];
  static int stream_oversigt_loaded_done=0;
  GLuint texture;

  static int antal_loaded=0;
  bool finish=false;

  if ((this->streamantal()) && (stream_oversigt_loaded==false) && (this->stream_oversigt_loaded_nr<this->streamantal())) {

      if (stack[stream_oversigt_loaded_nr]) strcpy(gfxfilename,stack[stream_oversigt_loaded_nr]->feed_gfx_mythtv);
      else strcpy(gfxfilename,"");

      strcpy(gfxshortname,"");
      gfxshortnamepointer=strrchr(gfxfilename,'.');	// get last char = type of file
      if (gfxshortnamepointer) {
        strcpy(gfxshortname,gfxshortnamepointer);
      }

      if (strcmp(gfxfilename,"")!=0) {
        // check om der findes en downloaded icon
        strcpy(downloadfilenamelong,"");
        strcat(downloadfilenamelong,gfxfilename);
        // check om filen findes i cache dir eller i mythtv netvision dir
        if (file_exists(gfxfilename)) {
          texture=loadTexture ((char *) gfxfilename);
          if (texture) set_texture(stream_oversigt_loaded_nr,texture);
          antal_loaded+=1;
        } else if (file_exists(downloadfilenamelong)) {
          texture=loadTexture ((char *) downloadfilenamelong);
          if (texture) set_texture(stream_oversigt_loaded_nr,texture);
          antal_loaded+=1;
        } else texture=0;
      }

      //if (debugmode & 4) printf("load mythtv named %s or %s \n", gfxfilename,downloadfilenamelong);

      if (stream_oversigt_loaded_nr==this->streamantal()) {
        stream_oversigt_loaded=true;
        stream_oversigt_loaded_done=true;
      } else stream_oversigt_loaded_nr++;
  }

  // check om vi har loaded alle images in thread
  //if (antal_loaded>=this->streamantal()) finish=true; else finish=false;

//	    printf("gfxloaded %d \n",gfx_loaded);

  if (!(gfx_loaded)) {
    stream_oversigt_loaded_nr=0;
    stream_oversigt_loaded=false;
  }




  switch(screen_size) {
      case 1: xof=-370.0f;
              yof= 260.0f;
              sofset=(_mangley/41)*5;
              bonline=5;					// numbers of icons in x direction
              xvgaz=-800.0f;					// 800
              lstreamoversigt_antal=5*5;			// numbers of icons in y direction
              buttonsizex=88.0f;				// 68
              buttonsizey=68.0f;
              break;
      case 2: xof=-380.0f;
              yof= 270.0f;				// 250.0f
              sofset=(_mangley/41)*5;
              bonline=5;
              xvgaz=-850.0f;
              lstreamoversigt_antal=6*4;
              buttonsizex=88.0f;
              buttonsizey=78.0f;
              break;
      case 3: xof=-660.0f;			// mode 3 1080p
              yof= 336.0f;
              sofset=(_mangley/41)*9;
              bonline=9;
              xvgaz=-1000.0f;			// -1000
              lstreamoversigt_antal=9*6;
              buttonsizex=84.0f+5;		// 84
              buttonsizey=74.0f+5;		// 74
              break;
      case 4: xof=-660.0f;
              yof= 336.0f;
              sofset=(_mangley/41)*8;
              bonline=8;
              xvgaz=-1000.0f;
              lstreamoversigt_antal=8*5;
              buttonsizex=96.0f;
              buttonsizey=86.0f;
              break;
      default:
              xof=-380.0;
              yof= 250.0f;
              sofset=(_mangley/41)*5;
              bonline=5;
              xvgaz=-800.0f;
              lstreamoversigt_antal=5*4;
              buttonsizex=68.0f;
              buttonsizey=58.0f;
              break;
  }
  boffset=buttonsizey*1.4;
  //
  // hvis der ikke brugfes nvidia core ret størelser
  //
  if (cur_avail_mem_kb==0) {
      buttonsizex-=20.0f;
      buttonsizey-=20.0f;
  }

  // viser det antal streams som kan være på skærmen på en gang
  // ellers er der et start ofset (sofset) som beskriver start ofset fra array (bliver rettet andet sted) pilup/pildown osv osv
  while((i<lstreamoversigt_antal) && ((int) i+(int) sofset<(int) antal) && (stack[i+sofset]!=NULL)) {
    switch(screen_size) {
      case 1:
        if (((i % bonline)==0) && (i>0)) {
          yof=yof-(boffset+30);
          xof=-370;
        }
        break;
      case 2:
        if (((i % bonline)==0) && (i>0)) {
          yof=yof-(boffset+30);
          xof=-380;
        }
        break;
      case 3:
        if (((i % bonline)==0) && (i>0)) {
          yof=yof-(boffset+30);
          xof=-660;
        }
        break;
      case 4:
        if (((i % bonline)==0) && (i>0)) {
          yof=yof-(boffset+30);
          xof=-660;
        }
        break;
    }


    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    // draw mask
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    //glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glColor4f(1.0f, 1.0f, 1.0f,1.0f);

    if (stack[i+sofset]->textureId) {
      glBindTexture(GL_TEXTURE_2D,icon_mask);					// stack[i+sofset]->textureId
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
      glBindTexture(GL_TEXTURE_2D,icon_mask);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glTranslatef(xof,yof,xvgaz);
    glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-(buttonsizex+buttonzoom), -(buttonsizey+buttonzoom), 0.0);
    glTexCoord2f(0, 1); glVertex3f(-(buttonsizex+buttonzoom),  buttonsizey+buttonzoom, 0.0);
    glTexCoord2f(1, 1); glVertex3f( buttonsizex+buttonzoom,  buttonsizey+buttonzoom, 0.0);
    glTexCoord2f(1, 0); glVertex3f( buttonsizex+buttonzoom, -(buttonsizey+buttonzoom), 0.0);
    glEnd(); //End quadrilateral coordinates


    glLoadIdentity();
    glBlendFunc(GL_ONE, GL_ONE);
    glColor4f(1.0f, 1.0f, 1.0f,1.0f);

    // stream har et icon in db
    if (stack[i+sofset]->textureId) {
        // draw icon no indhold
        glBindTexture(GL_TEXTURE_2D,empty_icon);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // zoom valgte
        if (i+1==(int) stream_key_selected) buttonzoom=18.0f;
        else buttonzoom=12.0f;					// default 0.0f

        glTranslatef(xof,yof,xvgaz);
        glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-(buttonsizex+buttonzoom), -(buttonsizey+buttonzoom), 0.0);
        glTexCoord2f(0, 1); glVertex3f(-(buttonsizex+buttonzoom),  buttonsizey+buttonzoom, 0.0);
        glTexCoord2f(1, 1); glVertex3f( buttonsizex+buttonzoom,  buttonsizey+buttonzoom, 0.0);
        glTexCoord2f(1, 0); glVertex3f( buttonsizex+buttonzoom, -(buttonsizey+buttonzoom), 0.0);
        glEnd(); //End quadrilateral coordinates

/*
        //
        // draw icon mask
        //
        glLoadIdentity();
        glBindTexture(GL_TEXTURE_2D,onlinestreammask);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glBlendFunc(GL_ONE, GL_ONE);
        // To be able to render transparent PNGs, you have to enable blending like this
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        glBlendFunc(GL_DST_COLOR, GL_ZERO);
        // zoom valgte
        if (i+1==(int) stream_key_selected) buttonzoom=0.0f;
        else buttonzoom=-12.0f;

        glTranslatef(xof,yof,xvgaz);
        glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-(buttonsizex+buttonzoom), -(buttonsizey+buttonzoom), 0.0);
        glTexCoord2f(0, 1); glVertex3f(-(buttonsizex+buttonzoom),  buttonsizey+buttonzoom, 0.0);
        glTexCoord2f(1, 1); glVertex3f( buttonsizex+buttonzoom,  buttonsizey+buttonzoom, 0.0);
        glTexCoord2f(1, 0); glVertex3f( buttonsizex+buttonzoom, -(buttonsizey+buttonzoom), 0.0);
        glEnd(); //End quadrilateral coordinates
*/

        //
        // draw icon indhold
        //
        glLoadIdentity();
        glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBlendFunc(GL_ONE, GL_ONE);
        // To be able to render transparent PNGs, you have to enable blending like this
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // zoom valgte
        if (i+1==(int) stream_key_selected) buttonzoom=18.0f;		// 0.0f
        else buttonzoom=12.0f;		// -12.0f

        glTranslatef(xof,yof,xvgaz);
        glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-(buttonsizex+buttonzoom)+5, -(buttonsizey+buttonzoom)+5, 0.0);
        glTexCoord2f(0, 1); glVertex3f(-(buttonsizex+buttonzoom)+5,  (buttonsizey+buttonzoom)-5, 0.0);
        glTexCoord2f(1, 1); glVertex3f( (buttonsizex+buttonzoom)-5,  (buttonsizey+buttonzoom)-5, 0.0);
        glTexCoord2f(1, 0); glVertex3f( (buttonsizex+buttonzoom)-5, -(buttonsizey+buttonzoom)+5, 0.0);
        glEnd(); //End quadrilateral coordinates

    } else {
        glBindTexture(GL_TEXTURE_2D,normal_icon);		// default icon
        glBlendFunc(GL_ONE, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // zoom valgte
        if (i+1==(int) stream_key_selected) buttonzoom=18.0f;
        else buttonzoom=12.0f;

        glTranslatef(xof,yof,xvgaz);
        glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-(buttonsizex+buttonzoom), -(buttonsizey+buttonzoom), 0.0);
        glTexCoord2f(0, 1); glVertex3f(-(buttonsizex+buttonzoom),  buttonsizey+buttonzoom, 0.0);
        glTexCoord2f(1, 1); glVertex3f( buttonsizex+buttonzoom,  buttonsizey+buttonzoom, 0.0);
        glTexCoord2f(1, 0); glVertex3f( buttonsizex+buttonzoom, -(buttonsizey+buttonzoom), 0.0);
        glEnd(); //End quadrilateral coordinates

    }

    // draw numbers in group
    if (stack[i+sofset]->feed_group_antal>0) {
        // show numbers in group
        glLoadIdentity();
        glDisable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        glTranslatef(xof+10,yof-36,xvgaz);		// +38 - 27
        glRasterPos2f(0.0f, 0.0f);
        glScalef(14.0, 14.0, 1.0);
        glColor4f(1.0f, 1.0f, 1.0f,1.0f);

        sprintf(temptxt,"%4d",stack[i+sofset]->feed_group_antal);
        glcRenderString(temptxt);
    }


    buttonzoom=0.0f;

    strcpy(temptxt,stack[i+sofset]->feed_showtxt);        // text to show
    //lastslash=strrchr(temptxt,'/');
    //if (lastslash) strcpy(temptxt,lastslash+1);
//    temptxt[13]=0;
//    txtbrede=t3dDrawWidth(musicoversigt[i+sofset].album_name);

    glPushMatrix();

    float ytextofset=0.0f;
    int ii,j,k,pos;
    ii=pos=0;
    char word[16000];

    if (strlen(temptxt)<=14) {
      glLoadIdentity();
      ofs=(strlen(temptxt)/2)*9;
      switch(screen_size) {
        case 3: glTranslatef(xof-ofs,  yof-60-4 ,xvgaz);
                break;
        case 4: glTranslatef(xof-ofs,  yof-60-10 ,xvgaz);
                break;
        default:glTranslatef(xof-ofs,  yof-60 ,xvgaz);
                break;
      }
      glRasterPos2f(0.0f, 0.0f);
      glDisable(GL_TEXTURE_2D);
      glScalef(14.0, 14.0, 1.0);
      glcRenderString(temptxt);
    } else {

        temptxt[26]='\0';
        glLoadIdentity();
        ofs=(strlen(temptxt)/2)*9;
        switch(screen_size) {
            case 3: glTranslatef(xof-50,  yof-60-4 ,xvgaz);
                    break;
            case 4: glTranslatef(xof-50,  yof-60-10 ,xvgaz);
                    break;
            default:glTranslatef(xof-50,  yof-60 ,xvgaz);
                    break;
        }
        glRasterPos2f(0.0f, 0.0f);
        glDisable(GL_TEXTURE_2D);
        glScalef(14.0, 14.0, 1.0);


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
                  glLoadIdentity();
                  ofs=0;
                  switch(screen_size) {
                      case 3: glTranslatef(xof-50,  yof-60-4-ytextofset ,xvgaz);
                              break;
                      case 4: glTranslatef(xof-50,  yof-60-10-ytextofset ,xvgaz);
                              break;
                      default:glTranslatef(xof-50,  yof-60-ytextofset ,xvgaz);
                              break;
                  }
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
                glLoadIdentity();
                ofs=(int) (strlen(word)/2)*9;
                switch(screen_size) {
                    case 3: glTranslatef(xof-50,  yof-60-4-ytextofset ,xvgaz);
                            break;
                    case 4: glTranslatef(xof-50,  yof-60-10-ytextofset ,xvgaz);
                            break;
                    default:glTranslatef(xof-50,  yof-60-ytextofset ,xvgaz);
                            break;
                }
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
/*
     // lav ramme om valgte radio station
     if (i+1==radio_key_selected) {
          // lav valgte border
          glLoadIdentity();
          glTranslatef((xof), (yof), xvgaz);
          glColor3f(1.0f,1.0f,1.0f);
          glBindTexture(GL_TEXTURE_2D,0);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glBegin(GL_QUADS); //Begin quadrilateral coordinates

          // left
          glTexCoord2f(0.0, 0.0); glVertex3f(-72, -75, 0.0);
          glTexCoord2f(0.0, 1.0); glVertex3f(-75, -75, 0.0);
          glTexCoord2f(1.0, 1.0); glVertex3f(-75, 75-15, 0.0);
          glTexCoord2f(1.0, 0.0); glVertex3f(-72, 75-15, 0.0);
          // right
          glTexCoord2f(0.0, 0.0); glVertex3f(72, -75, 0.0);
          glTexCoord2f(0.0, 1.0); glVertex3f(75, -75, 0.0);
          glTexCoord2f(1.0, 1.0); glVertex3f(75, 75-15, 0.0);
          glTexCoord2f(1.0, 0.0); glVertex3f(72, 75-15, 0.0);

          // button
          glVertex3f(-75, -72, 0.0);
          glVertex3f(-75, -75, 0.0);
          glVertex3f(75, -75, 0.0);
          glVertex3f(75, -72, 0.0);

          // top
          glVertex3f(-75, 72-15, 0.0);
          glVertex3f(-75, 75-15, 0.0);
          glVertex3f(75, 75-15, 0.0);
          glVertex3f(75, 72-15, 0.0);
          glEnd();
    }

*/

    glPopMatrix();
    i++;

  //
  // hvis der ikke brugfes nvidia core ret størelser
  //

    switch(screen_size) {
        case 1: if (cur_avail_mem_kb==0) xof+=(buttonsizex*2.0)+8; else  xof+=(buttonsizex*1.6);
                break;
        case 2: if (cur_avail_mem_kb==0) xof+=(buttonsizex*2.0)+8; else xof+=(buttonsizex*1.6);
                break;
        case 3: if (cur_avail_mem_kb==0) xof+=(buttonsizex*2.0)+8; else xof+=(buttonsizex*1.7);
                break;
        case 4: if (cur_avail_mem_kb==0) xof+=(buttonsizex*2.0)+8; else xof+=(buttonsizex*1.65);
                break;
        default:xof+=(buttonsizex*2)+8;
                break;
    }
  }

  // show loading status
  if (stream_oversigt_loaded_nr<this->streamantal()) {
    switch (screen_size) {
          case 1: loader_xpos=-250;
                  loader_ypos=-230+2;
                  break;
          case 2: loader_xpos=-250;
                  loader_ypos=-230+2;
                  break;
          case 3: loader_xpos=-380;
                  loader_ypos=-230+2;
                  break;
          case 4: loader_xpos=-250;
                  loader_ypos=-230+2;
                  break;
          default:loader_xpos=-250;
                  loader_ypos=-230+2;
                  break;
    }

    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
//    glDisable(GL_BLEND);
//    glBlendFunc(GL_DST_COLOR, GL_ZERO);
//    glBlendFunc(GL_ONE, GL_ONE);


    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
/*
    glTranslatef(loader_xpos,loader_ypos,-600);
    glBindTexture(GL_TEXTURE_2D,_textureIdloading_mask);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-170/2, -60/2, 0.0);
    glTexCoord2f(0, 1); glVertex3f(-170/2,  60/2, 0.0);
    glTexCoord2f(1, 1); glVertex3f( 170/2,  60/2, 0.0);
    glTexCoord2f(1, 0); glVertex3f( 170/2, -60/2, 0.0);
    glEnd(); //End quadrilateral coordinates
*/
    glLoadIdentity();
    glTranslatef(loader_xpos,loader_ypos,-600);
    glBindTexture(GL_TEXTURE_2D,_textureIdloading);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-170/2, -60/2, 0.0);
    glTexCoord2f(0, 1); glVertex3f(-170/2,  60/2, 0.0);
    glTexCoord2f(1, 1); glVertex3f( 170/2,  60/2, 0.0);
    glTexCoord2f(1, 0); glVertex3f( 170/2, -60/2, 0.0);
    glEnd(); //End quadrilateral coordinates

    glLoadIdentity();
    glTranslatef(loader_xpos-36,loader_ypos-16,-600);
    glRasterPos2f(0.0f, 0.0f);
    glDisable(GL_TEXTURE_2D);
    glScalef(8.0, 8.0, 1.0);

    sprintf(temptxt,"%4d of %4d ",this->stream_oversigt_nowloading,this->streamantal());

    glcRenderString(temptxt);
  }

  if (this->streamantal()<1) {

    glLoadIdentity();
    glTranslatef(-10,10,-600);

    if (_textureIdloading1) {
      glBindTexture(GL_TEXTURE_2D,_textureIdloading1);
      glBlendFunc(GL_ONE,GL_ZERO);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glRotatef(45.0f, 0.0f, 0.0f, 0.0f);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(-170, -60, 0.0);
      glTexCoord2f(0, 1); glVertex3f(-170,  60, 0.0);
      glTexCoord2f(1, 1); glVertex3f( 170,  60, 0.0);
      glTexCoord2f(1, 0); glVertex3f( 170, -60, 0.0);
      glEnd(); //End quadrilateral coordinates
    }
    glLoadIdentity();
    glTranslatef(-52,6,-300);
    glRasterPos2f(0.0f, 0.0f);
    glDisable(GL_TEXTURE_2D);
    glScalef(8.0, 8.0, 1.0);
    sprintf(temptxt,"No stations loaded.");
    glcRenderString(temptxt);

    if (configmythtvver==0) {
        glLoadIdentity();
        glTranslatef(-52,-6,-300);
        glRasterPos2f(0.0f, 0.0f);
        glDisable(GL_TEXTURE_2D);
        glScalef(8.0, 8.0, 1.0);
        strcpy(temptxt,"No backend ");
        strcat(temptxt,configmysqlhost);
        glcRenderString(temptxt);
    }
  }
}
