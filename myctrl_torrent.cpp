
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
#include <mysql.h>                      // mysql stuf
#include "utility.h"
#include "myctrl_torrent.h"
#include "myctrl_glprint.h"

extern char configmysqluser[256];                              // /mythtv/mysql access info
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //

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
// Copy file to other path (selected in this function).
//
// ****************************************************************************************

void torrent_loader::select_file_name_and_copy_to_otherdir(char *filepath) {
  char filenamepath[1024];
  strcpy(filenamepath,"");
  std::string filenamepath1;
  std::string filename="";
  std::string tmp;
  std::string source_file="";
  std::string dest_file="";
  std::string realpath="";
  std::ofstream torrentfile;
  FILE *f = popen("/usr/bin/zenity --file-selection --file-filter=*.torrent --modal --title=\"Select destination directory.\" --directory 2> /dev/null", "r");
  fgets(filenamepath, 1024, f);
  if (!(f)) {
    return;
  }
  fclose(f);
  if ((strlen(filepath)>0) && (strlen(filenamepath)>0)) {    
    source_file = filepath;
    source_file.erase(std::remove(source_file.begin(), source_file.end(), '\n'), source_file.cend());
    std::string tmpfilename = fs::path(filepath).filename();
    dest_file = filenamepath;
    dest_file = dest_file + "/";
    dest_file = dest_file + tmpfilename;
    dest_file.erase(std::remove(dest_file.begin(), dest_file.end(), '\n'), dest_file.cend());
    ifstream source(source_file, ios::binary);
    ofstream dest(dest_file, ios::binary);
    if (source) {
      dest << source.rdbuf();
      source.close();
      dest.close();
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
  downloadpath = "/tmp/";                                                // default download path
  torrent_download_path = "/tmp/";                                       // default torrent download path
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
  // Højere forbindelsesgrænser
  pack.set_int(lt::settings_pack::connections_limit, 500);  // total connections
  pack.set_int(lt::settings_pack::connections_slack, 50);
  pack.set_int(lt::settings_pack::max_peerlist_size, 2000);
  // Tillad mange aktive downloads
  pack.set_int(lt::settings_pack::active_downloads, -1); // ubegrænset
  pack.set_int(lt::settings_pack::active_seeds, -1);
  pack.set_int(lt::settings_pack::active_limit, -1);
}



// ****************************************************************************************
//
// check download status and return the nr
//
// ****************************************************************************************

int torrent_loader::get_torrent_download_status() { 
  bool found = false;
  int nr = 0;
  for (nr=0;nr<this->torrent_list.size();nr++) {
    if (torrent_list[nr].active) {
      if (torrent_list[nr].is_finished) {
        return(nr+1); // return nr+1, so we can use 0 as not found
        found = true;
        break;
      }
    }
  }
  if (found) return(nr+1); else return(-1);
}


// ****************************************************************************************
//
// set torrent moved file is done flag to true
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
  // save to db
  save_torrent_file_to_db(torrent_list_antal, true, false, torrent_info_data.torrent_file_name , torrent_info_data.total_wanted);
  // add torrent info to vector
  torrent_list.push_back(torrent_info_data);                                           //  add record to vector
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
    if (torrent_list.size()>0) {
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
                  if (torrent_list.at(tnr).downloaded == false) 
                    opdate_done_flag_in_db(torrent_list.at(tnr).torrent_file_name, status.total_wanted);
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
      // kan crash
      if (torrent_list.size()>0) torrent_list.at(0).progress = 0.0f;
    }
  }
}


// ****************************************************************************************
//
// Set torrent on running/pause
//
// ****************************************************************************************

void torrent_loader::pause_torrent(int nr) {
  if (nr<torrent_list.size()) {
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
// inc line in torrent overview
//
// ****************************************************************************************


void torrent_loader::next_edit_line() { 
  if (edit_line_nr+1<torrent_list.size()) edit_line_nr++; 
}



// ****************************************************************************************
//
// dec in torrent overview
//
// ****************************************************************************************


void torrent_loader::last_edit_line() { 
  if (edit_line_nr>0) edit_line_nr--; 
}



// ****************************************************************************************
//
// Delete torrent from view and torrent_loader.txt
//
// ****************************************************************************************

bool torrent_loader::delete_torrent() {
  int n = 0;
  std::string husknavn = torrent_list[edit_line_nr].torrent_file_name;
  std::string line = "";
  int nr = edit_line_nr;                                                         // the active record in the list
  if (nr<torrent_list.size()) {
    torrent_list_antal--;
    try {
      if (nr<handles.size()) this->s.remove_torrent(handles[nr]);      
      if (nr<torrent_list.size()) {
        set_torrent_active(nr,false);                                               // disable torrent
        torrent_list.erase(torrent_list.begin() + nr);                              // Remove the torrent from the vector
      }
    } catch (const std::exception& e) {
      cout << "error remove torrent " << std::endl;
    }
    // clean up torrent file list.
    try {
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
    } catch (const std::exception& e) {
      cout << "error cleanup torrent_loader.txt" << std::endl;
    }
    if ((edit_line_nr>0) && (edit_line_nr>=torrent_list.size())) edit_line_nr--; // Decrease edit line number after deletion
    if (std::rename("torrent_loader_new.txt", "torrent_loader.txt")) {
      std::cerr << "Kunne ikke rename file: torrent_loader_new.txt to torrent_loader.txt\n";
      return false;
    }
  }
  return(true);
}


// ****************************************************************************************
//
// Delete torrent from view and torrent_loader.txt
//
// nr is the torrent number in the list
//
// ****************************************************************************************

bool torrent_loader::delete_torrent(int nr) {
  int n = 0;
  std::string husknavn = torrent_list[edit_line_nr].torrent_file_name;
  std::string line = "";
  // int nr = edit_line_nr;                                                         // the active record in the list
  if (nr<torrent_list.size()) {
    torrent_list_antal--;
    try {
      if (nr<handles.size()) this->s.remove_torrent(handles[nr]);      
      if (nr<torrent_list.size()) {
        set_torrent_active(nr,false);                                               // disable torrent
        torrent_list.erase(torrent_list.begin() + nr);                              // Remove the torrent from the vector
      }
    } catch (const std::exception& e) {
      cout << "error remove torrent " << std::endl;
    }
    // clean up torrent file list.
    try {
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
    } catch (const std::exception& e) {
      cout << "error cleanup torrent_loader.txt" << std::endl;
    }
    if ((edit_line_nr>0) && (edit_line_nr>=torrent_list.size())) edit_line_nr--; // Decrease edit line number after deletion
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
  if (nr<torrent_list.size()) {
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

int torrent_loader::copy_disk_entry(const std::string& source, const std::string& destination) {
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
      std::cerr << "Dest file exist: " << e.what() << '\n';
      return (-1);
    }
  } else {
    copy_file(source,destination);
  }
  return(1);
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
// save torrent status to database
//
// ****************************************************************************************

bool torrent_loader::save_torrent_file_to_db(int nr, bool active, bool is_finished, const std::string& torrent_file_name, int size) {
  // This function is a placeholder for saving torrent file information to a database.
  // Implementation details would depend on the specific database and schema used.
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  std::string sqlselect;
  std::string sqlcreate;
  sqlcreate = "CREATE TABLE IF NOT EXISTS torrent_file_list (nr int(10) unsigned DEFAULT NULL,active tinyint(1) DEFAULT 0,is_finished  tinyint(1) DEFAULT NULL,added_time timestamp NULL DEFAULT NULL,torrent_file_name varchar(1024) DEFAULT NULL,size int(10) unsigned DEFAULT NULL,id int(10) unsigned NOT NULL AUTO_INCREMENT,PRIMARY KEY (id)) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;";
  sqlselect = fmt::format("insert into torrent_file_list(nr, active ,is_finished, added_time, torrent_file_name, size) values ({}, {}, {}, CURRENT_TIMESTAMP(), '{}', {})",nr, active, is_finished, torrent_file_name, size,0);
  // cout << "SQL: " << sqlselect << std::endl;
  // mysql stuf
  char *database = (char *) "mythtvcontroller";
  conn=mysql_init(NULL);
  // Connect to database
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,sqlcreate.c_str());
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect.c_str());
    res = mysql_store_result(conn);
    mysql_close(conn);
  }
  return(1);
}

// ****************************************************************************************
//
// update size and done flag in database
//
// ****************************************************************************************

bool torrent_loader::opdate_done_flag_in_db(const std::string& torrent_file_name, int size) {
  // This function is a placeholder for saving torrent file information to a database.
  // Implementation details would depend on the specific database and schema used.
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  std::string sqlselect;
  cout << " **********************   opdate_done_flag_in_db: " << torrent_file_name << " size: " << size << std::endl;
  sqlselect = fmt::format("update torrent_file_list set size={},is_finished=1 where torrent_file_name like '{}'" ,size , torrent_file_name);
  // cout << "SQL: " << sqlselect << std::endl;
  // mysql stuf
  char *database = (char *) "mythtvcontroller";
  conn=mysql_init(NULL);
  // Connect to database
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    // mysql_query(conn,"set NAMES 'utf8'");
    // res = mysql_store_result(conn);
    mysql_query(conn,sqlselect.c_str());
    res = mysql_store_result(conn);
    mysql_close(conn);
  }
  return(1);
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
  const char *options_text[]={"Movie","Music","Other"};
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
  for (int n=0;n<3;n++) {
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






//  ****************************************************************************************
//
//
//
//  ****************************************************************************************

std::string format_bits(double bits_per_sec) {
    const char* units[] = { " B", "Kb", "MB", "GB", "TB" };
    int unit_index = 0;
    while (bits_per_sec >= 1024.0 && unit_index < 4) {
        bits_per_sec /= 1024.0;
        ++unit_index;
    }
    return fmt::format("{:6.2f} {:<3}", bits_per_sec, units[unit_index]);
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
  myglprint((char *) "Name.                                                               Progress.                      Status.");
  for (int n=0;n<TORRENT_ANTAL-1;n++) {
    glRasterPos2f(0.0f, 0.0f+(200-(n*18)));
    if (n<torrent_list.size()) {
      std::string torrent_name = torrent_list.at(n).torrent_name;
      if (torrent_name.size()>64) {
        torrent_name.resize(60); // Ensure the name is 64 characters long
        torrent_name += "..."; // Add ellipsis if truncated
      }
      if (torrent_list.at(n).active) {
        if (!(torrent_list.at(n).downloaded)) {
          if (torrent_list.at(n).paused) {
            showtxt = fmt::format(" {:64} {:>8.4} % {} of {} {:>14}", torrent_list.at(n).torrent_name, torrent_list.at(n).progress, torrent_list.at(n).downloaded_size/1024/1024, format_bits(torrent_list.at(n).total_wanted), "Paused");
          } else {
            showtxt = fmt::format(" {:64} {:>8.4} % {} of {} {:>14}", torrent_name, torrent_list.at(n).progress, format_bits(torrent_list.at(n).downloaded_size), format_bits(torrent_list.at(n).total_wanted), torrent_list.at(n).state_text);
          }
        } else {
          if (get_automove_done(n)) {
            showtxt = fmt::format(" {:64} {:>8.4} % {} of {} {} Downloaded/Moved.", torrent_list.at(n).torrent_name, torrent_list.at(n).progress, format_bits(torrent_list.at(n).downloaded_size), format_bits(torrent_list.at(n).total_wanted), "");
          } else {
            showtxt = fmt::format(" {:64} {:>8.4} % {} of {} {} Downloaded.", torrent_list.at(n).torrent_name, torrent_list.at(n).progress, format_bits(torrent_list.at(n).total_wanted), format_bits(torrent_list.at(n).total_wanted), "");
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

