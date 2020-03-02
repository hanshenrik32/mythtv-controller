#ifndef MYCTRL_TIDAL
#define MYCTRL_TIDAL

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "json-parser/json.h"

// web server
//#include "mongoose-master/mongoose.h"


const int tidal_playlisttype=0;                         // playlist type
const int tidal_songlisttype=1;                         // song list type

//
// device struct
//

class tridal_device_def {
  public:
    char        id[41];                     // tidal id
    bool        is_active;                  // is it working
    bool        is_private_session;         //
    bool        is_restricted;              // is private
    char        name[200];                  // dev name
    char        devtype[30];                // dev type
    int         devvolume;                  // play volume
    tridal_device_def();
};


//
// playlist/song overview def tidal
//

class tridal_oversigt_type {
  public:
    char        feed_showtxt[80+1];			          // what to show in overview
    char        feed_name[80+1];				          // playlist/song name
    char        feed_artist[80+1];                // artist
    char        feed_desc[80+1];				          // desc
    char        feed_gfx_url[1024+1];             //
    char        feed_release_date[40+1];				  //
    char        playlistid[100+1];                // playlist id
    char        playlisturl[1024+1];               // play list url + tidal command
    unsigned int feed_group_antal;
    unsigned int feed_path_antal;
    bool        nyt;                              //
    GLuint      textureId;                        // gfx icon loaded
    long        intnr;
    char        type;
    tridal_oversigt_type();                      // constructor
};

//
// active song to play
//

class tridal_active_play_info_type {                // sample data down here
  public:
    tridal_active_play_info_type();
    long progress_ms;                                   // 27834
    long duration_ms;                                   // 245119
    char song_name[200];                              // Joe Bonamassa
    char artist_name[200];                              // Joe Bonamassa
    char cover_image_url[256];                          // 300*300 pixel (https://i.scdn.co/image/0b8eca8ecc907dc58fbdacbc6ac6b58aca88b805)
    GLuint cover_image;
    char album_name[200];                               // (British Blues Explosion Live)
    char release_date[24];                              //
    long popularity;                                    // (27)
    bool is_playing;                                    // (true)
};

// tidal global class

class tridal_class {
    private:
        enum { maxantal=5000 };					                                        // MAX antal rss stream in wiew
        tridal_oversigt_type *stack[maxantal];			                            // tidal playlist stack
        tridal_device_def tridal_device[10];
        int tridal_device_antal;                                               // antal device found
        tridal_active_play_info_type tridal_aktiv_song[1];                    //
        int tridal_aktiv_song_antal;					                                  // Antal songs in playlist
        bool tridal_update_loaded_begin;
        //
        char tidaltoken[512];                                                 // access_token
        char tidaltoken_refresh[512];                                         // refresh_token
        int antal;					                       	                            // Antal songs in playlist
        int antalplaylists;                                                     // antal playlist in view
        bool tridal_is_playing;                                                // do we play ?
        bool tridal_is_pause;                                                  // do we pause
        //char overview_show_bane_name[81];                                       // name of the band show in overview then you search on band and play songs from it
        int get_search_result_online(char *searchstring,int type);
        int download_user_playlist(char *tidaltoken,int startofset);          // download playlist(json) file from tidal
        bool gfx_loaded;			                                                  // gfx_loaded = true then gfx is loaded
        bool search_loaded;
    public:
        bool set_search_loaded() { search_loaded=true; }
        int loaded_antal;                                                       // antal loaded i loader
        unsigned int tridal_playlist_antal;
        void settextureidfile(int nr,char *filename);
        void set_tridal_update_flag(bool flag);
        bool get_tridal_update_flag();
        char overview_show_band_name[81];                                       // name of the band show in overview then you search on band and play songs from it
        char overview_show_cd_name[81];                                         // name of the band show in overview then you search on band and play songs from it
        bool search_tridal_online_done;
        bool show_search_result;                                                // are ew showing search result in tidal view ?
        bool search_playlist_song;                                              // search type 1 for song 0 for playlist
        char tidal_client_id[255];                                            // Client id
        char tidal_secret_id[255];                                            // Secret id
        GLuint aktiv_song_tridal_icon;                                         // loaded gfx info for playing
        int active_tridal_device;                                              // active device then get tidal devices or -1
        int active_default_play_device;                                         // active device or -1
        char active_default_play_device_name[256];                              // active device name
        char tridal_playlistname[256];
        char tridal_playlistid[256];
        char client_id[120];                                                    // tidal client id
        char client_secret[120];                                                // tidal client secret
        int stream_optionselect;				                                        // bruges til valgt af stream type som skal vises
        void set_texture(int nr,GLuint idtexture);                              // set texture
        int opdatere_stream_gfx(int nr,char *gfxpath);		                      // NOT in use
        bool startup_loaded;					                                          // load stream icons statios list
        bool tridal_oversigt_loaded;                                           //
        int tridal_oversigt_loaded_nr;                                         //
        int tridal_oversigt_nowloading;				                                // denne tæller op når der loades gfx
        int type;                                                               // 0 = playlist 1 = songs 2 = Artist 3 =
        int searchtype;                                                         // 0 = artist, 1 = album, 2 = playlits, 3 = track
        int get_antal_rss_feeds_sources(MYSQL *conn);                           // get # of rss feeds from db
        // used by webserver
        struct mg_mgr mgr;                                                      // web server
        struct mg_mgr client_mgr;                                               // web server client
        struct mg_connection *c;                                                // connection struct
        // end webserver
        int load_tridal_iconoversigt();			                                  // load web gfx in to cache dir
        // in use
        void tridal_set_token(char *token,char *refresh);                      // set token in struct
        char *tridal_get_token() { return(tidaltoken); };                    // get token from struct
        int tridal_refresh_token();                                            // refresh token on postify api 2
        bool tridal_check_tridaldb_empty();
        int tridal_aktiv_song_msplay() { return( tridal_aktiv_song[0].progress_ms ); };                     //
        int tridal_aktiv_song_mslength() { return( tridal_aktiv_song[0].duration_ms ); };                   //
        char *tridal_aktiv_song_name() { return( tridal_aktiv_song[0].song_name ); };                       //
        char *tridal_aktiv_artist_name() { return( tridal_aktiv_song[0].artist_name ); };                   // aktiv sang som spilles
        char *tridal_aktiv_song_release_date() { return( tridal_aktiv_song[0].release_date ); };            //
        char *get_active_device_id() { return(tridal_device[active_tridal_device].id); };   // get active dev id
        char *get_active_tridal_device_name();                                 //                         //
        char *get_device_id(int nr) { return(tridal_device[nr].id); };         // get active dev id
        char *get_device_name(int nr) { return(tridal_device[nr].name); };     // get active dev id
        int get_tridal_intnr(int nr);                                          //
        char *get_tridal_playlistid(int nr);                                   // get id to play
        char *get_tridal_name(int nr);                                         // get record name
        char *get_tridal_desc(int nr);                                         // get record desc
        char *get_tridal_textureurl(int nr) { if ( nr < antal ) return(stack[nr]->feed_gfx_url); else return(0); }
        char *get_tridal_feed_showtxt(int nr) { if ( nr < antal ) return(stack[nr]->feed_showtxt); else return(0); }
        char *get_tridal_artistname(int nr) { if ( nr < antal ) return(stack[nr]->feed_artist); else return(0); }
        int get_tridal_type(int nr) { if ( nr < antal ) return(stack[nr]->type); else return(0); }
        GLuint get_texture(int nr) { if ( nr < antal ) return(stack[nr]->textureId); else return(0); }
        int antal_tridal_streams() { return antalplaylists; };
        tridal_class();
        ~tridal_class();
        int streamantal() { return(antal-1); }                                  //
        void clean_tridal_oversigt();                                          // clear list
        int tridal_req_playlist();                                             //
        int tridal_get_user_playlists(bool force,int startoffset);                             // get user playlist (list of playlist)
        int tridal_get_playlist(const char *playlist,bool force,bool create_playlistdb);       // get playlist name info + songs info and update db
        int tridal_get_user_id();
        int tridal_play_playlist(char *songarray);
        int tridal_play_now_playlist(char *playlist_song,bool now);            // play playlist
        int tridal_play_now_playlist2(char *playlist_name,bool now);            // play playlist
        int tridal_play_now_song(char *playlist_song,bool now);                // play song
        int tridal_play_now_artist(char *playlist_song,bool now);              // play artist
        int tridal_play_now_album(char *playlist_song,bool now);               // play album
        int tridal_get_access_token2();                                        // new get token
        int tridal_get_available_devices();                                    // get list of devices
        int tridal_do_we_play();                                               // Do we play song now
        int tridal_pause_play();                                               // Pause
        int tridal_pause_play2();                                              // Pause
        int tridal_resume_play();                                              // resume play
        int tridal_last_play();                                                // play last song
        int tridal_last_play2();                                               // new play last song
        int tridal_next_play();                                                // play next song
        int tridal_next_play2();                                               // play next song
        void select_device_to_play();                                           // show device list to play on
        void set_default_device_to_play(int nr);                                // set default device list to play on
        void show_setup_tridal();                                              //
        int opdatere_tridal_oversigt(char *refid);                             // update from db from refid - if refid=0 then from root.
        int opdatere_tridal_oversigt_searchtxt(char *keybuffer,int type);        // search in db
        int opdatere_tridal_oversigt_searchtxt_online(char *keybuffer,int type); // search online
        // show tidal playlist overview
        void show_tridal_oversigt(GLuint normal_icon,GLuint song_icon,GLuint empty_icon,GLuint backicon,int sofset,int stream_key_selected);
        void show_tridal_search_oversigt(GLuint normal_icon,GLuint song_icon,GLuint empty_icon,GLuint backicon,int sofset,int stream_key_selected,char *searchstring);
};


int download_image(char *imgurl,char *filename);

void *load_tidal_web(void *data);
char *b64_encode(const unsigned char *in, size_t len);

#endif
