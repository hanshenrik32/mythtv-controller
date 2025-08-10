#ifndef MYCTRL_TORRENT
#define MYCTRL_TORRENT

#include <vector>
#include <string>
#include <libtorrent/session.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_info.hpp>


const int TORRENT_ANTAL = 20;

struct torrent_loader_struct {
    int nr;
    bool active;
    bool downloaded;
    bool paused;
    bool is_finished;
    long num_connections;
    std::string state_text;
    float progress;
    bool automove_done_to_moviepath;
    std::time_t added_time;
    std::string torrent_name;
    std::string torrent_file_name;
    std::string save_path;
    std::int64_t total_wanted;
    std::int64_t downloaded_size;
};

class torrent_loader {
        lt::torrent_handle torrent_status;
        lt::torrent_status status;
        // lt::session s;
        lt::add_torrent_params torrentp;
        std::vector<lt::torrent_handle> handles;
        std::vector<torrent_loader_struct> torrent_list;
        int torrent_list_antal;
        int add_torrent(char *filename);
        int torrent_trackers();
        int torrent_nodes();
        float get_progress(int nr);
        int edit_line_nr;
        int torrent_info_line_nr;
        int torrent_info_move_line_nr;
    public:
        lt::settings_pack pack;        
        lt::session s;
        std::string downloadpath;
        bool trash_torrent;
        bool automove_to_movie_path;
        void select_file_name();
        bool get_torrent_download_status(int nr) { return(torrent_list[nr].is_finished); }
        char *get_name(int nr) { return((char *) torrent_list[nr].torrent_name.c_str()); }
        int get_torrent_info_line_nr() { return(torrent_info_line_nr); }
        void next_edit_line() { if (edit_line_nr+1<torrent_list_antal) edit_line_nr++; }
        void last_edit_line() { if (edit_line_nr>0) edit_line_nr--; }
        int get_edit_line() { return(edit_line_nr); }
        void next_edit_line_info() { if (torrent_info_line_nr+1<3) torrent_info_line_nr++; }
        void last_edit_line_info() { if (torrent_info_line_nr>0) torrent_info_line_nr--; }

        void next_edit_line_move_info() { if (torrent_info_move_line_nr+1<2) torrent_info_move_line_nr++; }
        void last_edit_line_move_info() { if (torrent_info_move_line_nr>0) torrent_info_move_line_nr--; }
        int get_edit_line_move_info() { return(torrent_info_move_line_nr); }

        int antal() { return(torrent_list_antal-1); }
        void set_torrent_active(int n,bool activ) { torrent_list[n].active=activ; }
        void set_torrent_paused(int n,bool pause) { torrent_list[n].paused=pause; }
        torrent_loader();
        void opdate_progress();
        void show_torrent_oversigt(int sofset,int key_selected);
        void show_torrent_options();                                                            // show pause/move/delete optios in opengl
        void show_move_options();                                                               // show options in opengl
        void pause_torrent(int nr);                                                             // Pause torrent file
        bool delete_torrent(int nr);                                                            // Delete torrent file
        void move_torrent(int nr);                                                              // Set Show move in opengl flag
        void show_file_move();                                                                  // show the info in opengl
        int load_torrent();                                                                     // load files from torrent_loader.txt
        void set_automove_done(int nr);
        bool get_automove_done(int nr);
        bool copy_file(const std::string& source, const std::string& destination);
        bool copy_disk_entry(const std::string& source, const std::string& destination);
        void opdate_torrent();
};

#endif
