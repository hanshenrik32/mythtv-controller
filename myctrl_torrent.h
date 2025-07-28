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
    std::string torrent_fname;
    std::string state_text;
    float progress;
    std::time_t added_time;    
    std::string torrent_name;
    std::string save_path;
};

class torrent_loader {
        lt::torrent_handle torrent_status;
        lt::torrent_status status;
        lt::session s;
        lt::add_torrent_params torrentp;
        std::vector<lt::torrent_handle> handles;
        torrent_loader_struct torrent_list[TORRENT_ANTAL];
        int torrent_list_antal;
        int add_torrent(char *filename);
        int torrent_trackers();
        int torrent_nodes();
        float get_progress(int nr);
    public:
        int antal() { return(torrent_list_antal); }
        torrent_loader();
        void opdate_progress();
        void show_torrent_oversigt(int sofset,int key_selected);
        int load_torrent();                                                                     // load files from torrent_loader.txt
};

#endif
