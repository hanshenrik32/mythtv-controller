#ifndef MYCTRL_TIDAL
#define MYCTRL_TIDAL

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "json-parser/json.h"

// web server

#include "mongoose-master/mongoose.h"


const int tidal_playlisttype=0;                         // playlist type
const int tidal_songlisttype=1;                         // song list type

//
// device struct
//

class tidal_device_def {
  public:
    char        id[41];                     // tidal id
    bool        is_active;                  // is it working
    bool        is_private_session;         //
    bool        is_restricted;              // is private
    char        name[200];                  // dev name
    char        devtype[30];                // dev type
    int         devvolume;                  // play volume
    tidal_device_def();
};


//
// playlist/song overview def tidal
//

class tidal_oversigt_type {
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
    tidal_oversigt_type();                        // constructor
};

//
// active song to play
//

class tidal_active_play_info_type {                // sample data down here
  public:
    tidal_active_play_info_type();
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

class tidal_class {
    private:
        enum { maxantal=5000 };					                                        // MAX antal rss stream in wiew
        tidal_oversigt_type *stack[maxantal];			                            // tidal playlist stack
        tidal_device_def tidal_device[10];
        int tidal_device_antal;                                               // antal device found
        tidal_active_play_info_type tidal_aktiv_song[1];                    //
        int tidal_aktiv_song_antal;					                                  // Antal songs in playlist
        bool tidal_update_loaded_begin;
        //
        char tidaltoken[512];                                                 // access_token
        char tidaltoken2[512];                                                 // access_token2
        char countryCode[512];                                                //
        char tidaltoken_refresh[512];                                         // refresh_token
        int antal;					                       	                            // Antal songs in playlist
        int antalplaylists;                                                     // antal playlist in view
        bool tidal_is_playing;                                                // do we play ?
        bool tidal_is_pause;                                                  // do we pause
        //char overview_show_bane_name[81];                                       // name of the band show in overview then you search on band and play songs from it
        bool gfx_loaded;			                                                  // gfx_loaded = true then gfx is loaded
        bool search_loaded;
    public:
        bool reset_amin_in_viewer();
        bool anim_viewer;
        bool anim_viewer_search;
        float anim_angle;

        bool set_search_loaded() { search_loaded=true; return(1); }
        bool get_tidal_update_flag();
        void set_tidal_update_flag(bool flag);
        int loaded_antal;                                                       // antal loaded i loader
        unsigned int tidal_playlist_antal;
        char overview_show_band_name[81];                                       // name of the band show in overview then you search on band and play songs from it
        char overview_show_cd_name[81];                                         // name of the band show in overview then you search on band and play songs from it
        bool search_tidal_online_done;
        bool show_search_result;                                                // are ew showing search result in tidal view ?
        bool search_playlist_song;                                              // search type 1 for song 0 for playlist
        char tidal_client_id[255];                                              // Client id
        char tidal_secret_id[255];                                              // Secret id
        GLuint aktiv_song_tidal_icon;                                           // loaded gfx info for playing
        int active_tidal_device;                                                // active device then get tidal devices or -1
        int active_default_play_device;                                         // active device or -1
        char active_default_play_device_name[256];                              // active device name
        char tidal_playlistname[256];
        char tidal_playlistid[256];
        char client_id[120];                                                    // tidal client id
        char client_secret[120];                                                // tidal client secret
        int stream_optionselect;				                                        // bruges til valgt af stream type som skal vises
        void set_texture(int nr,GLuint idtexture);                              // set texture
        int opdatere_stream_gfx(int nr,char *gfxpath);		                      // NOT in use
        bool startup_loaded;					                                          // load stream icons statios list
        bool tidal_oversigt_loaded;                                             //
        int tidal_oversigt_loaded_nr;                                           //
        int tidal_oversigt_nowloading;				                                  // denne tæller op når der loades gfx
        int type;                                                               // 0 = playlist 1 = songs 2 = Artist 3 =
        int searchtype;                                                         // 0 = artist, 1 = album, 2 = playlits, 3 = track
        int get_antal_rss_feeds_sources(MYSQL *conn);                           // get # of rss feeds from db
        // used by webserver
        struct mg_mgr mgr;                                                      // web server
        struct mg_mgr client_mgr;                                               // web server client
        struct mg_connection *c;                                                // connection struct
        // end webserver
        tidal_class();
        ~tidal_class();
        int streamantal() { return(antal-1); }                                  //
        void tidal_set_token(char *token,char *refresh);
        char *tidal_get_token() { return(tidaltoken); };                    // get token from struct
        int tidal_get_user_id();
        int tidal_get_available_devices();
        void clean_tidal_oversigt();
        void select_device_to_play();
        int gettoken();                               // TEST
        int opdatere_tidal_oversigt(char *refid);                             // update from db from refid - if refid=0 then from root.
        int tidal_get_user_playlists(bool force,int startoffset);
        char *get_device_id(int nr) { return(tidal_device[nr].id); };         // get active dev id
        char *get_device_name(int nr) { return(tidal_device[nr].name); };     // get active dev id

        void process_value_token(json_value* value, int depth,int x);
        void process_object_token(json_value* value, int depth);
        void process_array_token(json_value* value, int depth);


        // check if done
        int tidal_do_we_play();                                               // Do we play song now
        int tidal_pause_play();                                               // Pause
        int tidal_resume_play();                                              // resume play
        int tidal_last_play();                                                // play last song
        int tidal_next_play();                                                // play next song
        int get_tidal_playlistid();
        int tidal_play_now_artist(char *playlist_song,bool now);              // play artist
        int tidal_play_now_album(char *playlist_song,bool now);               // play album

        char *get_tidal_name(int nr);                                         // get record name
        char *get_tidal_playlistid(int nr);                                   // get id to play
        int tidal_play_now_playlist(char *playlist_song,bool now);            //
        char *get_active_tidal_device_name();                                 //
        int tidal_refresh_token();
        int tidal_get_playlist(const char *playlist,bool force,bool create_playlistdb);       // get playlist name info + songs info and update db
        void show_tidal_oversigt(GLuint normal_icon,GLuint song_icon,GLuint empty_icon,GLuint backicon,int sofset,int stream_key_selected);

        int auth_device_authorization();

        char *tidal_aktiv_song_name() { return( tidal_aktiv_song[0].song_name ); };                       //
        char *tidal_aktiv_artist_name() { return( tidal_aktiv_song[0].artist_name ); };                   // aktiv sang som spilles
        char *tidal_aktiv_song_release_date() { return( tidal_aktiv_song[0].release_date ); };            //


        int get_tidal_type(int nr) { if ( nr < antal ) return(stack[nr]->type); else return(0); }
        GLuint get_texture(int nr) { if ( nr < antal ) return(stack[nr]->textureId); else return(0); }
        int antal_tidal_streams() { return antalplaylists; };
        char *get_tidal_textureurl(int nr) { if ( nr < antal ) return(stack[nr]->feed_gfx_url); else return(0); }
        char *get_tidal_feed_showtxt(int nr) { if ( nr < antal ) return(stack[nr]->feed_showtxt); else return(0); }
        char *get_tidal_artistname(int nr) { if ( nr < antal ) return(stack[nr]->feed_artist); else return(0); }


        int tidal_aktiv_song_msplay() { return( tidal_aktiv_song[0].progress_ms ); };                     //
        int tidal_aktiv_song_mslength() { return( tidal_aktiv_song[0].duration_ms ); };                   //
        char *get_active_device_id() { return(tidal_device[active_tidal_device].id); };   // get active dev id

        // works
        int load_tidal_iconoversigt();
        void print_depth_shift(int depth);
        void playlist_print_depth_shift(int depth);
        int get_access_token(char *loginbase64);                                                        // get token
        int get_users_album(char *albumid);
        void process_value_playlist(json_value* value, int depth,int x);
        void process_object_playlist(json_value* value, int depth);
        void process_array_playlist(json_value* value, int depth);

        int tidal_play_now_song(char *playlist_song,bool now);                // play song
        int get_playlist_from_file(char *filename);                           // read/import playlists from file

};

int tidal_download_image(char *imgurl,char *filename);

#endif