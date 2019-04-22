#ifndef MYCTRL_SPOTIFY
#define MYCTRL_SPOTIFY

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "myth_vlcplayer.h"
#include "json-parser/json.h"

// web server
#include "mongoose-master/mongoose.h"

struct spotify_oversigt_type {
    char feed_showtxt[80+1];			        // what to show in overview
    char feed_name[80+1];				          // playlist/song name
    char feed_desc[80+1];				          // desc
    char feed_gfx_url[4000+1];            //
    char feed_gfx_mythtv[2000+1];				  //
    char playlistid[100+1];               // playlist id
    char playlisturl[200+1];              // play list url + spotify command
    unsigned int feed_group_antal;
    unsigned int feed_path_antal;
    bool nyt;
    GLuint textureId;                     // gfx icon loaded
    long intnr;
};

class spotify_class : vlc_controller {
    public:
        enum { maxantal=3000 };					                                        // MAX antal rss stream in wiew
        spotify_oversigt_type *stack[maxantal];			                            // spotify playlist stack
        char spotify_playlistname[256];
        int antal;					                       	                            // Antal songs in playlist
        int antalplaylists;                                                     // antal
        char client_id[120];                                                    // spotify client id
        char client_secret[120];                                                // spotify client secret
        char spotify_authorize_token[255];                                      // token get from spotify
        char spotify_client_id[255];                                            // Client id
        char spotify_secret_id[255];                                            // Secret id
        int stream_optionselect;				                                        // bruges til valgt af stream type som skal vises
        void set_texture(int nr,GLuint idtexture);                              // set texture
        int opdatere_stream_gfx(int nr,char *gfxpath);		                      // NOT in use
        bool startup_loaded;					                                          // load stream icons statios list
        bool spotify_oversigt_loaded;                                           //
        int spotify_oversigt_loaded_nr;                                         //
        int spotify_oversigt_nowloading;				                                // denne tæller op når der loades gfx
        int spotify_rssparse_nowloading;				                                // denne tæller op når der loades rss
        //int parsexmlrssfile(char *filename,char *baseiconfile);               // parse file from web and return bane icons from xml file
        int get_antal_rss_feeds_sources(MYSQL *conn);                           // get # of rss feeds from db
        int spotify_get_access_token();
        int spotify_get_user_playlists();
        int spotify_get_playlist(char *playlist);
        int spotify_get_available_devices();                                    // get list of devices
        // used my webserver
        struct mg_mgr mgr;
        struct mg_mgr client_mgr;
        struct mg_connection *c;
        bool stream_is_playing;
        bool stream_is_pause;
        int loadweb_stream_iconoversigt();			                                // load web gfx in to cache dir
        int type;
        bool gfx_loaded;					                                              //
        char *get_spotify_name(int nr);                                          // get name
        char *get_spotify_desc(int nr);                                          // get desc
        GLuint get_texture(int nr) { if (nr<antal) return(stack[nr]->textureId); else return(0); }
        int antal_spotify_streams() { return antalplaylists; };
        spotify_class();
        ~spotify_class();
        int streamantal() { return(antal-1); }
        void clean_spotify_oversigt();
        int spotify_req_playlist();
        int spotify_get_list_of_users_playlists();                              // Get a List of a User's Playlists
        int spotify_get_user_id();
        int spotify_play_songs(char *songarray);
        int spotify_play_playlist(char *songarray);
        int spotify_play_now(bool now);
        void print_depth_shift(int);
        void process_value(json_value*, int,int x);
        void process_object(json_value*, int);
        void process_array(json_value*, int);

        void playlist_print_depth_shift(int depth);
        void playlist_process_object(json_value* value, int depth,MYSQL *conn);
        void playlist_process_array(json_value* value, int depth,MYSQL *conn);
        void playlist_process_value(json_value* value, int depth,int x,MYSQL *conn);

        int opdatere_spotify_oversigt(int refid);
        void show_spotify_oversigt(GLuint normal_icon,GLuint empty_icon,GLuint backicon,int _mangley,int stream_key_selected);
};

void *load_spotify_web(void *data);

char *b64_encode(const unsigned char *in, size_t len);

#endif
