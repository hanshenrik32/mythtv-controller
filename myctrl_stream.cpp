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
#include <cmath>
#include <iostream>
#include <fmt/format.h>

#include "myctrl_stream.h"
#include "utility.h"
#include "myth_ttffont.h"
#include "utility.h"
#include "readjpg.h"
#include "loadpng.h"
#include "myctrl_glprint.h"
#include "myth_config.h"

extern config_icons config_menu;

// web file loader
#include "myctrl_readwebfile.h"

//
// text render is glcRenderString for freetype font support (slow)
// new text render in use drawText()
//

extern long configrssguidelastupdate;
extern FILE *logfile;
extern char localuserhomedir[4096];                                    // user homedir set in main
extern char debuglogdata[1024];                                 // used by log system

extern char debuglogdata[1024];                                 // used by log system
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

// rss feeds and podcast

// to create youtube podcast use 
// https://www.youtube.com/feeds/videos.xml?channel_id=(channel id) 

// 2 levels array for earch line

// table internetcontent, table internetcontentarticles

std::vector<std::string> sql_podcast_array={
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Venjent music',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Venjent music',NULL,NULL,'Venjent music',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCrY8Y0rs3BlE3T57TOMM1aw',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Joshua Bardwell Livestream Clips',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Joshua Bardwell Livestream Clips',NULL,NULL,'Joshua Bardwell Livestream Clips',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UChpbytrxf-Aj9qe7Gp7sECg',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Aftenshowet',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Aftenshowet',NULL,NULL,'Aftenshowet',0,0,NULL,'https://www.dr.dk/mu/Feed/aftenshowet-9.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Anders Lund Madsen i Den Yderste By',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Anders Lund Madsen i Den Yderste By',NULL,NULL,'Anders lund massen i den udereste by',0,0,NULL,'http://www.dr.dk/mu/Feed/anders-lund-madsen-i-den-yderste-by.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Best of YouTube (video)',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Best of YouTube (video)',NULL,NULL,'Best of YouTube (video)',0,0,NULL,'http://feeds.feedburner.com/boyt',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Bonderøven',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Bonderøven',NULL,NULL,'Bonderøven',0,0,NULL,'https://www.dr.dk/mu/Feed/bonderoven-alle.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('CNET',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('CNET',NULL,NULL,'CNET',0,0,NULL,'http://feed.cnet.com/feed/podcast/all/hd.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Droner og kanoner',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Droner og kanoner',NULL,NULL,'Droner og kanoner',0,0,NULL,'https://www.dr.dk/mu/Feed/droner-og-kanoner.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Dynamo',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Dynamo',NULL,NULL,'Dynamo',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UC7SDsqJba5428-EOBZWOn3w',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('So ein Ding',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('So ein Ding',NULL,NULL,'So ein Ding',0,0,NULL,'https://www.dr.dk/mu/Feed/so-ein-ding?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Verdens Bedste Filmklub',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Verdens Bedste Filmklub',NULL,NULL,'Verdens Bedste Filmklub',0,0,NULL,'http://lytbar.dk/wordpress/?feed=rss2&cat=7',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('I seng med NOVA',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('I seng med NOVA',NULL,NULL,'I seng med NOVA',0,0,NULL,'https://tools.planetradio.co.uk/core/podcasts/rss.php?name=i-seng-med-nova',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Krager og Drager - Dansk Game of Thrones Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Krager og Drager - Dansk Game of Thrones Podcast',NULL,NULL,'Krager og Drager - Dansk Game of Thrones Podcast',0,0,NULL,'http://www.kragerogdrager.dk/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The Verge',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Verge',NULL,NULL,'The Verge',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCddiUEpeqJcYeBxX1IVBKvQ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Tech Talk Today',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Tech Talk Today',NULL,NULL,'Tech Talk Today',0,0,NULL,'http://techtalk.today/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Kontant',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Kontant',NULL,NULL,'Kontant',0,0,NULL,'https://www.dr.dk/mu/Feed/kontant-2.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Movieclips Coming Soon',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Movieclips Coming Soon',NULL,NULL,'Movieclips Coming Soon',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCkR0GY0ue02aMyM-oxwgg9g',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The Rocket Ranch',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Rocket Ranch',NULL,NULL,'The Rocket Ranch',0,0,NULL,'https://www.nasa.gov/rss/dyn/Rocket-Ranch.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Troldspejlet Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Troldspejlet Podcast',NULL,NULL,'Troldspejlet Podcast',0,0,NULL,'https://www.dr.dk/mu/feed/troldspejlet-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('GameSpot',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('GameSpot',NULL,NULL,'GameSpot',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCbu2SsF-Or3Rsn3NxqODImw',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The Story from The Guardian',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Story from The Guardian',NULL,NULL,'The Story from The Guardian',0,0,NULL,'https://www.theguardian.com/news/series/the-story/podcast.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('DEN GRÅ SIDE',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('DEN GRÅ SIDE',NULL,NULL,'DEN GRÅ SIDE',0,0,NULL,'http://www.spreaker.com/show/2108328/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('TechSNAP',NULL,1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('TechSNAP',NULL,NULL,'TechSNAP',0,0,NULL,'http://techsnap.systems/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Bitch Sesh: A Real Housewives Breakdown',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Bitch Sesh: A Real Housewives Breakdown',NULL,NULL,'Bitch Sesh: A Real Housewives Breakdown',0,0,NULL,'http://rss.earwolf.com/bitch-sesh',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('2 Dope Queens',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('2 Dope Queens',NULL,NULL,'2 Dope Queens',0,0,NULL,'http://feeds.wnyc.org/2dopequeens',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Ramajetterne',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Ramajetterne',NULL,NULL,'Ramajetterne',0,0,NULL,'https://www.dr.dk/mu/Feed/ramajetterne-2.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('De Sorte Spejdere - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('De Sorte Spejdere - podcast',NULL,NULL,'De Sorte Spejdere - podcast',0,0,NULL,'http://www.dr.dk/mu/Feed/de-sorte-spejdere-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Nak & æd',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Nak & æd',NULL,NULL,'Nak & æd',0,0,NULL,'http://www.dr.dk/mu/Feed/nak-og-aed-alle.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Ubådssagen',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Ubådssagen',NULL,NULL,'Ubådssagen',0,0,NULL,'http://www.dr.dk/mu/Feed/ubadssagen.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Videnskabens Verden',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles(feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Videnskabens Verden',NULL,NULL,'Ubådssagen',0,0,NULL,'http://www.dr.dk/mu/Feed/videnskabens-verden.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Brainstorm',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles(feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Brainstorm',NULL,NULL,'Brainstorm',0,0,NULL,'https://audioboom.com/channels/4985564.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Gametest Fan Uploads',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Gametest Fan Uploads',NULL,NULL,'Gametest Fan Uploads',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCuX8EWTbH--tyha5FUoBSrg',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Gravity Assist',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Gravity Assist',NULL,NULL,'Gravity Assist',0,0,NULL,'https://www.nasa.gov/rss/dyn/Gravity-Assist.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('TED',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('TED',NULL,NULL,'TED',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCAuUUnT6oDeKwE6v1NGQxug',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Hva så?! med Christian Fuhlendorff',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Hva så?! med Christian Fuhlendorff',NULL,NULL,'Hva så?! med Christian Fuhlendorff',0,0,NULL,'http://hva-saa.podomatic.com/rss2.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Mads & Monopolet - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Mads & Monopolet - podcast',NULL,NULL,'Mads & Monopolet - podcast',0,0,NULL,'https://www.dr.dk/mu/feed/mads-monopolet-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Det røde rum',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Det røde rum',NULL,NULL,'Det røde rum',0,0,NULL,'https://www.dr.dk/mu/Feed/det-roede-rum-radio.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Selvsving',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Selvsving',NULL,NULL,'Selvsving',0,0,NULL,'https://www.dr.dk/mu/Feed/selvsving?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Lille Nørd',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Lille Nørd',NULL,NULL,'Lille Nørd',0,0,NULL,'https://www.dr.dk/mu/Feed/lille-noerd?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Sofus',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Sofus',NULL,NULL,'Sofus',0,0,NULL,'https://www.dr.dk/mu/Feed/sofus-2.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('CBS This Morning',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('CBS This Morning',NULL,NULL,'CBS This Morning',0,0,NULL,'http://cbsradionewsfeed.com/rss.php?id=149&ud=12',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('CBS Evening News -- Full Audio',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('CBS Evening News -- Full Audio',NULL,NULL,'CBS Evening News -- Full Audio',0,0,NULL,'http://cbsradionewsfeed.com/rss.php?id=126&ud=12',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Vergecast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Vergecast',NULL,NULL,'Vergecast',0,0,NULL,'http://feeds.feedburner.com/ThisIsMyNextPodcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Linux Action News',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Linux Action News',NULL,NULL,'Linux Action News',0,0,NULL,'http://linuxactionnews.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Linux Action Show',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Linux Action Show',NULL,NULL,'Linux Action Show',0,0,NULL,'http://feeds.feedburner.com/linuxashd?format=xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('GNU World Order',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('GNU World Order',NULL,NULL,'GNU World Order',0,0,NULL,'http://gnuworldorder.info/ogg.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The Command Line Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Command Line Podcast',NULL,NULL,'The Command Line Podcast',0,0,NULL,'https://thecommandline.net/cmdln_free',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Her går det godt',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Her går det godt',NULL,NULL,'Her går det godt',0,0,NULL,'http://www.spreaker.com/show/2093919/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('he RC Newb Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The RC Newb Podcast',NULL,NULL,'The RC Newb Podcast',0,0,NULL,'https://rcnewb.com/feed/podcast/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('P7 MIX Maraton - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('P7 MIX Maraton - podcast',NULL,NULL,'P7 MIX Maraton',0,0,NULL,'https://www.dr.dk/mu/feed/p7-maraton.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('going linux',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('going linux',NULL,NULL,'going linux',0,0,NULL,'http://goinglinux.com/mp3podcast.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Fire forførende radiofortællinger',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Fire forførende radiofortællinger',NULL,NULL,'Fire forførende radiofortællinger',0,0,NULL,'https://www.dr.dk/mu/feed/fire-forfoerende-radiofortaellinger.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Videnskab.dk Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Videnskab.dk Podcast',NULL,NULL,'Videnskab.dk Podcast',0,0,NULL,'http://feeds.soundcloud.com/users/soundcloud:users:4378703/sounds.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Den nye stil - historien om dansk rap',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Den nye stil - historien om dansk rap',NULL,NULL,'Historien-om-dansk-rap',0,0,NULL,'https://www.dr.dk/mu/feed/den-nye-stil-historien-om-dansk-rap.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Giant Gnome Productions',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Giant Gnome Productions',NULL,NULL,'Giant Gnome Productions',0,0,NULL,'https://giantgnome.com/feed/podcast/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('META',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('META',NULL,NULL,'META',0,0,NULL,'https://www.spreaker.com/show/2663274/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Voksen ABC podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Voksen ABC podcast',NULL,NULL,'Voksen ABC podcast',0,0,NULL,'http://voksenabc.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Third Ear',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Third Ear',NULL,NULL,'Third Ear',0,0,NULL,'https://thirdear.podbean.com/feed.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Sex Nerd Sandra',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Sex Nerd Sandra',NULL,NULL,'Sex Nerd Sandra',0,0,NULL,'http://sexnerdsandra.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Skru op for sex',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Skru op for sex',NULL,NULL,'Skru op for sex',0,0,NULL,'https://www.spreaker.com/show/3006683/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Techsistens',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Techsistens',NULL,NULL,'Techsistens',0,0,NULL,'http://feeds.soundcloud.com/users/soundcloud:users:56822074/sounds.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('beginner audiophile | hifi | gear reviews | stereo | hi-end audio',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('beginner audiophile | hifi | gear reviews | stereo | hi-end audio',NULL,NULL,'beginner audiophile | hifi | gear reviews | stereo | hi-end audio',0,0,NULL,'http://beginneraudiophile.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('What Hi-Fi? Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('What Hi-Fi? Podcast',NULL,NULL,'What Hi-Fi? Podcast',0,0,NULL,'http://feeds.soundcloud.com/users/soundcloud:users:425082507/sounds.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('NASACast Video',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('NASACast Video',NULL,NULL,'NASACast Video',0,0,NULL,'https://www.nasa.gov/rss/dyn/NASAcast_vodcast.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Inside Dynaudio',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Inside Dynaudio',NULL,NULL,'Inside Dynaudio',0,0,NULL,'https://www.nasa.gov/rss/dyn/NASAcast_vodcast.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Converge with Casey Newton',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL,NULL,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Converge with Casey Newton',NULL,NULL,'Converge with Casey Newton',0,0,NULL,'http://feeds.feedburner.com/ConvergeWithCaseyNewton',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('TED Talks Daily',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('TED Talks Daily',NULL,NULL,'TED Talks Daily ',0,0,NULL,'http://feeds.feedburner.com/TEDTalks_audio',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('RISE podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('RISE podcast',NULL,NULL,'RISE podcast ',0,0,NULL,'http://feeds.soundcloud.com/users/soundcloud:users:300950004/sounds.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('StarTalk Radio',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('StarTalk Radio',NULL,NULL,'StarTalk Radio ',0,0,NULL,'https://rss.art19.com/startalk-radio',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Stuff You Should Know',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Stuff You Should Know',NULL,NULL,'Stuff You Should Know',0,0,NULL,'https://feeds.megaphone.fm/stuffyoushouldknow',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Tilbage til Mars',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Tilbage til Mars',NULL,NULL,'Tilbage til Mars',0,0,NULL,'https://www.dr.dk/mu/feed/tilbage-til-mars.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Love + Radio',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Love + Radio',NULL,NULL,'Love + Radio',0,0,NULL,'http://feed.loveandradio.org/loveplusradio',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Barometeret',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Barometeret',NULL,NULL,'Barometeret',0,0,NULL,'https://www.dr.dk/mu/Feed/barometeret.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Et langsomt mord',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Et langsomt mord',NULL,NULL,'Et langsomt mord',0,0,NULL,'https://www.dr.dk/mu/feed/et-stille-mord.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Swingercast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Swingercast',NULL,NULL,'Swingercast',0,0,NULL,'http://feeds.feedburner.com/Swingercast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('FONK! Det er podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('FONK! Det er podcast',NULL,NULL,'FONK! Det er podcast',0,0,NULL,'https://www.dr.dk/mu/feed/fonk-det-er-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Filmland',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Filmland',NULL,NULL,'Filmland',0,0,NULL,'https://www.dr.dk/mu/feed/filmland.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Radio Information',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Radio Information',NULL,NULL,'Radio Information',0,0,NULL,'https://radioinformation.podbean.com/feed.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('kino.dk podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('kino.dk podcast',NULL,NULL,'kino.dk podcast',0,0,NULL,'http://kinodk.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Tændt',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Tændt',NULL,NULL,'Tændt',0,0,NULL,'https://www.dr.dk/mu/feed/taendt.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Sound of Cinema',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Sound of Cinema',NULL,NULL,'Sound of Cinema',0,0,NULL,'http://podcasts.files.bbci.co.uk/b03bzf92.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('P1 podcaster',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('P1 podcaster',NULL,NULL,'P1 podcaster',0,0,NULL,'https://www.dr.dk/mu/feed/p1-podcaster.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Guys We F****d',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Guys We F****d',NULL,NULL,'Guys We F****d',0,0,NULL,'https://feeds.megaphone.fm/LM9233046886',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('bollemusik',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('bollemusik',NULL,NULL,'bollemusik',0,0,NULL,'https://heartbeats.dk/feed/podcast/bollemusik',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Danskerbingo',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Danskerbingo',NULL,NULL,'Danskerbingo',0,0,NULL,'https://www.dr.dk/mu/feed/danskerbingo-radio.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Uafbrudt Samleje &#8211; Heartbeats.dk',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Uafbrudt Samleje &#8211; Heartbeats.dk',NULL,NULL,'Uafbrudt Samleje &#8211; Heartbeats.dk',0,0,NULL,'https://heartbeats.dk/series/uafbrudt-samleje/feed/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Alt om Emma - en serie om MDMA',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Alt om Emma - en serie om MDMA',NULL,NULL,'Alt om Emma - en serie om MDMA',0,0,NULL,'https://www.dr.dk/mu/Feed/alt-om-emma.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Radiofortællinger',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Radiofortællinger',NULL,NULL,'Radiofortællinger',0,0,NULL,'https://www.dr.dk/mu/feed/radiofortaellinger.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Shitstorm',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Shitstorm',NULL,NULL,'Shitstorm',0,0,NULL,'https://www.dr.dk/mu/feed/shitstorm.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('For Adults Only | Sexy Hot Stories Erotic from the Street',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('For Adults Only | Sexy Hot Stories Erotic from the Street',NULL,NULL,'For Adults Only | Sexy Hot Stories Erotic from the Street',0,0,NULL,'http://adultstories.fastforward.libsynpro.com/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('FONK! Det er podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('FONK! Det er podcast',NULL,NULL,'FONK! Det er podcast',0,0,NULL,'https://www.dr.dk/mu/feed/fonk-det-er-podcast.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Security Central',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Security Central',NULL,NULL,'Security Central',0,0,NULL,'https://www.spreaker.com/show/3577866/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Stuff You Should Know',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Stuff You Should Know',NULL,NULL,'Stuff You Should Know,0,0,NULL,'https://feeds.megaphone.fm/stuffyoushouldknow',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Blackout - pigen på billedet',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Blackout - pigen på billedet',NULL,NULL,'Blackout - pigen på billedet',0,0,NULL,'https://www.dr.dk/mu/feed/blackout-pigen-paa-billedet.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Hjernen på overarbejde',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Hjernen på overarbejde',NULL,NULL,'Hjernen på overarbejde',0,0,NULL,'https://www.spreaker.com/show/3188722/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Østrogenial',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Østrogenial',NULL,NULL,'Østrogenial',0,0,NULL,'https://www.spreaker.com/show/3484630/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('P6 BEAT elsker - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('P6 BEAT elsker - podcast',NULL,NULL,'P6 BEAT elsker - podcast',0,0,NULL,'https://www.dr.dk/mu/feed/p6-beat-elsker.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Kid-N-Drag Gaming',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Kid-N-Drag Gaming',NULL,NULL,'Kid-N-Drag Gaming',0,0,NULL,'https://podcast.kidndrag.chat/feed/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Godnathistorien',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Godnathistorien',NULL,NULL,'Godnathistorien',0,0,NULL,'https://rss.podplaystudio.com/1060.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Beslutningen',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Beslutningen',NULL,NULL,'Beslutningen',0,0,NULL,'https://rss.podplaystudio.com/1052.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('NextM Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('NextM Podcast',NULL,NULL,'NextM Podcast',0,0,NULL,'https://rss.podplaystudio.com/1040.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Det er bare en overgang',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Det er bare en overgang',NULL,NULL,'Det er bare en overgang',0,0,NULL,'https://rss.podplaystudio.com/1069.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Sex om Dagen',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Sex om Dagen',NULL,NULL,'Sex om Dagen',0,0,NULL,'https://rss.podplaystudio.com/1075.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Varigt vægttab',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Varigt vægttab',NULL,NULL,'Varigt vægttab',0,0,NULL,'https://rss.podplaystudio.com/1006.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Nattens dronning',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Nattens dronning',NULL,NULL,'Nattens dronning',0,0,NULL,'https://www.dr.dk/mu/feed/nattens-dronning.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The Heart',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Heart',NULL,NULL,'The Heart',0,0,NULL,'http://feeds.theheartradio.org/TheHeartRadio',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Curlingklubben',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Curlingklubben',NULL,NULL,'Curlingklubben',0,0,NULL,'https://www.dr.dk/mu/feed/p3-med-christian-og-maria.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Geek Beat',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Geek Beat',NULL,NULL,'Geek Beat',0,0,NULL,'https://feeds.feedburner.com/geekbeatblog',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The WIRED Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The WIRED Podcast',NULL,NULL,'The WIRED Podcast',0,0,NULL,'https://www.wired.co.uk/rss/podcast/wired-podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Ear Hustle',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Ear Hustle',NULL,NULL,'Ear Hustle',0,0,NULL,'http://feeds.earhustlesq.com/earhustlesq',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('War on the Rocks',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('War on the Rocks',NULL,NULL,'War on the Rocks',0,0,NULL,'https://warontherocks.com/feed/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The Joe Rogan Experience',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Joe Rogan Experience',NULL,NULL,'The Joe Rogan Experience',0,0,NULL,'http://joeroganexp.joerogan.libsynpro.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('heardit',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('heardit',NULL,NULL,'heardit',0,0,NULL,'https://feeds.buzzsprout.com/480832.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Threat Wire',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Threat Wire',NULL,NULL,'Threat Wire',0,0,NULL,'https://feed.podbean.com/shannonmorse/feed.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Replay',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Replay',NULL,NULL,'Replay',0,0,NULL,'https://podcasts.files.bbci.co.uk/p06qbzmj.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Marsfall',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Marsfall',NULL,NULL,'Marsfall',0,0,NULL,'http://marsfallpodcast.com/feed/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('PARADOXICA',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('PARADOXICA',NULL,NULL,'PARADOXICA',0,0,NULL,'https://arsparadoxica.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('StarTripper!!',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('StarTripper!!',NULL,NULL,'StarTripper!!',0,0,NULL,'https://startripper.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The Big Loop',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Big Loop',NULL,NULL,'The Big Loop',0,0,NULL,'https://thebiglooppodcast.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Janus Descending',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Janus Descending',NULL,NULL,'Janus Descending',0,0,NULL,'https://janusdescending.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Wolf 359',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Wolf 359',NULL,NULL,'Wolf 359',0,0,NULL,'https://wolf359radio.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The First Stop For Security News',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The First Stop For Security News',NULL,NULL,'The First Stop For Security News',0,0,NULL,'https://threatpost.com/category/podcasts/feed/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Threat Wire',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Threat Wire',NULL,NULL,'Threat Wire',0,0,NULL,'https://feed.podbean.com/shannonmorse/feed.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Reply All',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Reply All',NULL,NULL,'Reply All',0,0,NULL,'https://feeds.megaphone.fm/replyall',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('This Week in Linux',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('This Week in Linux',NULL,NULL,'This Week in Linux',0,0,NULL,'https://tuxdigital.com/feed/thisweekinlinux-mp3',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Destination Linux',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Destination Linux',NULL,NULL,'Destination Linux',0,0,NULL,'https://destinationlinux.org/feed/mp3/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Digitalt',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Digitalt',NULL,NULL,'Digitalt',0,0,NULL,'https://www.dr.dk/mu/feed/digitalt.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Jordforbindelsen der forsvandt',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Jordforbindelsen der forsvandt',NULL,NULL,'Jordforbindelsen der forsvandt',0,0,NULL,'https://www.dr.dk/mu/feed/jordforbindelsen-der-forsvandt.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('I virkeligheden',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('I virkeligheden',NULL,NULL,'I virkeligheden',0,0,NULL,'https://www.dr.dk/mu/feed/i-virkeligheden-radio.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Jazzklubben',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Jazzklubben',NULL,NULL,'Jazzklubben',0,0,NULL,'https://www.dr.dk/mu/feed/jazzklubben.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Line Kirsten Giftekniv',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Line Kirsten Giftekniv',NULL,NULL,'Line Kirsten Giftekniv',0,0,NULL,'https://www.dr.dk/mu/feed/line-kirsten-giftekniv.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Mod det uendelige univers',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Mod det uendelige univers',NULL,NULL,'Mod det uendelige univers',0,0,NULL,'https://www.dr.dk/mu/feed/mod-det-uendelige-univers.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Min yndlingsmusik',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Min yndlingsmusik',NULL,NULL,'Min yndlingsmusik',0,0,NULL,'https://www.dr.dk/mu/feed/min-yndlingsmusik.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Verdens bedste lyttere på date',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Verdens bedste lyttere på date',NULL,NULL,'Verdens bedste lyttere på date',0,0,NULL,'https://www.dr.dk/mu/feed/verdens-bedste-lyttere-pa-date-podcast.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Wikibeats - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Wikibeats - podcast',NULL,NULL,'Wikibeats - podcast',0,0,NULL,'https://www.dr.dk/mu/feed/wikibeats-podcast.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Absolute Absolute Music',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Absolute Absolute Music',NULL,NULL,'Absolute Absolute Music',0,0,NULL,'https://www.dr.dk/mu/feed/absolute-absolute-music.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The Infinite Monkey Cage',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Infinite Monkey Cage',NULL,NULL,'The Infinite Monkey Cage',0,0,NULL,'https://podcasts.files.bbci.co.uk/b00snr0w.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Putting Science to Work',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Putting Science to Work',NULL,NULL,'Putting Science to Work',0,0,NULL,'https://podcasts.files.bbci.co.uk/b06t1y3z.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('13 Minutes to the Moon',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('13 Minutes to the Moon',NULL,NULL,'13 Minutes to the Moon',0,0,NULL,'https://podcasts.files.bbci.co.uk/w13xttx2.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Jazz Library',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Jazz Library',NULL,NULL,'Jazz Library',0,0,NULL,'https://podcasts.files.bbci.co.uk/b006x41z.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Det Tomme Magasin',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Det Tomme Magasin',NULL,NULL,'Det Tomme Magasin',0,0,NULL,'https://www.dr.dk/mu/feed/det-tomme-magasin.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Erotic Audio',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Erotic Audio','Erotic Audio',0,0,NULL,'https://feeds.buzzsprout.com/687557.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Tech fra toppen',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Tech fra toppen','Tech fra toppen',0,0,NULL,'http://www.spreaker.com/show/2832975/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Brittas bedrag',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Brittas bedrag',NULL,NULL,'Brittas bedrag',0,0,NULL,'https://www.dr.dk/mu/feed/britta-vs-staten.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Stegelmanns score',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Stegelmanns score',NULL,NULL,'Stegelmanns score',0,0,NULL,'https://www.dr.dk/mu/feed/stegelmanns-scores.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Er der liv på Venus?',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Er der liv på Venus?',NULL,NULL,'Er der liv på Venus?',0,0,NULL,'https://www.dr.dk/mu/feed/er-der-liv-pa-venus.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Fries before guys',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Fries before guys',NULL,NULL,'Fries before guys',0,0,NULL,'https://friesb4guyspodcast.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('TikTok-manden',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('TikTok-manden',NULL,NULL,'TikTok-manden',0,0,NULL,'https://www.spreaker.com/show/4711015/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Ask Drone U',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Ask Drone U',NULL,NULL,'Ask Drone U',0,0,NULL,'https://www.thedroneu.com/feed/podcast/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Drone Radio Show',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Drone Radio Show',NULL,NULL,'Drone Radio Show',0,0,NULL,'http://droneradioshow.com/feed/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Drone Radio Show',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Drone Radio Show',NULL,NULL,'Drone Radio Show',0,0,NULL,'http://droneradioshow.com/feed/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('<![CDATA[The Business of Drones]]>',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('<![CDATA[The Business of Drones]]>',NULL,NULL,'<![CDATA[The Business of Drones]]>',0,0,NULL,'https://anchor.fm/s/2bdc4bc/podcast/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('VENNETJENESTEN',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('VENNETJENESTEN',NULL,NULL,'VENNETJENESTEN',0,0,NULL,'https://radio4.dk/rss/xml/vennetjenesten.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Alex og kaninhullet',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Alex og kaninhullet',NULL,NULL,'Alex og kaninhullet',0,0,NULL,'https://politiken.dk/webservice/podcast/channels/channel_alex/feed/itunes/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('How to FPV Per Lommel',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('How to FPV Per Lommel',NULL,NULL,'How to FPV Per Lommel',0,0,NULL,'https://feeds.buzzsprout.com/1759649.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Quad Talk FPV Drone Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Quad Talk FPV Drone Podcast',NULL,NULL,'Quad Talk FPV Drone Podcast',0,0,NULL,'http://tun.in/tlkFLa',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The FPV Show',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The FPV Show',NULL,NULL,'The FPV Show',0,0,NULL,'https://thefpvshow.libsyn.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Blodets bånd',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Blodets bånd',NULL,NULL,'Blodets bånd',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/blodets-band-2.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Krigen på isen',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Krigen på isen',NULL,NULL,'Krigen på isen',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/krigen-pa-isen.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('P1 forklarer',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('P1 forklarer',NULL,NULL,'P1 forklarer',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/p1-forklarer.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('10-20-30',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('10-20-30',NULL,NULL,'10-20-30',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/10-20-30.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Det brændte bordel',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Det brændte bordel',NULL,NULL,'Det brændte bordel',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/det-braendte-bordel.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Hvad tænder os to?',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Hvad tænder os to?',NULL,NULL,'Hvad tænder os to?',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/hvad-taender-os-to.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Dansk musik i verden',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Dansk musik i verden',NULL,NULL,'Dansk musik i verden',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/dansk-musik-i-verden.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Kejserens nye klub',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Kejserens nye klub',NULL,NULL,'Kejserens nye klub',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/kejserens-nye-klub.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Bræk det ned med Kato',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Bræk det ned med Kato',NULL,NULL,'Bræk det ned med Kato',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/braek-det-ned-med-kato.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Sex IRL',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Sex IRL',NULL,NULL,'Sex IRL',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/sex-irl.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Splittet til atomer',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Splittet til atomer',NULL,NULL,'Splittet til atomer',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/splittet-til-atomer.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('DR Romanprisen',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('DR Romanprisen',NULL,NULL,'DR Romanprisen',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/dr-romanprisen-podcast.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Bakspejl',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Bakspejl',NULL,NULL,'Bakspejl',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/bakspejl.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Ramt af kunst',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Ramt af kunst',NULL,NULL,'Ramt af kunst',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/ramt-af-kunst.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Skønlitteratur',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Skønlitteratur',NULL,NULL,'Skønlitteratur',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/skoenlitteratur-pa-p1.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Ubegribeligt',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Ubegribeligt',NULL,NULL,'Ubegribeligt',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/ubegribeligt.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Hemmeligheder',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Hemmeligheder',NULL,NULL,'Hemmeligheder',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/hemmeligheder-2.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Hjernekassen på P1',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Hjernekassen på P1',NULL,NULL,'Hjernekassen på P1',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/hjernekassen-pa-p1.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Verdens bedste film',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Verdens bedste film',NULL,NULL,'Verdens bedste film',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/verdens-bedste-film-radio.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Fantasier',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Aftenshowet',NULL,NULL,'Aftenshowet',0,0,NULL,'https://www.dr.dk/mu/Feed/aftenshowet-9.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Anders Lund Madsen i Den Yderste By',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Anders Lund Madsen i Den Yderste By',NULL,NULL,'Anders lund massen i den udereste by',0,0,NULL,'http://www.dr.dk/mu/Feed/anders-lund-madsen-i-den-yderste-by.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Best of YouTube (video)',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Best of YouTube (video)',NULL,NULL,'Best of YouTube (video)',0,0,NULL,'http://feeds.feedburner.com/boyt',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Bonderøven',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Bonderøven',NULL,NULL,'Bonderøven',0,0,NULL,'https://www.dr.dk/mu/Feed/bonderoven-alle.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('CNET',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('CNET',NULL,NULL,'CNET',0,0,NULL,'http://feed.cnet.com/feed/podcast/all/hd.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Droner og kanoner',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Droner og kanoner',NULL,NULL,'Droner og kanoner',0,0,NULL,'https://www.dr.dk/mu/Feed/droner-og-kanoner.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Dynamo',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Dynamo',NULL,NULL,'Dynamo',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UC7SDsqJba5428-EOBZWOn3w',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('So ein Ding',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('So ein Ding',NULL,NULL,'So ein Ding',0,0,NULL,'https://www.dr.dk/mu/Feed/so-ein-ding?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Verdens Bedste Filmklub',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Verdens Bedste Filmklub',NULL,NULL,'Verdens Bedste Filmklub',0,0,NULL,'http://lytbar.dk/wordpress/?feed=rss2&cat=7',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('I seng med NOVA',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('I seng med NOVA',NULL,NULL,'I seng med NOVA',0,0,NULL,'https://tools.planetradio.co.uk/core/podcasts/rss.php?name=i-seng-med-nova',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Krager og Drager - Dansk Game of Thrones Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Krager og Drager - Dansk Game of Thrones Podcast',NULL,NULL,'Krager og Drager - Dansk Game of Thrones Podcast',0,0,NULL,'http://www.kragerogdrager.dk/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The Verge',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Verge',NULL,NULL,'The Verge',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCddiUEpeqJcYeBxX1IVBKvQ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Tech Talk Today',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Tech Talk Today',NULL,NULL,'Tech Talk Today',0,0,NULL,'http://techtalk.today/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Kontant',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Kontant',NULL,NULL,'Kontant',0,0,NULL,'https://www.dr.dk/mu/Feed/kontant-2.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Movieclips Coming Soon',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Movieclips Coming Soon',NULL,NULL,'Movieclips Coming Soon',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCkR0GY0ue02aMyM-oxwgg9g',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The Rocket Ranch',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Rocket Ranch',NULL,NULL,'The Rocket Ranch',0,0,NULL,'https://www.nasa.gov/rss/dyn/Rocket-Ranch.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Troldspejlet Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Troldspejlet Podcast',NULL,NULL,'Troldspejlet Podcast',0,0,NULL,'https://www.dr.dk/mu/feed/troldspejlet-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('GameSpot',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('GameSpot',NULL,NULL,'GameSpot',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCbu2SsF-Or3Rsn3NxqODImw',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The Story from The Guardian',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Story from The Guardian',NULL,NULL,'The Story from The Guardian',0,0,NULL,'https://www.theguardian.com/news/series/the-story/podcast.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('DEN GRÅ SIDE',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('DEN GRÅ SIDE',NULL,NULL,'DEN GRÅ SIDE',0,0,NULL,'http://www.spreaker.com/show/2108328/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('TechSNAP',NULL,1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('TechSNAP',NULL,NULL,'TechSNAP',0,0,NULL,'http://techsnap.systems/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Bitch Sesh: A Real Housewives Breakdown',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Bitch Sesh: A Real Housewives Breakdown',NULL,NULL,'Bitch Sesh: A Real Housewives Breakdown',0,0,NULL,'http://rss.earwolf.com/bitch-sesh',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('2 Dope Queens',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('2 Dope Queens',NULL,NULL,'2 Dope Queens',0,0,NULL,'http://feeds.wnyc.org/2dopequeens',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Ramajetterne',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Ramajetterne',NULL,NULL,'Ramajetterne',0,0,NULL,'https://www.dr.dk/mu/Feed/ramajetterne-2.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('De Sorte Spejdere - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('De Sorte Spejdere - podcast',NULL,NULL,'De Sorte Spejdere - podcast',0,0,NULL,'http://www.dr.dk/mu/Feed/de-sorte-spejdere-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Nak & æd',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Nak & æd',NULL,NULL,'Nak & æd',0,0,NULL,'http://www.dr.dk/mu/Feed/nak-og-aed-alle.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Ubådssagen',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Ubådssagen',NULL,NULL,'Ubådssagen',0,0,NULL,'http://www.dr.dk/mu/Feed/ubadssagen.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Videnskabens Verden',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles(feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Videnskabens Verden',NULL,NULL,'Ubådssagen',0,0,NULL,'http://www.dr.dk/mu/Feed/videnskabens-verden.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Brainstorm',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles(feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Brainstorm',NULL,NULL,'Brainstorm',0,0,NULL,'https://audioboom.com/channels/4985564.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Gametest Fan Uploads',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Gametest Fan Uploads',NULL,NULL,'Gametest Fan Uploads',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCuX8EWTbH--tyha5FUoBSrg',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Gravity Assist',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Gravity Assist',NULL,NULL,'Gravity Assist',0,0,NULL,'https://www.nasa.gov/rss/dyn/Gravity-Assist.rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('TED',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('TED',NULL,NULL,'TED',0,0,NULL,'https://www.youtube.com/feeds/videos.xml?channel_id=UCAuUUnT6oDeKwE6v1NGQxug',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Hva så?! med Christian Fuhlendorff',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Hva så?! med Christian Fuhlendorff',NULL,NULL,'Hva så?! med Christian Fuhlendorff',0,0,NULL,'http://hva-saa.podomatic.com/rss2.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Mads & Monopolet - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Mads & Monopolet - podcast',NULL,NULL,'Mads & Monopolet - podcast',0,0,NULL,'https://www.dr.dk/mu/feed/mads-monopolet-podcast.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Det røde rum',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Det røde rum',NULL,NULL,'Det røde rum',0,0,NULL,'https://www.dr.dk/mu/Feed/det-roede-rum-radio.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Selvsving',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Selvsving',NULL,NULL,'Selvsving',0,0,NULL,'https://www.dr.dk/mu/Feed/selvsving?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Lille Nørd',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Lille Nørd',NULL,NULL,'Lille Nørd',0,0,NULL,'https://www.dr.dk/mu/Feed/lille-noerd?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Sofus',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Sofus',NULL,NULL,'Sofus',0,0,NULL,'https://www.dr.dk/mu/Feed/sofus-2.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('CBS This Morning',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('CBS This Morning',NULL,NULL,'CBS This Morning',0,0,NULL,'http://cbsradionewsfeed.com/rss.php?id=149&ud=12',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('CBS Evening News -- Full Audio',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('CBS Evening News -- Full Audio',NULL,NULL,'CBS Evening News -- Full Audio',0,0,NULL,'http://cbsradionewsfeed.com/rss.php?id=126&ud=12',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Vergecast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Vergecast',NULL,NULL,'Vergecast',0,0,NULL,'http://feeds.feedburner.com/ThisIsMyNextPodcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Linux Action News',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Linux Action News',NULL,NULL,'Linux Action News',0,0,NULL,'http://linuxactionnews.com/rss',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Linux Action Show',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Linux Action Show',NULL,NULL,'Linux Action Show',0,0,NULL,'http://feeds.feedburner.com/linuxashd?format=xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('GNU World Order',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('GNU World Order',NULL,NULL,'GNU World Order',0,0,NULL,'http://gnuworldorder.info/ogg.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('The Command Line Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The Command Line Podcast',NULL,NULL,'The Command Line Podcast',0,0,NULL,'https://thecommandline.net/cmdln_free',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Her går det godt',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Her går det godt',NULL,NULL,'Her går det godt',0,0,NULL,'http://www.spreaker.com/show/2093919/episodes/feed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('he RC Newb Podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('The RC Newb Podcast',NULL,NULL,'The RC Newb Podcast',0,0,NULL,'https://rcnewb.com/feed/podcast/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('P7 MIX Maraton - podcast',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('P7 MIX Maraton - podcast',NULL,NULL,'P7 MIX Maraton',0,0,NULL,'https://www.dr.dk/mu/feed/p7-maraton.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('going linux',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('going linux',NULL,NULL,'going linux',0,0,NULL,'http://goinglinux.com/mp3podcast.xml',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Fire forførende radiofortællinger',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Fire forførende radiofortællinger',NULL,NULL,'Fire forførende radiofortællinger',0,0,NULL,'https://www.dr.dk/mu/feed/fire-forfoerende-radiofortaellinger.xml?format=podcast&limit=500',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Prompt',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Prompt',NULL,NULL,'Prompt',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/prompt.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Den gode tone',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Den gode tone',NULL,NULL,'Den gode tone',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/den-gode-tone.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Bogselskabet',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Bogselskabet',NULL,NULL,'Bogselskabet',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/bogselskabet.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('De 169 piger',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('De 169 piger',NULL,NULL,'De 169 piger',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/de-169-piger.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Den største kamel',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Den største kamel',NULL,NULL,'Den største kamel',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/slug-kamelen.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Dillermand og urtekusse',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Dillermand og urtekusse',NULL,NULL,'Dillermand og urtekusse',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/dillermand-og-urtekusse.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Feature',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Feature',NULL,NULL,'Feature',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/feature.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Generation Mars',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Generation Mars',NULL,NULL,'Generation Mars',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/generation-mars.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Go Morgen P3',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Go Morgen P3',NULL,NULL,'Go Morgen P3',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/gomorgenp3.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Anmelderne',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Anmelderne',NULL,NULL,'Anmelderne',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/anmelderne.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Orientering Udsyn',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Orientering Udsyn',NULL,NULL,'Orientering Udsyn',0,0,NULL,'https://api.dr.dk/podcasts/v1/feeds/orientering.xml?format=podcast',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
  "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('Joshua Bardwell',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('Joshua Bardwell',NULL,NULL,'Joshua Bardwell',0,0,NULL,'https://www.patreon.com/rss/thedroneracingengineer?auth=JS4Yb-OaQVI2Q_LsIGb9jX9imuA8eGbx&show=931863',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)"
};


// "REPLACE INTO mythtvcontroller.internetcontent(name,thumbnail,type,author,description,commandline,version,updated,search,tree,podcast,download,active,host) VALUES ('title',NULL,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)","REPLACE INTO mythtvcontroller.internetcontentarticles (feedtitle,path,paththumb,title,season,episode,description,url,type,thumbnail,mediaURL,author,date,time,rating,filesize,player,playerargs,download,downloadargs,width,height,language,podcast,downloadable,customhtml,countries) VALUES ('title',NULL,NULL,'beskrivelse',0,0,NULL,'podcasturl',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)"

// ****************************************************************************************
//
// constructor
//
// ****************************************************************************************

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

// ****************************************************************************************
//
// destructor
//
// ****************************************************************************************

stream_class::~stream_class() {
    clean_stream_oversigt();
}


// ****************************************************************************************
//
// return the name
//
// ****************************************************************************************

char *stream_class::get_stream_name(int nr) {
  if (nr<antal) return (stack[nr]->feed_name); else return (NULL);
}

// ****************************************************************************************
//
// return the description
//
// ****************************************************************************************

char *stream_class::get_stream_desc(int nr) {
  if (nr<antal) return (stack[nr]->feed_desc); else return (NULL);
}

// ****************************************************************************************
//
// clean up number of created
//
// ****************************************************************************************
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

// ****************************************************************************************
//
// set en stream icon image
//
// ****************************************************************************************
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

// ****************************************************************************************
//
// get length on stream
//
// ****************************************************************************************

unsigned long stream_class::get_length_in_ms() {
  vlc_controller::get_length_in_ms();
  return(1);
}

// ****************************************************************************************
//
// jump in player
//
// ****************************************************************************************

float stream_class::jump_position(float ofset) {
    ofset=vlc_controller::jump_position(ofset);
    return(ofset);
}

// to play streams from web
//vlc_m = libvlc_media_new_location(vlc_inst, "http://www.ukaff.ac.uk/movies/cluster.avi");

// ****************************************************************************************
//
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
  std::string sqlinsert;
  MYSQL *conn;
  MYSQL_RES *res,*res1;
  MYSQL_ROW row;
  char *database = (char *) "mythtvcontroller";
  conn=mysql_init(NULL);
  // get homedir
  try {
    if (conn) {
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      sqlinsert = fmt::format("update mythtvcontroller.internetcontentarticles set time=time+1 where mediaURL like '{}'",url);
      mysql_query(conn,sqlinsert.c_str());
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
  std::string sqlselect1;
  char sqlinsert[32768];
  char totalurl[2048];
  char parsefilename[2048];
  char homedir[2048];
  char baseicon[2048];
  char temptxt[2024];
  unsigned int recantal=0;
  MYSQL *conn;
  MYSQL_RES *res,*res1;
  MYSQL_ROW row;
  time_t timenow;
  char *database = (char *) "mythtvcontroller";
  struct stat attr;
  const int updateinterval=86400;  
  time(&timenow);
  bool set_update_rss=false;
  conn=mysql_init(NULL);
  // get homedir
  strcpy(homedir,localuserhomedir);
  strcat(homedir,"/rss");
  if (!(file_exists(homedir))) mkdir(homedir,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  strcat(homedir,"/images");
  if (!(file_exists(homedir))) mkdir(homedir,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (configrssguidelastupdate==0) {    
    configrssguidelastupdate=time(NULL);
    set_update_rss=false;
  } else {
    if ((configrssguidelastupdate+600)<=time(NULL)) {
      set_update_rss=true;
      //configrssguidelastupdate=time(NULL);
    }
  }

  if (updaterssfile==true) set_update_rss=true; // force update
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
  if (conn) {
    // create db
    snprintf(sqlselect,sizeof(sqlselect),"CREATE TABLE IF NOT EXISTS mythtvcontroller.internetcontentarticles(feedtitle varchar(255),path text,paththumb text,title varchar(255),season smallint(5) DEFAULT 0,episode smallint(5) DEFAULT 0,description text,url text,type smallint(3),thumbnail text,mediaURL text,author varchar(255),date datetime,time int(11),rating varchar(255),filesize bigint(20),player varchar(255),playerargs text,download varchar(255),downloadargs text,width smallint(6),height smallint(6),language varchar(128),podcast tinyint(1),downloadable tinyint(1),customhtml tinyint(1),countries varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=60 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
    if (mysql_query(conn,sqlselect)!=0) {
      printf("mysql create table error.\n");
      printf("SQL : %s\n",sqlselect);
    }
    res = mysql_store_result(conn);
    // create db
    snprintf(sqlselect,sizeof(sqlselect),"CREATE TABLE IF NOT EXISTS mythtvcontroller.internetcontent(name varchar(255),thumbnail varchar(255),type smallint(3),author varchar(128),description text,commandline text,version double,updated datetime,search tinyint(1),tree tinyint(1),podcast tinyint(1),download tinyint(1),host varchar(128),active int,id int NOT NULL AUTO_INCREMENT PRIMARY KEY,INDEX Idx (name (15),thumbnail (15),type, author (15),  description (15),commandline (15),version,updated,search ,tree,podcast,download,host (15))) ENGINE=MyISAM AUTO_INCREMENT=60 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
    if (mysql_query(conn,sqlselect)!=0) {
      printf("mysql create table error.\n");
      printf("SQL : %s\n",sqlselect);
    }
    // create index
    // snprintf(sqlselect,sizeof(sqlselect),"CREATE INDEX IF NOT EXISTS `internetcontentarticles_feedtitle`  ON `mythtvcontroller`.`internetcontentarticles` (feedtitle) COMMENT '' ALGORITHM DEFAULT LOCK DEFAULT");
    sqlselect1 = "CREATE INDEX IF NOT EXISTS `internetcontentarticles_feedtitle`  ON `mythtvcontroller`.`internetcontentarticles` (feedtitle) COMMENT '' ALGORITHM DEFAULT LOCK DEFAULT";
    if (mysql_query(conn,sqlselect1.c_str())!=0) {
      printf("mysql create index error.\n");
    }
    // set charset to utf8mb4
    sqlselect1 = "ALTER TABLE internetcontentarticles CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci";
    if (mysql_query(conn,sqlselect1.c_str())!=0) {
      printf("mysql create index error.\n");
    }
  }
  if (set_update_rss) printf("set_update_rss %d configrssguidelastupdate = %ld time = %ld \n",set_update_rss,configrssguidelastupdate,time(NULL));
  if ((conn) && (set_update_rss)) {
    // mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
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
        printf("Get rss file %10s \n",row[0]);
        snprintf(temptxt,sizeof(temptxt),"Get rss feed title %10s ",row[0]);
        write_logfile(logfile,temptxt);
        if ((row[3]) && (strcmp(row[3],"")!=0)) {
          strcpy(homedir,localuserhomedir);                                   // Get user homedir
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
              // and get base image from func call (baseicon (url to image))
              parsexmlrssfile(parsefilename,baseicon);
              //printf("... Parse %s rss file \n",parsefilename);
            } else {
              printf("XML FILE is missing/not working on %s file.\n",row[3]);
            }
          }
          // update master icon if none
          if ((strcmp(row[0],"")!=0) && (strcmp(baseicon,"")!=0)) {
            snprintf(sqlinsert,sizeof(sqlinsert),"UPDATE internetcontentarticles set paththumb='%s' where feedtitle like '%s' and paththumb IS NULL",baseicon,row[0]);
            mysql_query(conn,sqlinsert);
            res1 = mysql_store_result(conn);
            haveupdated=true;
          }
          // if podcast is not rss and title ok
          if ((strcmp(row[3],"")!=0) && (row[23])) {
            if (atoi(row[23])==1) {
              snprintf(sqlinsert,sizeof(sqlinsert),"UPDATE internetcontentarticles set mediaURL=url where podcast=1 and feedtitle like '%s'",row[0]);
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
// search and replace used here
//
// ****************************************************************************************


void search_and_replace2(char *text) {
  int n=0;
  std::string newtext;
  newtext="";
  while(n<strlen(text)) {
    if (text[n]=='"') {
      newtext.append("'");
      n++;
    } else {
      newtext.append(1, (char ) text[n]);
      n++;
    }
  }
  strcpy(text,newtext.c_str());
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

  std::string sqlinsert1;
  std::string sqlselect1;
  std::string debuglogdata1;
  

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
                tmpdat=xmlGetProp(subnode,( xmlChar *) "url");
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
                  snprintf(sqlinsert,sizeof(sqlinsert),"select feedtitle from internetcontentarticles where (feedtitle like \"%s\" and mediaURL like '%s')",rssprgtitle,rssvideolink);
                  mysql_query(conn,sqlinsert);
                  res = mysql_store_result(conn);
                  if (res) {
                    while ((row = mysql_fetch_row(res)) != NULL) {
                      recordexist=true;
                    }
                  }
                  // create record if not exist
                  if (!(recordexist)) {
                    search_and_replace2(rssprgtitle);
                    search_and_replace2(rssprgfeedtitle);
                    search_and_replace2(rssprgdesc);
                    // write debug log
                    debuglogdata1 = fmt::format("Podcast update title {} date {}", rssprgtitle,rssprgpubdate);
                    write_logfile(logfile,(char *) debuglogdata1.c_str());                  
                    snprintf(sqlinsert,sizeof(sqlinsert),"REPLACE into internetcontentarticles(feedtitle,mediaURL,title,episode,season,author,path,description,paththumb,date,time) values(\"%s\",'%s',\"%s\",%d,%d,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d)",rssprgtitle,rssvideolink,rssprgfeedtitle,rssepisode,rssseason,rssauthor,"",rssprgdesc,rssprgimage,rssopretdato,0);
                    std::string sqlinsert1 = fmt::format("REPLACE into internetcontentarticles(feedtitle,mediaURL,title,episode,season,author,path,description,paththumb,date,time) values(\"{}\",'{}',\"{}\",'{}','{}','{}','','{}','{}','{}')",rssprgtitle,rssvideolink,rssprgfeedtitle,rssepisode,rssseason,rssauthor,rssprgdesc,rssprgimage,rssopretdato,0);
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
              // snprintf(sqlinsert,sizeof(sqlinsert),"SELECT feedtitle from internetcontentarticles where (feedtitle like '%s' and mediaURL like '%s' and title like '%s')",rssprgtitle,rssvideolink,rssprgfeedtitle);
              sqlinsert1 = fmt::format("SELECT feedtitle from internetcontentarticles where (feedtitle like '{}' mediaURL like '{}' and title like \"{}\")",rssprgtitle,rssvideolink,rssprgfeedtitle);
              mysql_query(conn,sqlinsert1.c_str());
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
                // write debug log
                std::string debugdata1;
                debugdata1 = fmt::format("Podcast update title {} Date {}",rssprgtitle,rssprgpubdate);
                write_logfile(logfile,(char *) debuglogdata1.c_str());
                sqlinsert1 = fmt::format("REPLACE into internetcontentarticles(feedtitle,mediaURL,title,episode,season,author,path,description,paththumb,date,time) values('{}','{}','{}',{},{},'{}','{}','{}','{}','{}',{})",rssprgtitle,rssvideolink,rssprgfeedtitle,rssepisode,rssseason,rssauthor,"",rssprgdesc,rssprgimage1,rssopretdato,0);
                if (mysql_query(conn,sqlinsert1.c_str())!=0) {
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
    // write debug log
    snprintf(debuglogdata,sizeof(debuglogdata),"Error reading %s xmlfile 0 bytes long.",filename);
    write_logfile(logfile,(char *) debuglogdata);
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
    snprintf(sqlselect,sizeof(sqlselect),"SELECT count(name) from mythtvcontroller.internetcontent where active=1");
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
    snprintf(sqlselect,sizeof(sqlselect),"SELECT count(title) from internetcontentarticles where feedtitle like '%s'",typedata);
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
    snprintf(sqlselect,sizeof(sqlselect),"SELECT feedtitle from mythtvcontroller.internetcontentarticles where feedtitle like '%s' limit 1",rssname);
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
  std::string sqlselect1;
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
  int i=0;
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
      write_logfile(logfile,(char *) "Update rss files.");
      // thumbnail   = name of an local image file
      // commandline = Program to fetch content with
      // updated     = Time of last update
      if ((!(dbexist)) && (mysql_query(conn,"CREATE database if not exist mythtvcontroller")!=0)) {
        printf("mysql db create error.\n");
      }
      res = mysql_store_result(conn);
      // create db
      snprintf(sqlselect,sizeof(sqlselect),"CREATE TABLE IF NOT EXISTS mythtvcontroller.internetcontentarticles(feedtitle varchar(255),path text,paththumb text,title varchar(255),season smallint(5) DEFAULT 0,episode smallint(5) DEFAULT 0,description text,url text,type smallint(3),thumbnail text,mediaURL text,author varchar(255),date datetime,time int(11),rating varchar(255),filesize bigint(20),player varchar(255),playerargs text,download varchar(255),downloadargs text,width smallint(6),height smallint(6),language varchar(128),podcast tinyint(1),downloadable tinyint(1),customhtml tinyint(1),countries varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=60 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
      if (mysql_query(conn,sqlselect)!=0) {
        printf("mysql create table error.\n");
        printf("SQL : %s\n",sqlselect);
      }
      res = mysql_store_result(conn);
      // create db
      snprintf(sqlselect,sizeof(sqlselect),"CREATE TABLE IF NOT EXISTS mythtvcontroller.internetcontent(name varchar(255),thumbnail varchar(255),type smallint(3),author varchar(128),description text,commandline text,version double,updated datetime,search tinyint(1),tree tinyint(1),podcast tinyint(1),download tinyint(1),host varchar(128),active int,id int NOT NULL AUTO_INCREMENT PRIMARY KEY,INDEX Idx (name (15),thumbnail (15),type, author (15),  description (15),commandline (15),version,updated,search ,tree,podcast,download,active,host (15))) ENGINE=MyISAM AUTO_INCREMENT=60 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
      if (mysql_query(conn,sqlselect)!=0) {
        printf("mysql create table error.\n");
        printf("SQL : %s\n",sqlselect);
      }
      if (!(dbexist)) {
        // create index
        snprintf(sqlselect,sizeof(sqlselect),"CREATE INDEX `internetcontentarticles_feedtitle`  ON `mythtvcontroller`.`internetcontentarticles` (feedtitle) COMMENT '' ALGORITHM DEFAULT LOCK DEFAULT");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql create index error.\n");
        }
        // set charset to utf8mb4
        sqlselect1= "ALTER TABLE internetcontentarticles CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci";
        if (mysql_query(conn,sqlselect1.c_str())!=0) {
          printf("mysql create index error.\n");
        }
      }
      res = mysql_store_result(conn);
      // Create records
      i=0;
      int array_size=sql_podcast_array.size()/2;
      while(i<array_size) {
        if (mysql_query(conn,sql_podcast_array.at(i).c_str())!=0) {
          printf("mysql insert error. %s \n",sql_podcast_array[i].c_str());
        }
        res = mysql_store_result(conn);
        mysql_free_result(res); 
        if (mysql_query(conn,sql_podcast_array[i+1].c_str())!=0) {
          printf("mysql insert error. %s \n",sql_podcast_array[i+1].c_str());
        }
        res = mysql_store_result(conn);
        mysql_free_result(res); 
        i+=2;
      }      
    }    
    if (conn) mysql_close(conn);
    // download new rrs files we just insert in db
    // and update db from rss file
    if (!(dbexist)) loadrssfile(1);
  }
  clean_stream_oversigt();                // clean old list
  strcpy(lasttmpfilename,"");    					// reset
  write_logfile(logfile,(char *) "loading rss/stream data from db.");
  //printf("art = %s tpath = %s \n",art,fpath);
  // find records after type to find
  if ((strcmp(art,"")==0) && (strcmp(fpath,"")==0)) {
    // select internetcontentarticles.feedtitle,
    sqlselect1 = "select (internetcontentarticles.feedtitle) as feedtitle,(internetcontentarticles.path) as path,(internetcontentarticles.title) as title,(internetcontentarticles.description) as description,(internetcontentarticles.url) as url,(internetcontent.thumbnail),count(internetcontentarticles.feedtitle) as counter,(internetcontent.thumbnail) as thumbnail,(internetcontentarticles.time) as nroftimes,(internetcontentarticles.paththumb) from internetcontentarticles left join internetcontent on internetcontentarticles.feedtitle=internetcontent.name where mediaURL is NOT NULL AND (internetcontent.active=1) group by (internetcontent.name) ORDER BY feedtitle,title DESC";
    getart=0;
  } else if ((strcmp(art,"")!=0) && (strcmp(fpath,"")==0)) {
    sqlselect1 = "select (feedtitle) as feedtitle,(path) as path,(title) as title,(description),(url),(thumbnail),count(path),(paththumb),(mediaURL),(time) as nroftimes,(id) as id from internetcontentarticles where mediaURL is NOT NULL and feedtitle like '";
    sqlselect1 = sqlselect1 + art;
    sqlselect1 = sqlselect1 + "' GROUP BY title ORDER BY id";      
    getart=1;
  } else if ((strcmp(art,"")!=0) && (strcmp(fpath,"")!=0)) {
    sqlselect1 = "select feedtitle as feedtitle,(path) as path,(title) as title,(description),(url),(thumbnail),(paththumb),(time) as nroftimes,(id) as id from internetcontentarticles where mediaURL is NULL and feedtitle like '";
    // strcat(sqlselect,art);
    sqlselect1 = sqlselect1 + art;
    sqlselect1 = sqlselect1 + "' AND path like '";
    sqlselect1 = sqlselect1 + fpath;
    sqlselect1 = sqlselect1 + "' ORDER BY abs(title) desc";
    // strcat(sqlselect,"' AND path like '");
    // strcat(sqlselect,fpath);
    // strcat(sqlselect,"' ORDER BY abs(title) desc"); // ASC
    getart=2;
  }
  /*
  printf("\n");
  printf("SQL SELECT ****************************** : %s\n",sqlselect1.c_str());
  printf("\n");
  */
  this->type=getart;					// husk sql type
  write_logfile(logfile,(char *) "RSS stream loader started.");
  conn=mysql_init(NULL);
  // Connect to database
  if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
    // get antal streams
    antalrss_feeds=get_antal_rss_feeds_sources(conn);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    if (mysql_query(conn,sqlselect1.c_str())!=0) {
      printf("mysql insert error.\n");
      printf("SQL %s \n",sqlselect1.c_str());
    }
    res = mysql_store_result(conn);
    if (res) {
      while (((row = mysql_fetch_row(res)) != NULL) && (antal<maxantal)) {
        //if (debugmode & 4) printf("Get info from stream nr %d %-20s\n",antal,row[2]);
        if (antal<maxantal) {
          stack[antal]=new (struct stream_oversigt_type);
          if (stack[antal]) {
            strcpy(stack[antal]->feed_showtxt,"");          	        // show name
            strcpy(stack[antal]->feed_name,"");		        // mythtv db feedtitle
            strcpy(stack[antal]->feed_desc,"");                       // desc
            strcpy(stack[antal]->feed_path,"");                       // mythtv db path
            strcpy(stack[antal]->feed_gfx_url,"");			//
            strcpy(stack[antal]->feed_gfx_mythtv,"");			//
            strcpy(stack[antal]->feed_streamurl,"");			//
            stack[antal]->feed_group_antal=0;				//
            stack[antal]->feed_path_antal=0;				//
            stack[antal]->textureId=0;				//
            stack[antal]->intnr=0;					//
            stack[antal]->nyt=false;					// New podcast ?
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
              get_webfilenamelong(downloadfilename,tmpfilename);                              // get file name from url to gfx
              // check filename
              strcpy(downloadfilename1,downloadfilename);                 // back name before change
              int mmm=0;
              while(mmm<strlen(downloadfilename)) {
                if ((downloadfilename[mmm]=='?') || (downloadfilename[mmm]=='&') || (downloadfilename[mmm]=='=')) downloadfilename[mmm]='_';
                mmm++;
              }
              //getuserhomedir(homedir);                                                  // get homedir
              strcpy(homedir,localuserhomedir);
              strcpy(downloadfilenamelong,homedir);
              strcat(downloadfilenamelong,"/rss/images/");
              strcat(downloadfilenamelong,downloadfilename);
              if (!(file_exists(downloadfilenamelong))) {
                // download gfx file and use as icon
                if (get_webfile2(tmpfilename,downloadfilenamelong)==-1) {
                  //printf("Download error \n");
                  write_logfile(logfile,(char *) "Image download error.");
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
                    //getuserhomedir(homedir);                                  // get homedir
                    strcpy(homedir,localuserhomedir);
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
                // strcpy(stack[antal]->feed_streamurl,row[4]);	// stream url
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
      write_logfile(logfile,(char *) "No stream data loaded.");
    }
    // load all the data in phread datainfoloader
    // web gfx file loader in phread
    loadweb_stream_iconoversigt();
    return(antal-1);
  } else printf("Failed to update feed stream db, can not connect to database: %s Error: %s\n",dbname,mysql_error(conn));
  write_logfile(logfile,(char *) "RSS/PODCAST loader done.");
  return(0);
}



// ****************************************************************************************
//
// thread web loader (loading all icons)
//
// ****************************************************************************************


void *loadweb(void *data) {
  write_logfile(logfile,(char *) "Start web icon loader thread.");
  streamoversigt.loadweb_stream_iconoversigt();
  write_logfile(logfile,(char *) "End/Stop web icon loader thread.");
}



// ****************************************************************************************
// in use
// downloading all rss podcast icon gfx
//
// ****************************************************************************************

int stream_class::loadweb_stream_iconoversigt() {
  std::string tmpfilename;
  int antal;
  int nr=0;
  int loadstatus;
  char downloadfilename[2900];
  char downloadfilenamelong[5000];
  char homedir[200];
  antal=this->streamantal();
  this->gfx_loaded=false;
  write_logfile(logfile,(char *) "RSS stream graphic download start.");
  while(nr<antal) {
    if (strcmp(stack[nr]->feed_gfx_mythtv,"")!=0) {
      loadstatus=0;
      // return downloadfilename from stack[nr]->feed_gfx_mythtv
      tmpfilename=stack[nr]->feed_gfx_mythtv;
      if (strncmp(tmpfilename.c_str(),"http://",7)==0) {
        // download file from web
        // return dowloadfilebame = file downloaded name no path
        get_webfilename(downloadfilename,(char *) tmpfilename.c_str());
        // add download path
        // add download filename to class opbject
        //getuserhomedir(downloadfilenamelong);
        strcpy(downloadfilenamelong,localuserhomedir);
        strcat(downloadfilenamelong,"/rss/images/");
        strcat(downloadfilenamelong,downloadfilename);
        if ((!(file_exists(downloadfilenamelong))) && (check_zerro_bytes_file(downloadfilenamelong)==0))  {
          if (debugmode & 4) printf("nr %3d Downloading : %s \n",nr,(char *) tmpfilename.c_str());
          loadstatus=get_webfile((char *) tmpfilename.c_str(),downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
        } else {
          if (!(file_exists(downloadfilenamelong))) loadstatus=get_webfile2((char *) tmpfilename.c_str(),downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
            //printf("File exist %s then set filename \n",downloadfilenamelong);
        }
      } else if (strncmp((char *) tmpfilename.c_str(),"https://",8)==0) {
        //strcpy(lastfile,downloadfilename);
        get_webfilename(downloadfilename,(char *) tmpfilename.c_str());
        //getuserhomedir(downloadfilenamelong);
        strcpy(downloadfilenamelong,localuserhomedir);

        // build path
        strcat(downloadfilenamelong,"/rss/images/");
        strcat(downloadfilenamelong,downloadfilename);
        // Download stream gfx if not exist in rss/image dir
        if ((!(file_exists(downloadfilenamelong))) && (check_zerro_bytes_file(downloadfilenamelong)==0)) {
          if (debugmode & 4) printf("nr %3d Downloading : %s \n",nr,(char *) tmpfilename.c_str());
          loadstatus=get_webfile2((char *) tmpfilename.c_str(),downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
        } else {
          if (!(file_exists(downloadfilenamelong))) loadstatus=get_webfile2((char *) tmpfilename.c_str(),downloadfilenamelong);
          // file downloaded
          if (loadstatus==0) {
            strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
            //snprintf(tmpfilename,sizeof(tmpfilename),"RSS stream graphic download file %s",downloadfilenamelong);
            tmpfilename = "RSS stream graphic download file ";
            tmpfilename = tmpfilename + downloadfilenamelong;
            write_logfile(logfile,(char *) tmpfilename.c_str());
          } else strcpy(stack[nr]->feed_gfx_mythtv,"");
        }
      }
      // set recordnr loaded info to update users view
      this->stream_oversigt_nowloading=nr;
    }
    nr++;
  }
  if (nr>0) this->gfx_loaded=true; else this->gfx_loaded=false;
  if (gfx_loaded) write_logfile(logfile,(char *) "RSS stream graphic download end.");
  else write_logfile(logfile,(char *) "RSS stream graphic download error.");
  return(1);
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
// show stream overview
// 
// ****************************************************************************************


void stream_class::show_stream_oversigt(GLuint normal_icon,GLuint empty_icon,GLuint empty_icon1,int _mangley,int stream_key_selected) {
  int j,ii,k,pos;
  // float buttonsizey=160.0f;                                                   // button size
  // float buttonsizex=200.0f;                                                   // button size
  float buttonsizex=config_menu.config_stream_main_window_icon_sizex;
  float buttonsizey=config_menu.config_stream_main_window_icon_sizey;
  float yof=orgwinsizey-(buttonsizey);                                        // start ypos
  float xof=0.0f;
  xof=config_menu.config_stream_main_windowx;                     // start xpos
  int xx=(float) config_menu.config_stream_main_window_sizex/(buttonsizex);
  int yy=(float) (config_menu.config_stream_main_window_sizey/buttonsizey);
  int lstreamoversigt_antal=8*5;
  // int lstreamoversigt_antal = xx*(yy-1);        // 
  int i=0;                                                                    // data ofset in stack array
  unsigned int sofset=0;
  int bonline=8;                                                              // antal pr linie
  float boffset;
  char gfxfilename[200];
  std::string gfxfilename1;
  char downloadfilename[200];
  std::string downloadfilename1;
  char downloadfilenamelong[1024];
  std::string downloadfilenamelong1;
  char *gfxshortnamepointer;
  char gfxshortname[200];
  std::string gfxshortname1;
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
  bool show_round_corner=false;
  std::string temprgtxt1;
  int cx=100+xof;                 // pos x
  int cy=80+yof;                  // pos y
  int dx=buttonsizex-20;          // siz y
  int dy=buttonsizey-30;          // siz x
  int r=20;                       // radius    
  float x0,y0,x,y,a=0.0;
  const int n=9;
  float da=1.5707963267948966192313216916398/float(n);
  int loop;
  if (stream_oversigt_loaded_nr==0) strcpy(downloadfilename_last,"");
  if ((this->streamantal()) && (stream_oversigt_loaded==false) && (this->stream_oversigt_loaded_nr<this->streamantal())) {
    if (stack[stream_oversigt_loaded_nr]) {
      strcpy(gfxfilename,stack[stream_oversigt_loaded_nr]->feed_gfx_mythtv);
      gfxfilename1 = stack[stream_oversigt_loaded_nr]->feed_gfx_mythtv;
    } else {
      strcpy(gfxfilename,"");
      gfxfilename1 = "";
    }
    strcpy(gfxshortname,"");
    gfxshortname1 = "";
    gfxshortnamepointer=strrchr(gfxfilename,'.');     // get last char = type of file     
    if (gfxshortnamepointer) {
      strcpy(gfxshortname,gfxshortnamepointer);
      gfxshortname1 = gfxshortnamepointer;
    }
    // load texture if none loaded
    // get_texture return 0 if not loaded
    if (get_texture(stream_oversigt_loaded_nr)==0) {
      if (strlen(gfxfilename)<200) {
        if (strcmp(gfxfilename1.c_str(),"")!=0) {
          // check om der findes en downloaded icon
          strcpy(downloadfilenamelong,"");
          downloadfilenamelong1 = "";
          downloadfilenamelong1 = downloadfilenamelong1 + gfxfilename1;
          strcat(downloadfilenamelong,gfxfilename);
          if (strcmp(downloadfilename_last,gfxfilename1.c_str())!=0) {
            // check om filen findes i cache dir eller i mythtv netvision dir
            if (file_exists(gfxfilename1.c_str())) {
              texture=loadTexture ((char *) gfxfilename1.c_str());
              if (texture) set_texture(stream_oversigt_loaded_nr,texture);
              last_texture=texture;
              antal_loaded+=1;
            } else if (file_exists(downloadfilenamelong1.c_str())) {
              // er det ikke samme texture som sidst loaded så load it
              // else set last used
              texture=loadTexture ((char *) downloadfilenamelong1.c_str());
              if (texture) set_texture(stream_oversigt_loaded_nr,texture);
              last_texture=texture;
              antal_loaded+=1;
            } else texture=0;
          } else {
            if (last_texture) set_texture(stream_oversigt_loaded_nr,last_texture);
            antal_loaded+=1;
            std::string temptxt2;
            temptxt2 = fmt::format("RSS stream graphic download file {}",downloadfilenamelong);
            write_logfile(logfile,(char *) temptxt2.c_str());
          }
          // husk last file name
          strcpy(downloadfilename_last,downloadfilenamelong1.c_str());
        }
      }
    }
    // downloading ?
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
      yof=yof-(config_menu.config_stream_main_window_icon_sizey+20);
      xof=config_menu.config_stream_main_windowx;
    }
    // selected biger
    if (i+1==(int) stream_key_selected) {
      buttonsizey=config_menu.config_stream_main_window_icon_sizey;
      buttonsizex=config_menu.config_stream_main_window_icon_sizex;
      show_round_corner = true;
    } else {
      buttonsizey=config_menu.config_stream_main_window_icon_sizey;
      buttonsizex=config_menu.config_stream_main_window_icon_sizex;
      show_round_corner = false;
    }
    if (stack[i+sofset]->textureId) {
      // stream icon exist draw it
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      glBindTexture(GL_TEXTURE_2D,empty_icon1);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // round corner
      if (show_round_corner) {
        cx=100+xof;                 // pos x
        cy=80+yof;                  // pos y
        dx=buttonsizex-20;          // siz y
        dy=buttonsizey-30;          // siz x
        r=20;                       // radius
        a=0.0;
        da=1.5707963267948966192313216916398/float(n);
        dx-=r+r;
        dy-=r+r;
        loop=0;
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0.5f, 0.5f); 
        glVertex2f(cx,cy);
        x0=cx+(0.5*dx);
        y0=cy+(0.5*dy);
        for (loop=0;loop<n;loop++,a+=da) {
            x=x0+(r*cos(a));
            y=y0+(r*sin(a));
            glTexCoord2f(0, 0); 
            glVertex2f(x,y);
        }
        x0-=dx;
        for (loop=0;loop<n;loop++,a+=da) {
            x=x0+(r*cos(a));
            y=y0+(r*sin(a));
            glVertex2f(x,y);
        }
        y0-=dy;
        for (loop=0;loop<n;loop++,a+=da) {
            x=x0+(r*cos(a));
            y=y0+(r*sin(a));
            glVertex2f(x,y);
        }
        x0+=dx;
        for (loop=0;loop<n;loop++,a+=da) {
            x=x0+(r*cos(a));
            y=y0+(r*sin(a));
            glVertex2f(x,y);
        }          
        glVertex2f(x,cy+(0.5*dy));
        glEnd();
      } else {
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsizex-10, yof+buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsizex-10, yof+10 , 0.0);
        glEnd();
      }

      glPushMatrix();
      // indsite draw icon rss gfx - SELECTED
      if (show_round_corner) {  

        /*
        cx=100+xof;          // pos x
        cy=80+yof;               // pos y
        dx=buttonsizex-20;          // siz y
        dy=buttonsizey-30;          // siz x
        r=20;            // radius
        a=0.0;
        da=1.5707963267948966192313216916398/float(n);
        dx-=r+r;
        dy-=r+r;
        loop=0;

        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0.5f, 0.5f); 
        glVertex2f(cx,cy);
        x0=cx+(0.5*dx);
        y0=cy+(0.5*dy);
        for (loop=0;loop<n;loop++,a+=da) {
            x=x0+(r*cos(a));
            y=y0+(r*sin(a));
            glTexCoord2f(0, 0); 
            glVertex2f(x,y);
        }
        x0-=dx;
        for (loop=0;loop<n;loop++,a+=da) {
            x=x0+(r*cos(a));
            y=y0+(r*sin(a));
            glVertex2f(x,y);
        }
        y0-=dy;
        for (loop=0;loop<n;loop++,a+=da) {
            x=x0+(r*cos(a));
            y=y0+(r*sin(a));
            glVertex2f(x,y);
        }
        x0+=dx;
        for (loop=0;loop<n;loop++,a+=da) {
            x=x0+(r*cos(a));
            y=y0+(r*sin(a));
            glVertex2f(x,y);
        }          
        glVertex2f(x,cy+(0.5*dy));
        glEnd();
        */
        
        // indsite draw icon rss gfx
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ONE);
        glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        if (tema==5) {
          glTexCoord2f(0, 0); glVertex3f( xof+25, yof+10, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+25,yof+buttonsizey-10, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsizex-25, yof+buttonsizey-10 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsizex-25, yof+10 , 0.0);
        } else {
          glTexCoord2f(0, 0); glVertex3f( xof+20, yof+20, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+20,yof+buttonsizey-30, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsizex-20, yof+buttonsizey-30 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsizex-20, yof+20 , 0.0);
        }
        glEnd();
      } else {
        // indsite draw icon rss gfx - NOT SELECTED
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
      }

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
      // no icon - draw default icon
      glPushMatrix();
      // indsite draw radio station icon
      if (show_round_corner) {  
        cx=100+xof;          // pos x
        cy=80+yof;               // pos y
        dx=buttonsizex-20;          // siz y
        dy=buttonsizey-30;          // siz x
        r=20;            // radius
        a=0.0;
        da=1.5707963267948966192313216916398/float(n);
        dx-=r+r;
        dy-=r+r;
        loop=0;

        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindTexture(GL_TEXTURE_2D,empty_icon1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0.5f, 0.5f); 
        glVertex2f(cx,cy);
        x0=cx+(0.5*dx);
        y0=cy+(0.5*dy);
        for (loop=0;loop<n;loop++,a+=da) {
            x=x0+(r*cos(a));
            y=y0+(r*sin(a));
            glTexCoord2f(0, 0); 
            glVertex2f(x,y);
        }
        x0-=dx;
        for (loop=0;loop<n;loop++,a+=da) {
            x=x0+(r*cos(a));
            y=y0+(r*sin(a));
            glVertex2f(x,y);
        }
        y0-=dy;
        for (loop=0;loop<n;loop++,a+=da) {
            x=x0+(r*cos(a));
            y=y0+(r*sin(a));
            glVertex2f(x,y);
        }
        x0+=dx;
        for (loop=0;loop<n;loop++,a+=da) {
            x=x0+(r*cos(a));
            y=y0+(r*sin(a));
            glVertex2f(x,y);
        }          
        glVertex2f(x,cy+(0.5*dy));
        glEnd();
      } else {
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
      }
      glPopMatrix();
    }
    // draw numbers in group
    if (stack[i+sofset]->feed_group_antal>1) {
      // show numbers in group
      temprgtxt1 = fmt::format("Feeds {}",stack[i+sofset]->feed_group_antal);
      drawText(temprgtxt1.c_str(), xof+22,yof+14, 0.3f,1);
    }
    // show text 20 of elements in string
    temprgtxt1 = fmt::format("{:^20}",stack[i+sofset]->feed_showtxt);
    temprgtxt1.resize(20);
    drawText(temprgtxt1.c_str(), xof+20,yof-10, 0.4f,1);
    // next button
    i++;
    xof+=(buttonsizex+10);
  }
  // no records loaded error
  //
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
    drawText("Please wait Loading ...", 700, 260, 0.4f,1);
  }
}
