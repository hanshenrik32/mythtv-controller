
#include <libtorrent/session.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
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

void myglprint(char *string) {
  int len,i;
  len = (int) strlen(string);
  for (i = 0; i < len; i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
}



// ****************************************************************************************
//
// Constructor
//
// ****************************************************************************************

torrent_loader::torrent_loader() {
  torrent_list_antal=0;
  for (int i=0;i<TORRENT_ANTAL-1;i++) {
    torrent_list[i].progress=0;
    torrent_list[i].torrent_name="";
    torrent_list[i].state_text="";
    torrent_list[i].save_path="/tmp/";
  }
}


// ****************************************************************************************
//
// show trackers
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
// private: Set torrent file to download in thread
//
// ****************************************************************************************

int torrent_loader::add_torrent(char *filename) {
  // Start session
  // test code this->torrentp.save_path = "/tmp/";
  torrent_list[torrent_list_antal].torrent_name=filename;
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
  if (torrent_list_antal>0) {
    for (auto& h : handles) {
      if (h.is_valid()) {
        // status = torrent_status.status(); 
        lt::torrent_status status = h.status();
        torrent_list[tnr].progress = status.progress * 100;    
        torrent_list[tnr].added_time = status.added_time;    
        torrent_list[tnr].torrent_name = status.name;
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
            torrent_list[tnr].state_text = "Seeding.";
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




void torrent_loader::show_torrent_oversigt(int sofset,int key_selected) {
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
  // list of torrent rinning status
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(300, 480, 0.0f);
  glRasterPos2f(0.0f, 0.0f+(230.0f));
  myglprint((char *) "Name                                                                                                          Progress                              Status");
  for (int n=0;n<19;n++) {  
    glRasterPos2f(0.0f, 0.0f+(200-(n*18)));
    if (torrent_list[n].torrent_name.length()>0) {      
      showtxt=fmt::format(" {:80} {:>5.4} % {:>40} ",torrent_list[n].torrent_name,torrent_list[n].progress,torrent_list[n].state_text);
    } else {
      showtxt=fmt::format(" No torrent. ");
    }
    myglprint((char *) showtxt.c_str());    
  }
  glPopMatrix();  
   // showcoursornow(-70,510-(0*20),0);
}

