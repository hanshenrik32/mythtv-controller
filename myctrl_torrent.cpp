
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
#include <filesystem>
#include <string>
#include "utility.h"
#include "myctrl_torrent.h"
#include "myctrl_glprint.h"


namespace lt = libtorrent;

using namespace std;
namespace fs = std::filesystem;

extern int orgwinsizey,orgwinsizex;
extern GLuint torrent_background;
extern GLuint _textureclose; 	                  // close icon
extern GLuint _textureloadfile;
extern FILE *logfile;                           // global logfile
extern GLuint _texturemusicplayer;
extern bool do_show_torrent_options_move;
extern float do_move_torrent_file_now_done;               // is it running now
extern std::string do_show_load__torrent_file_string;

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

// ****************************************************************************************
//
// glprint text big
//
// ****************************************************************************************


void myglprintbig(char *string) {
  int len,i;
  len = (int) strlen(string);
  for (i = 0; i < len; i++) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
}



// ****************************************************************************************
//
// Select file name
//
// call torrent_downloader.select_file_name();
// 
// ****************************************************************************************



void torrent_loader::select_file_name() {
  char filenamepath[1024];
  strcpy(filenamepath,"");
  std::string filenamepath1;
  std::string filename="";
  std::string tmp;
  std::string dest_file="";
  std::string realpath="";
  std::ofstream torrentfile;
  FILE *f = popen("/usr/bin/zenity --file-selection --file-filter=*.torrent --modal --title=\"Select torrent file.\" 2> /dev/null", "r");
  fgets(filenamepath, 1024, f);
  if (!(f)) {
    return;
  }
  fclose(f);
  if (strlen(filenamepath)>0) {
    // do_show_load__torrent_file_string=filenamepath;
    filename = fs::path(filenamepath).filename();
    filename.erase(std::remove(filename.begin(), filename.end(), '\n'), filename.cend());
    tmp = "/home/hans/datadisk/mythtv-controller-0.38/";
    tmp = tmp + filename;
    dest_file = tmp;
    filenamepath1 = filenamepath;
    filenamepath1.erase(std::remove(filenamepath1.begin(), filenamepath1.end(), '\n'), filenamepath1.cend());
    // copy torrent file to homedir
    ifstream source(filenamepath1, ios::binary);
    ofstream dest(dest_file, ios::binary);
    if (source) {
      dest << source.rdbuf();
      source.close();
      dest.close();
    }
    add_torrent((char *) filename.c_str());
    // add filename to torrlent_file.txt to load it after next reboot of mythtv-controller
    torrentfile.open("torrent_loader.txt", std::ios_base::app);
    if (torrentfile) {
      torrentfile << filename << "\n";
      torrentfile.close();
    }    
  }
}


// ****************************************************************************************
//
// Constructor
//
// ****************************************************************************************

torrent_loader::torrent_loader() {
  torrent_loader_struct torrent_info_data;
  torrent_list_antal=0;
  downloadpath = "/tmp/";                                       // default download path
  edit_line_nr = 0;
  torrent_info_line_nr= 0;
  torrent_info_move_line_nr = 0;
  automove_to_movie_path = true;
  trash_torrent = false;
  torrent_info_data.progress = 0;
  torrent_info_data.torrent_name = "";
  torrent_info_data.state_text = "";
  torrent_info_data.save_path = downloadpath;
  torrent_info_data.active = false;
  torrent_info_data.downloaded = false;
  torrent_info_data.paused = false;
  torrent_info_data.total_wanted = 0;
  torrent_info_data.downloaded_size = 0;
  torrent_info_data.is_finished = false;
  torrent_info_data.num_connections = 0;
  torrent_info_data.automove_done_to_moviepath = false;
  torrent_list.clear(); // Clear the vector before resizing
}


// ****************************************************************************************
//
// set torrent moved file flag to true
//
// ****************************************************************************************


void torrent_loader::set_automove_done(int nr) {
  torrent_list[nr].automove_done_to_moviepath = true;
}

// ****************************************************************************************
//
// get torrent moved file flag.
//
// ****************************************************************************************

bool torrent_loader::get_automove_done(int nr) {
  return(torrent_list[nr].automove_done_to_moviepath);
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
// Show nodes
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
  torrent_loader_struct torrent_info_data;
  std::string loginfo;
  // Start session
  // test code this->torrentp.save_path = "/tmp/";  
  torrent_list_antal++;
  torrent_info_data.torrent_name = filename;
  torrent_info_data.torrent_file_name = filename;
  torrent_info_data.save_path = "/tmp/";
  torrent_info_data.active = true;
  torrent_info_data.downloaded = false;
  torrent_info_data.paused = false;
  torrent_info_data.is_finished = false;
  torrent_info_data.num_connections = 0;
  torrent_info_data.state_text = "";
  torrent_info_data.progress = 0.0f;
  torrent_info_data.added_time = std::time(nullptr);
  torrent_info_data.total_wanted = 0;
  torrent_info_data.downloaded_size = 0;
  torrent_info_data.automove_done_to_moviepath = false;
  this->torrentp.save_path = torrent_info_data.save_path;
  pack.set_str(lt::settings_pack::listen_interfaces, "0.0.0.0:6881,[::]:6881");
  // add torrent info to vector
  torrent_list.push_back(torrent_info_data); // 
  // pack.set_bool(lt::settings_pack::enable_incoming_utp, true);
  // pack.set_bool(lt::settings_pack::enable_incoming_tcp, true);
  try {
    this->torrentp.ti = std::make_shared<lt::torrent_info>((char *) filename);
    torrent_status=this->s.add_torrent(torrentp);
    handles.push_back(torrent_status);                                                // add torrent handle to vector
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
    while(!(file.eof())) {
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
  return(antal);
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
// progress opdate set status
//
// ****************************************************************************************

void torrent_loader::opdate_progress() {
  int tnr=0;
  static int checkit=0;
  checkit++;
  if (checkit==20) {
    checkit=0;
    if (torrent_list_antal>0) {
      for (auto& h : handles) {
        if (h.is_valid()) {
          h.set_upload_limit(1);
          lt::torrent_status status = h.status();
          torrent_list.at(tnr).is_finished = status.is_finished;
          torrent_list.at(tnr).progress = status.progress * 100;
          torrent_list.at(tnr).added_time = status.added_time;
          torrent_list.at(tnr).torrent_name = status.name;
          torrent_list.at(tnr).total_wanted = status.total_wanted;
          torrent_list.at(tnr).downloaded_size = status.total_done;
          torrent_list.at(tnr).num_connections = status.num_connections;         
          switch(status.state) {
            case libtorrent::torrent_status::checking_files:
              torrent_list.at(tnr).state_text = "Checking files...";
              break;
            case libtorrent::torrent_status::downloading:
              torrent_list.at(tnr).state_text = "Downloading...";
              break;
            case libtorrent::torrent_status::downloading_metadata:
              torrent_list.at(tnr).state_text = "Downloading metadata...";
              break;
            case libtorrent::torrent_status::seeding:
              torrent_list.at(tnr).state_text = "Done/Seeding.";
              if (torrent_list.at(tnr).active) {
                if (status.is_finished) {
                  torrent_list.at(tnr).downloaded=true;
                  torrent_list.at(tnr).is_finished=true;
                }
              }
              break;
            case libtorrent::torrent_status::finished:
              torrent_list.at(tnr).state_text = "Done.";
              break;
            default:
              torrent_list.at(tnr).state_text = "No status";
          }
        }
        tnr++;
      }
    } else {
      torrent_list.at(0).progress = 0.0f;
    }
  }
}


// ****************************************************************************************
//
// Set torrent on running/pause
//
// ****************************************************************************************

void torrent_loader::pause_torrent(int nr) {
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
// Delete torrent from view and torrent_loader.txt
//
// ****************************************************************************************

bool torrent_loader::delete_torrent(int nr) {
  int n=0;
  std::string husknavn = torrent_list[nr].torrent_file_name;
  std::string line = "";
  if (nr<torrent_list_antal) {
    try {
      this->s.remove_torrent(handles[nr]);
    } catch (const std::exception& e) {
      cout << "error remove torrent " << std::endl;
    }
    set_torrent_active(nr,false);                   // disable torrent
    torrent_list.erase(torrent_list.begin() + nr); // Remove the torrent from the vector
    // clean up torrent file list.
    std::ifstream src("torrent_loader.txt", std::ios::binary);
    std::ofstream dest("torrent_loader_new.txt", std::ios::binary);
    if (!src.is_open()) {
        std::cerr << "Kunne ikke åbne source file: torrent_loader.txt\n";
        return false;
    }
    if (!dest.is_open()) {
        std::cerr << "Kunne ikke åbne destinations file: torrent_loader_new.txt\n";
        return false;
    }
    while(std::getline(src, line)) {
      if (line.compare(husknavn) != 0) {
        dest << line << endl;
      }
    }
    src.close();
    dest.close();
    if (std::rename("torrent_loader_new.txt", "torrent_loader.txt")) {
      std::cerr << "Kunne ikke rename file: torrent_loader_new.txt to torrent_loader.txt\n";
      return false;
    }
  }
  return(true);
}


// ****************************************************************************************
//
// Move torrent
//
// ****************************************************************************************

void torrent_loader::move_torrent(int nr) {
  if (nr<torrent_list_antal) {
    // set_torrent_active(nr,false);                   // disable torrent    
    do_show_torrent_options_move = true;
  }
}



// **********************************************************************************
//
// copy file
//
// **********************************************************************************

bool torrent_loader::copy_file(const std::string& source, const std::string& destination) {
  std::ifstream src(source, std::ios::binary);
  std::ofstream dst(destination, std::ios::binary);
  if (!src.is_open()) {
      std::cerr << "Kunne ikke åbne source file: " << source << "\n";
      return false;
  }
  if (!dst.is_open()) {
      std::cerr << "Kunne ikke åbne destinations file: " << destination << "\n";
      return false;
  }
  const std::size_t buffer_size = 4096; // 4 KB buffer
  std::vector<char> buffer(buffer_size);
  while (src) {
      src.read(buffer.data(), buffer.size());
      std::streamsize bytes_read = src.gcount();
      if (bytes_read > 0) {
          dst.write(buffer.data(), bytes_read);
          do_move_torrent_file_now_done = do_move_torrent_file_now_done + bytes_read;
      }
  }
  std::cout << "Filen blev kopieret fra '" << source << "' til '" << destination << "'\n";
  return true;
}


// **********************************************************************************
//
// copy directory
//
// **********************************************************************************

bool torrent_loader::copy_disk_entry(const std::string& source, const std::string& destination) {
  fs::path sourcePath(source);
  fs::path destPath(destination);
  fs::directory_entry Directory{source};
  if (Directory.is_directory()) {
    try {
      if (!fs::exists(destPath)) {
        fs::create_directory(destPath);
        // Gå gennem alle filer og mapper i source
        for (const auto& entry : fs::recursive_directory_iterator(sourcePath)) {
          const auto& path = entry.path();
          auto relativePath = fs::relative(path, sourcePath);
          auto targetPath = destPath / relativePath;
          if (fs::is_directory(path)) {
            fs::create_directories(targetPath); // opret undermapper
          } else if (fs::is_regular_file(path)) {
            std::string source_path_string{path.u8string()};
            std::string dest_path_string{targetPath.u8string()};
            copy_file(source_path_string,dest_path_string);
          } else {
            std::cerr << "Skipping non-regular file: " << path << '\n';
          }
        }
      }
    } catch (const fs::filesystem_error& e) {
      std::cerr << "Filesystem error: " << e.what() << '\n';
      return false;
    }
  } else {
    copy_file(source,destination);
  }
  return(true);
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
// opdate torrent called from thread started in main.
//
// ****************************************************************************************

void torrent_loader::opdate_torrent() {
  std::vector<lt::alert*> alerts;
  s.pop_alerts(&alerts);
  for (lt::alert* a : alerts) {
    std::cout << a->message() << std::endl;
  }
  // Sleep to avoid busy waiting
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}


// ****************************************************************************************
//
// Show status for file copy of torrent (called from main line 5906)
//
// ****************************************************************************************

void torrent_loader::show_file_move() {
  int xpos=20;
  int ypos=20;
  std::string showtxt;
  int xof=300;
  int yof=400;
  int xxof=0;
  float procentdone=0.0f;
  glPushMatrix();
  // background
  glEnable(GL_TEXTURE_2D);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturemusicplayer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f( xpos+225 + xof,ypos+180 + yof , 0.0);
  glTexCoord2f(0, 1); glVertex3f( xpos+225 + xof,ypos+250 + yof, 0.0);
  glTexCoord2f(1, 1); glVertex3f( xpos+225+400 + xof,ypos+250 + yof, 0.0);
  glTexCoord2f(1, 0); glVertex3f( xpos+225+400 + xof,ypos+180 + yof, 0.0);
  glEnd();
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  xof=350;
  yof=500;
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,0);
  glColor3f(0.2f, 0.8f, 0.2f);
  procentdone=do_move_torrent_file_now_done/torrent_list[edit_line_nr].total_wanted;
  for(int xx=0;xx<(procentdone*100)/2;xx++) {
    xxof=xx*7;
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( xpos+200 + xof + xxof,ypos+100 + yof , 0.0);
    glTexCoord2f(0, 1); glVertex3f( xpos+200 + xof + xxof,ypos+130 + yof, 0.0);
    glTexCoord2f(1, 1); glVertex3f( xpos+200+5 + xof + xxof,ypos+130 + yof, 0.0);
    glTexCoord2f(1, 0); glVertex3f( xpos+200+5 + xof + xxof,ypos+100 + yof, 0.0);
    glEnd();
  }
  glPopMatrix();
}




// ****************************************************************************************
//
// Show options from show_torrent
//
// ****************************************************************************************

void torrent_loader::show_move_options() {
  const char *options_text[]={"Movie","Music"};
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
  glTexCoord2f(0, 0); glVertex3f( xpos+225 + xof,ypos+100 + yof , 0.0);
  glTexCoord2f(0, 1); glVertex3f( xpos+225 + xof,ypos+350 + yof, 0.0);
  glTexCoord2f(1, 1); glVertex3f( xpos+225+350 + xof,ypos+350 + yof, 0.0);
  glTexCoord2f(1, 0); glVertex3f( xpos+225+350 + xof,ypos+100 + yof, 0.0);
  glEnd();
  glPopMatrix();
  
  glPushMatrix();  
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  // glTranslatef(350 + xof, 280 + yof, 0.0f);
  xof=350;
  yof=600;
  glTranslatef(0.0f, 0.0f, 0.0f);
  for (int n=0;n<2;n++) {
    // glRasterPos2f(0.0f, 0.0f+(n*28));
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);
    if (n==torrent_info_move_line_nr) glColor3f(0.2f, 0.8f, 0.2f); else glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( 200 + xof,100 + yof , 0.0);
    glTexCoord2f(0, 1); glVertex3f( 200 + xof,130 + yof, 0.0);
    glTexCoord2f(1, 1); glVertex3f( 200+300 + xof,130 + yof, 0.0);
    glTexCoord2f(1, 0); glVertex3f( 200+300 + xof,100 + yof, 0.0);
    glEnd();
    yof=yof-50;
  }
  glColor3f(0.5f, 0.5f, 0.0f);
  glTranslatef(650.0f, 658.0f, 0.0f);
  yof=600;
  for (int n=0;n<2;n++) {
    glDisable(GL_TEXTURE_2D);
    glRasterPos2f(0.0f, 0.0f+(n*50));
    showtxt=fmt::format(" {:.25} ", options_text[n]);
    myglprintbig((char *) showtxt.c_str());    
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
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBindTexture(GL_TEXTURE_2D,_textureloadfile);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  winsizx=188;
  winsizy=81;
  xpos=-140;
  ypos=650;
  glLoadName(41);
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
  glRasterPos2f(750.0f, 0.0f+(870.0f));
  myglprintbig((char *) "Torrent progress status.");
  
  glRasterPos2f(350.0f, 0.0f+(750.0f));
  int tconections=0;
  for (int n=0;n<torrent_list.size();n++) tconections = tconections + torrent_list[n].num_connections;
  std::string tempstr = fmt::format("Total connections {}",tconections);
  myglprint((char *) tempstr.c_str());

  glTranslatef(300, 480, 0.0f);
  glRasterPos2f(0.0f, 0.0f+(230.0f));
  myglprint((char *) "Name.                                                                              Progress.            Status.");
  for (int n=0;n<TORRENT_ANTAL-1;n++) {
    glRasterPos2f(0.0f, 0.0f+(200-(n*18)));
    if (n<torrent_list.size()) {
      if (torrent_list.at(n).active) {
        if (!(torrent_list.at(n).downloaded)) {
          if ((torrent_list.at(n).total_wanted/1024)>1024) {
            if ((torrent_list.at(n).total_wanted/1024/1024)>1024) {
              if ((torrent_list.at(n).total_wanted/1024/1024/1024)>1024) {
                if (torrent_list.at(n).paused) {
                  showtxt = fmt::format(" {:70} {:>6.2} %   {:4} of {:4} Gb {:>20} ", torrent_list.at(n).torrent_name, torrent_list.at(n).progress, torrent_list.at(n).downloaded_size/1024/1024, torrent_list.at(n).total_wanted/1024/1024/1024, "Paused");
                } else {
                  showtxt = fmt::format(" {:70} {:>6.2} %   {:4} of {:4} Gb {:>20} ", torrent_list.at(n).torrent_name, torrent_list.at(n).progress, torrent_list.at(n).downloaded_size/1024/1024, torrent_list.at(n).total_wanted/1024/1024/1024, torrent_list.at(n).state_text);
                }
              } else {
                if (torrent_list.at(n).paused) {
                  showtxt = fmt::format(" {:70} {:>6.2} %   {:4} of {:4} Mb {:>20} ", torrent_list.at(n).torrent_name, torrent_list.at(n).progress, torrent_list.at(n).downloaded_size/1024/1024, torrent_list.at(n).total_wanted/1024/1024, "Paused");
                } else {
                  showtxt = fmt::format(" {:70} {:>6.2} %   {:4} of {:4} Mb {:>20} ", torrent_list.at(n).torrent_name, torrent_list.at(n).progress, torrent_list.at(n).downloaded_size/1024/1024, torrent_list.at(n).total_wanted/1024/1024, torrent_list.at(n).state_text);
                }
              }
            } else {
              //Kb
              if (torrent_list.at(n).paused) {
                showtxt = fmt::format(" {:70} {:>6.2} %   {:4} of {:4} Kb {:>20} ", torrent_list.at(n).torrent_name, torrent_list.at(n).progress, torrent_list.at(n).downloaded_size/1024, torrent_list.at(n).total_wanted/1024, "Paused");
              } else {
                showtxt = fmt::format(" {:70} {:>6.2} %   {:4} of {:4} Kb {:>20} ", torrent_list.at(n).torrent_name, torrent_list.at(n).progress, torrent_list.at(n).downloaded_size/1024, torrent_list.at(n).total_wanted/1024, torrent_list.at(n).state_text);
              }
            }
          } else {
            if (torrent_list.at(n).paused) {
              showtxt = fmt::format(" {:70} {:>6.2} %   {:4} of {:4} B {:>20} ", torrent_list.at(n).torrent_name, torrent_list.at(n).progress, torrent_list.at(n).downloaded_size, torrent_list.at(n).total_wanted, "Paused");
            } else {
              showtxt = fmt::format(" {:70} {:>6.2} %   {:4} of {:4} B {:>20} ", torrent_list.at(n).torrent_name, torrent_list.at(n).progress, torrent_list.at(n).downloaded_size, torrent_list.at(n).total_wanted, torrent_list.at(n).state_text);
            }
          }
        } else {
          if ((torrent_list.at(n).total_wanted/1024)>1024) {
            if ((torrent_list.at(n).total_wanted/1024/1024)>1024) {
              if (get_automove_done(n)) {
                showtxt = fmt::format(" {:70} 100.00 %       {:4} Mb {:>21} ",torrent_list.at(n).torrent_name, torrent_list.at(n).total_wanted/1024/1024, "          Downloaded/Movied.");
              } else {
                showtxt = fmt::format(" {:70} 100.00 %       {:4} Mb {:>21} ",torrent_list.at(n).torrent_name, torrent_list.at(n).total_wanted/1024/1024, "Downloaded.");
              }
            } else {
              if (get_automove_done(n)) {
                showtxt = fmt::format(" {:70} 100.00 %       {:4} Kb {:>21} ",torrent_list.at(n).torrent_name, torrent_list.at(n).total_wanted/1024, "          Downloaded/Movied");
              } else {
                showtxt = fmt::format(" {:70} 100.00 %       {:4} Kb {:>21} ",torrent_list.at(n).torrent_name, torrent_list.at(n).total_wanted/1024, "Downloaded.");
              }
            }
          } else {
            if (get_automove_done(n)) {
              showtxt = fmt::format(" {:70} 100.00 %       {:4} b {:>21} ",torrent_list.at(n).torrent_name, torrent_list.at(n).total_wanted, "          Downloaded/Movied.");
            } else {
              showtxt = fmt::format(" {:70} 100.00 %       {:4} b {:>21} ",torrent_list.at(n).torrent_name, torrent_list.at(n).total_wanted, "Downloaded.");
            }
          }
        }
      } else {      
        showtxt = no_torrent_text[0];
      }
    } else showtxt = no_torrent_text[0];
    myglprint((char *) showtxt.c_str());
  }
  glPopMatrix();  
  coursornow(-70,528-((edit_line_nr)*18),0);
}

