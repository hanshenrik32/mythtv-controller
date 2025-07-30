
#include <libtorrent/session.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_handle.hpp>

#include <vector>
#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include "utility.h"
#include "myctrl_torrent.h"
#include "myctrl_glprint.h"


namespace lt = libtorrent;

using namespace std;


extern int orgwinsizey,orgwinsizex;
extern GLuint torrent_background;
extern GLuint _textureclose; 	                  // close icon
extern FILE *logfile;                           // global logfile
extern GLuint _texturemusicplayer;

// ****************************************************************************************
//
// glprint text
//
// ****************************************************************************************

void myglprint(char *string) {
  int len,i;
  len = (int) strlen(string);
  for (i = 0; i < len; i++) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
}

void myglprintbig(char *string) {
  int len,i;
  len = (int) strlen(string);
  for (i = 0; i < len; i++) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
}



// ****************************************************************************************
//
// Constructor
//
// ****************************************************************************************

torrent_loader::torrent_loader() {
  torrent_list_antal=0;
  for (int i=0;i<TORRENT_ANTAL-1;i++) {
    torrent_list[i].progress = 0;
    torrent_list[i].torrent_name = "";
    torrent_list[i].state_text = "";
    torrent_list[i].save_path = "/tmp/";
    torrent_list[i].active = false;
    torrent_list[i].downloaded = false;
    torrent_list[i].paused = false;
    torrent_list[i].total_wanted = 0;
    torrent_list[i].downloaded_size = 0;
  }
  edit_line_nr = 0;
  torrent_info_line_nr= 0 ;
}


// ****************************************************************************************
//
// Show trackers
//
// ****************************************************************************************

int torrent_loader::torrent_trackers() {
  try {
    if (!(this->torrentp.trackers.empty())) {
      std::printf("tracers:\n");
      auto tier_it = this->torrentp.tracker_tiers.begin();
      int tier = 0;
      for (auto const& i : this->torrentp.trackers) {
          if (tier_it != this->torrentp.tracker_tiers.end()) {
              tier = *tier_it;
              ++tier_it;
          }
          std::printf("%2d: %s\n", tier, i.c_str());
      }
    }
  } catch (const std::exception& e) {
  }
  std::cerr << "No tracker pt " << std::endl;        
  return(0);
}



// ****************************************************************************************
//
// Show notes
//
// ****************************************************************************************

int torrent_loader::torrent_nodes() {
  if (!(this->torrentp.dht_nodes.empty())) {
    std::printf("nodes:\n");
    for (auto const& i : this->torrentp.dht_nodes) {
      std::printf("%s: %d\n", i.first.c_str(), i.second);
    }
  }
  return(0);
}

// ****************************************************************************************
//
// Private: Set torrent file to download in thread
// used in load_torrent()
//
// ****************************************************************************************

int torrent_loader::add_torrent(char *filename) {
  std::string loginfo;
  // Start session
  // test code this->torrentp.save_path = "/tmp/";
  torrent_list[torrent_list_antal].torrent_name=filename;
  torrent_list[torrent_list_antal].active=true;
  this->torrentp.save_path = torrent_list[torrent_list_antal].save_path;
  torrent_list_antal++;
  try {
    this->torrentp.ti = std::make_shared<lt::torrent_info>((char *) filename);
    torrent_status=this->s.add_torrent(torrentp);
    handles.push_back(torrent_status);
  } catch (const std::exception& e) {
    std::cerr << "File torrent_loader.txt not found." << std::endl;
    loginfo=fmt::format("TORRENT: File torrent_loader.txt not found.");
    write_logfile(logfile,(char *) loginfo.c_str());
    return 1;
  }
  return 0;
}

// ****************************************************************************************
//
// Download torrent from file.
// And log it to log file
//
// ****************************************************************************************

int torrent_loader::load_torrent() {
  int antal=0;
  std::string tline;
  std::string loginfo;
  std::ifstream file("torrent_loader.txt");
  if (!(file.is_open())) {
    loginfo=fmt::format("TORRENT: torrent_loader.txt file not found.");
    write_logfile(logfile,(char *) loginfo.c_str());
    return(0);
  } else {
    while((!(file.eof())) && (antal<TORRENT_ANTAL)) {
      std::getline(file,tline);
      if (tline.length()>0) {
        if (file_exists(tline.c_str())) {
          add_torrent((char *) tline.c_str());
          loginfo=fmt::format("TORRENT: Downloading file {}",tline);
          write_logfile(logfile,(char *) loginfo.c_str());
        } else {
          loginfo=fmt::format("TORRENT: Torrent file {} do NOT exist.",tline);          
          write_logfile(logfile,(char *) loginfo.c_str());
        }
      }
      antal++;
    }
  }
  return(1);
}

// ****************************************************************************************
//
// Get progress
//
// ****************************************************************************************

float torrent_loader::get_progress(int nr) {
  return(torrent_list[0].progress);
}

// ****************************************************************************************
//
// progress opdate
//
// ****************************************************************************************

void torrent_loader::opdate_progress() {
  int tnr=0;
  static int checkit=0;
  checkit++;
  if (checkit==50) {
    checkit=0;
    if (torrent_list_antal>0) {
      for (auto& h : handles) {
        if (h.is_valid()) {
          h.set_upload_limit(1);
          // status = torrent_status.status(); 
          lt::torrent_status status = h.status();
          torrent_list[tnr].progress = status.progress * 100;    
          torrent_list[tnr].added_time = status.added_time;    
          torrent_list[tnr].torrent_name = status.name;
          torrent_list[tnr].total_wanted = status.total_wanted;
          torrent_list[tnr].downloaded_size = status.total_done;
          switch(status.state) {
            case libtorrent::torrent_status::checking_files:
              torrent_list[tnr].state_text = "Checking files...";
              break;
            case libtorrent::torrent_status::downloading:
              torrent_list[tnr].state_text = "Downloading...";
              break;
            case libtorrent::torrent_status::downloading_metadata:
              torrent_list[tnr].state_text = "Downloading metadata...";
              break;
            case libtorrent::torrent_status::seeding:
              torrent_list[tnr].state_text = "Done/Seeding.";
              if (torrent_list[tnr].active) {
                torrent_list[tnr].downloaded=true;
                printf("File done ******************************************************************\n");
                torrent_list[tnr].active=false;
              }
              break;
            case libtorrent::torrent_status::finished:
              torrent_list[tnr].state_text = "Done.";
              break;
            default:
              torrent_list[tnr].state_text = "No status";
          }
        }
        tnr++;
      }
    } else torrent_list[0].progress = 0.0f;
  }
}


// ****************************************************************************************
//
// Set torrent on running/pause
//
// ****************************************************************************************

void torrent_loader::pause_torrent(int nr) {
  printf("torrent_info_line_nr = %d \n ",torrent_info_line_nr);
  if (nr<torrent_list_antal) {
    if (torrent_list[nr].paused) {
      set_torrent_paused(nr,false);
      handles[nr].pause();                                       // pause in libtorrent
    } else {
      set_torrent_paused(nr,true);
      handles[nr].resume();                                      // resume in libtorrent
    }    
  }
}

// ****************************************************************************************
//
// Delete torrent
//
// ****************************************************************************************

void torrent_loader::delete_torrent(int nr) {
  if (nr<torrent_list_antal) {
    this->s.remove_torrent(handles[nr]);
    set_torrent_active(nr,false);                   // disable torrent
  }
}


// ****************************************************************************************
//
// Move torrent
//
// ****************************************************************************************

void torrent_loader::move_torrent(int nr) {
  if (nr<torrent_list_antal) {
    // set_torrent_active(nr,false);                   // disable torrent
  }
}

// ****************************************************************************************
//
// Draw cursor on screen at pos
//
// ****************************************************************************************

void coursornow(int cxpos,int cypos,int txtlength) {
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  cxpos+=4+(txtlength*9);
  cypos+=6;
  struct timespec timer;
  long lasttimer=0;
  static bool showcursor=false;
  clock_gettime(CLOCK_REALTIME, &timer);
  if (timer.tv_nsec>lasttimer+243600692) showcursor=true; else showcursor=false;
  if (showcursor) {
    lasttimer=timer.tv_nsec;
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(cxpos+((orgwinsizex/2)-(1200/2))+6,cypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(cxpos+((orgwinsizex/2)-(1200/2))+6,cypos+((orgwinsizey/2)-(800/2))+20, 0.0);
    glTexCoord2f(1, 1); glVertex3f(cxpos+((orgwinsizex/2)-(1200/2))+14,cypos+((orgwinsizey/2)-(800/2))+20 , 0.0);
    glTexCoord2f(1, 0); glVertex3f(cxpos+((orgwinsizex/2)-(1200/2))+14,cypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd();
  }
  glPopMatrix();
}


// ****************************************************************************************
//
// Show options from show_torrent
//
// ****************************************************************************************

void torrent_loader::show_torrent_options() {
  const char *options_text[]={"Delete","Move","Pause","Resume"};
  int xpos=0;
  int ypos=0;
  std::string showtxt;
  int xof=300;
  int yof=400;
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  glTranslatef(0.0f, 0.0f, 0.0f);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturemusicplayer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f( 225 + xof,100 + yof , 0.0);
  glTexCoord2f(0, 1); glVertex3f( 225 + xof,350 + yof, 0.0);
  glTexCoord2f(1, 1); glVertex3f( 225+350 + xof,350 + yof, 0.0);
  glTexCoord2f(1, 0); glVertex3f( 225+350 + xof,100 + yof, 0.0);
  glEnd();
  glPopMatrix();
  
  glPushMatrix();  
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  // glTranslatef(350 + xof, 280 + yof, 0.0f);
  xof=350;
  yof=600;
  glTranslatef(0.0f, 0.0f, 0.0f);
  for (int n=0;n<3;n++) {
    // glRasterPos2f(0.0f, 0.0f+(n*28));
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);
    if (n==torrent_info_line_nr) glColor3f(0.2f, 0.8f, 0.2f); else glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( 200 + xof,100 + yof , 0.0);
    glTexCoord2f(0, 1); glVertex3f( 200 + xof,130 + yof, 0.0);
    glTexCoord2f(1, 1); glVertex3f( 200+300 + xof,130 + yof, 0.0);
    glTexCoord2f(1, 0); glVertex3f( 200+300 + xof,100 + yof, 0.0);
    glEnd();
    yof=yof-50;
  }
  glColor3f(0.5f, 0.5f, 0.0f);
  glTranslatef(650.0f, 608.0f, 0.0f);
  yof=600;
  for (int n=0;n<3;n++) {
    glDisable(GL_TEXTURE_2D);
    glRasterPos2f(0.0f, 0.0f+(n*50));
    showtxt=fmt::format(" {:.25} ", options_text[n]);
    myglprintbig((char *) showtxt.c_str());    
  }
  
  glPopMatrix();    
}



// ****************************************************************************************
//
// Show torrent overview
//
// ****************************************************************************************

void torrent_loader::show_torrent_oversigt(int sofset,int key_selected) {
  const char *no_torrent_text[]={"No torrent.","ingen torrent.","Pas de torrent","Kein Torrent","لا سيل"};
  char keybuffer[200];
  int winsizx=1300;
  int winsizy=975;
  int xpos=0;
  int ypos=0;
  char text[200];
  char temptxt[200];
  int n;
  std::string showtxt;
  int startofset=0;
  static int do_show_setup_select_linie=0;
  // background
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  glTranslatef(0.0f, 0.0f, 0.0f);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,torrent_background);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // background
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f( 200,100 , 0.0);
  glTexCoord2f(0, 1); glVertex3f( 200,975 , 0.0);
  glTexCoord2f(1, 1); glVertex3f( 200+1300,975 , 0.0);
  glTexCoord2f(1, 0); glVertex3f( 200+1300,100 , 0.0);
  glEnd();
  glPopMatrix();
  // close buttons
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBindTexture(GL_TEXTURE_2D,_textureclose);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  winsizx=188;
  winsizy=81;
  xpos=400;
  ypos=-10;
  glLoadName(40);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  // list of torrent running status.
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(300, 480, 0.0f);
  glRasterPos2f(0.0f, 0.0f+(230.0f));
  myglprint((char *) "Name.                                                                              Progress.            Status.");
  for (int n=0;n<TORRENT_ANTAL-1;n++) {
    glRasterPos2f(0.0f, 0.0f+(200-(n*18)));
    if (torrent_list[n].active) {
      if (!(torrent_list[n].downloaded)) {
        if ((torrent_list[n].total_wanted/1024)>1024) {
          if ((torrent_list[n].total_wanted/1024/1024)>1024) {
            if (torrent_list[n].paused) {
              showtxt=fmt::format(" {:70} {:>6.2} %   {:4} of {:4} Mb {:>20} ", torrent_list[n].torrent_name, torrent_list[n].progress, torrent_list[n].downloaded_size/1024/1024,  torrent_list[n].total_wanted/1024/1024, "Paused");
            } else {
              showtxt=fmt::format(" {:70} {:>6.2} %   {:4} of {:4} Mb {:>20} ", torrent_list[n].torrent_name, torrent_list[n].progress, torrent_list[n].downloaded_size/1024/1024,  torrent_list[n].total_wanted/1024/1024, torrent_list[n].state_text);
            }
          } else {
            if (torrent_list[n].paused) {
              showtxt=fmt::format(" {:70} {:>6.2} %   {:4} of {:4} Kb {:>20} ", torrent_list[n].torrent_name, torrent_list[n].progress, torrent_list[n].downloaded_size/1024,  torrent_list[n].total_wanted/1024, "Paused");
            } else {
              showtxt=fmt::format(" {:70} {:>6.2} %   {:4} of {:4} Kb {:>20} ", torrent_list[n].torrent_name, torrent_list[n].progress, torrent_list[n].downloaded_size/1024,  torrent_list[n].total_wanted/1024, torrent_list[n].state_text);
            }
          }
        } else {
          if (torrent_list[n].paused) {
            showtxt=fmt::format(" {:70} {:>6.2} %   {:4} of {:4} B {:>20} ", torrent_list[n].torrent_name, torrent_list[n].progress, torrent_list[n].downloaded_size,  torrent_list[n].total_wanted, "Paused");
          } else {
            showtxt=fmt::format(" {:70} {:>6.2} %   {:4} of {:4} B {:>20} ", torrent_list[n].torrent_name, torrent_list[n].progress, torrent_list[n].downloaded_size,  torrent_list[n].total_wanted, torrent_list[n].state_text);
          }
        }
      } else showtxt=fmt::format(" {:70} {:>6.2} % {:>40} ",torrent_list[n].torrent_name,torrent_list[n].progress,"Downloaded.");
    } else {      
      showtxt=no_torrent_text[0];
    }
    myglprint((char *) showtxt.c_str());    
  }
  glPopMatrix();  
  coursornow(-70,528-((edit_line_nr)*18),0);
}

