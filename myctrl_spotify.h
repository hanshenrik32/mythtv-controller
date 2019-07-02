#ifndef MYCTRL_SPOTIFY
#define MYCTRL_SPOTIFY

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "myth_vlcplayer.h"
#include "json-parser/json.h"

// web server
#include "mongoose-master/mongoose.h"


//
// device struct
//

class spotify_device_def {
  public:
    char        id[41];                     // spotify id
    bool        is_active;                  // is it working
    bool        is_private_session;         //
    bool        is_restricted;              // is private
    char        name[256];                  // dev name
    char        devtype[30];                // dev type
    int         devvolume;                  // play volume
    spotify_device_def();
};


//
// playlist/song overview def spotify
//

class spotify_oversigt_type {
  public:
    char        feed_showtxt[80+1];			          // what to show in overview
    char        feed_name[80+1];				          // playlist/song name
    char        feed_artist[80+1];                // artist
    char        feed_desc[80+1];				          // desc
    char        feed_gfx_url[4000+1];             //
    char        feed_gfx_mythtv[2000+1];				  //
    char        playlistid[100+1];                // playlist id
    char        playlisturl[1024+1];               // play list url + spotify command
    unsigned int feed_group_antal;
    unsigned int feed_path_antal;
    bool        nyt;                              //
    GLuint      textureId;                        // gfx icon loaded
    long        intnr;
    char        type;
    spotify_oversigt_type();                      // constructor
};

//
// active song to play
//

class spotify_active_play_info_type {                // sample data down here
  public:
    spotify_active_play_info_type();
    long progress_ms;                                   // 27834
    long duration_ms;                                   // 245119
    char song_name[256];                              // Joe Bonamassa
    char artist_name[256];                              // Joe Bonamassa
    char cover_image_url[256];                          // 300*300 pixel (https://i.scdn.co/image/0b8eca8ecc907dc58fbdacbc6ac6b58aca88b805)
    GLuint cover_image;
    char album_name[256];                               // (British Blues Explosion Live)
    char release_date[24];                              //
    long popularity;                                    // (27)
    bool is_playing;                                    // (true)
};

// spotify global class

class spotify_class : vlc_controller {
    private:
        enum { maxantal=3000 };					                                        // MAX antal rss stream in wiew
        spotify_oversigt_type *stack[maxantal];			                            // spotify playlist stack
        spotify_device_def spotify_device[10];
        int spotify_device_antal;                                               // antal device found
        spotify_active_play_info_type spotify_aktiv_song[1];                    //
        int spotify_aktiv_song_antal;					                                  // Antal songs in playlist
        void print_depth_shift(int);
        void process_value_playlist(json_value*, int,int x);
        void process_object_playlist(json_value*, int);
        void process_array_playlist(json_value*, int);
        // spotify search online
        void search_process_value(json_value*, int,int x,int art);
        void search_process_object(json_value*, int,int art);
        void search_process_array(json_value*, int,int art);
        // end spotify search online
        void playlist_print_depth_shift(int depth);
        void playlist_process_object(json_value* value, int depth,MYSQL *conn);
        void playlist_process_array(json_value* value, int depth,MYSQL *conn);
        void playlist_process_value(json_value* value, int depth,int x,MYSQL *conn);
        //

        void process_object_playinfo(json_value* value, int depth);
        void process_array_playinfo(json_value* value, int depth);
        void process_value_playinfo(json_value* value, int depth,int x);


        char spotifytoken[512];                                                 // access_token
        char spotifytoken_refresh[512];                                         // refresh_token
        int antal;					                       	                            // Antal songs in playlist
        int antalplaylists;                                                     // antal
        bool spotify_is_playing;                                                // do we play ?
        bool spotify_is_pause;                                                  // do we pause
        //char overview_show_bane_name[81];                                       // name of the band show in overview then you search on band and play songs from it
    public:
        char overview_show_band_name[81];                                       // name of the band show in overview then you search on band and play songs from it
        char overview_show_cd_name[81];                                       // name of the band show in overview then you search on band and play songs from it
        bool search_spotify_online_done;
        bool show_search_result;                                                // are ew showing search result in spotify view ?
        bool search_playlist_song;                                              // search type 1 for song 0 for playlist
        char spotify_client_id[255];                                            // Client id
        char spotify_secret_id[255];                                            // Secret id
        GLuint aktiv_song_spotify_icon;                                         // loaded gfx info for playing
        void spotify_set_token(char *token,char *refresh);
        char *spotify_get_token() { return(spotifytoken); };
        int spotify_refresh_token();                                            // refresh token on postify api
        int spotify_aktiv_song_msplay() { return( spotify_aktiv_song[0].progress_ms ); };                     //
        int spotify_aktiv_song_mslength() { return( spotify_aktiv_song[0].duration_ms ); };                   //
        char *spotify_aktiv_song_name() { return( spotify_aktiv_song[0].song_name ); };                       //
        char *spotify_aktiv_artist_name() { return( spotify_aktiv_song[0].artist_name ); };                   // aktiv sang som spilles
        char *spotify_aktiv_song_release_date() { return( spotify_aktiv_song[0].release_date ); };            //
        char *get_active_device_id() { return(spotify_device[active_spotify_device].id); };   // get active dev id
        char *get_active_spotify_device_name();                                 //
        char *get_device_id(int nr) { return(spotify_device[nr].id); };         // get active dev id
        char *get_device_name(int nr) { return(spotify_device[nr].name); };     // get active dev id
        int active_spotify_device;                                              // active device then get spotify devices or -1
        int active_default_play_device;                                         // active device or -1
        char active_default_play_device_name[256];                              // active device name
        char spotify_playlistname[256];
        char client_id[120];                                                    // spotify client id
        char client_secret[120];                                                // spotify client secret
        int stream_optionselect;				                                        // bruges til valgt af stream type som skal vises
        void set_texture(int nr,GLuint idtexture);                              // set texture
        int opdatere_stream_gfx(int nr,char *gfxpath);		                      // NOT in use
        bool startup_loaded;					                                          // load stream icons statios list
        bool spotify_oversigt_loaded;                                           //
        int spotify_oversigt_loaded_nr;                                         //
        int spotify_oversigt_nowloading;				                                // denne tæller op når der loades gfx
        int get_antal_rss_feeds_sources(MYSQL *conn);                           // get # of rss feeds from db
        // used my webserver
        struct mg_mgr mgr;                                                      // web server
        struct mg_mgr client_mgr;                                               // web server client
        struct mg_connection *c;                                                // connection struct
        int load_spotify_iconoversigt();			                                  // load web gfx in to cache dir
        int type;                                                               // 0 = playlist 1 = songs 2 = Artist 3 =
        bool gfx_loaded;			        // get artis
        int get_spotify_intnr(int nr);                                          //
        char *get_spotify_playlistid(int nr);                                   // get id to play
        char *get_spotify_name(int nr);                                         // get record name
        char *get_spotify_desc(int nr);                                         // get record desc
        char *get_spotify_textureurl(int nr) { if ( nr < antal ) return(stack[nr]->feed_gfx_url); else return(0); }
        char *get_spotify_feed_showtxt(int nr) { if ( nr < antal ) return(stack[nr]->feed_showtxt); else return(0); }
        char *get_spotify_artistname(int nr) { if ( nr < antal ) return(stack[nr]->feed_artist); else return(0); }
        //char *get_spotify_albumname(int nr) { if ( nr < antal ) return(stack[nr]->album_name); else return(0); }
        int get_spotify_type(int nr) { if ( nr < antal ) return(stack[nr]->type); else return(0); }
        GLuint get_texture(int nr) { if ( nr < antal ) return(stack[nr]->textureId); else return(0); }
        int antal_spotify_streams() { return antalplaylists; };
        spotify_class();
        ~spotify_class();
        int streamantal() { return(antal-1); }
        void clean_spotify_oversigt();
        int spotify_req_playlist();
        // spotify_get_list_of_users_playlists NOT in use
        int spotify_get_list_of_users_playlists(char *client_id);               // Get a List of a User's Playlists
        // in use
        int spotify_get_user_playlists(bool force);                             // get user playlist (list of playlist)
        int spotify_get_playlist(char *playlist,bool force);                    // get playlist + songs and update db
        int spotify_get_user_id();
        int spotify_play_playlist(char *songarray);
        int spotify_play_now_playlist(char *playlist_song,bool now);            // play playlist
        int spotify_play_now_song(char *playlist_song,bool now);                // play song
        int spotify_play_now_artist(char *playlist_song,bool now);              // play artist
        int spotify_play_now_album(char *playlist_song,bool now);               // play album
        int spotify_get_access_token2();                                        // new get token
        int spotify_get_available_devices();                                    // get list of devices
        int spotify_do_we_play();                                               // Do we play song now
        int spotify_do_we_play2();                                               // Do we play song now
        int spotify_pause_play();                                               // Pause
        int spotify_pause_play2();                                                // Pause
        int spotify_resume_play();                                              // resume play
        int spotify_last_play();                                                // play last song
        int spotify_next_play();                                                // play next song
        int spotify_next_play2();                                                // play next song
        void select_device_to_play();                                           // show device list to play on
        void set_default_device_to_play(int nr);                                // set default device list to play on
        void show_setup_spotify();                                              //
        int opdatere_spotify_oversigt(char *refid);
        int opdatere_spotify_oversigt_searchtxt(char *keybuffer,int type);
        int opdatere_spotify_oversigt_searchtxt_online(char *keybuffer,int type);
        // show spotify playlist overview
        void show_spotify_oversigt(GLuint normal_icon,GLuint song_icon,GLuint empty_icon,GLuint backicon,int sofset,int stream_key_selected);
        void show_spotify_search_oversigt(GLuint normal_icon,GLuint song_icon,GLuint empty_icon,GLuint backicon,int sofset,int stream_key_selected,char *searchstring);
};



void *load_spotify_web(void *data);
char *b64_encode(const unsigned char *in, size_t len);

#endif
