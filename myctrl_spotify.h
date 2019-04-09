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
    char feed_showtxt[80+1];			// show name
    char feed_name[80+1];				// mythtv db feedtitle
    char feed_desc[80+1];				// desc
    char feed_gfx_url[4000+1];
    char feed_gfx_mythtv[2000+1];				  // icon gfx path in mythtv system
    char playlistid[100+1];
    unsigned int feed_group_antal;
    unsigned int feed_path_antal;
    bool nyt;
    GLuint textureId;
    long intnr;
};

class spotify_class : vlc_controller {
    public:
        enum { maxantal=3000 };					                                        // MAX antal rss stream in wiew
        spotify_oversigt_type *stack[maxantal];			                            // radio stack
        int antal;					                       	                            // Antal streams
        int antalrss_feeds;                                                     // antal feeds
        char client_id[120];                                                    // spotify client id
        char client_secret[120];                                                // spotify client secret
        int stream_optionselect;				                                        // bruges til valgt af stream type som skal vises
        void set_texture(int nr,GLuint idtexture);
        int opdatere_stream_gfx(int nr,char *gfxpath);		                      //
        bool startup_loaded;					                                          // load stream icons statios list
        bool stream_oversigt_loaded;
        int stream_oversigt_loaded_nr;
        int stream_oversigt_nowloading;				                                  // denne tæller op når der loades gfx
        int stream_rssparse_nowloading;				                                  // denne tæller op når der loades rss
        int parsexmlrssfile(char *filename,char *baseiconfile);                // parse file from web and return bane icons from xml file
        int get_antal_rss_feeds_sources(MYSQL *conn);                          // get # of rss feeds from db

        char spotify_authorize_token[255];
        char spotify_client_id[255];
        char spotify_secret_id[255];
        int spotify_get_access_token();
        int spotify_get_users_playlist();
        int spotify_get_playlist(char *playlist);
        struct mg_mgr mgr;
        struct mg_mgr client_mgr;
        struct mg_connection *c;
        bool stream_is_playing;
        bool stream_is_pause;
        int loadweb_stream_iconoversigt();			                                // load web gfx in to cache dir
        int type;
        bool gfx_loaded;					                                              //
        void update_rss_nr_of_view(char *url);                                  // save rss to db file (struct)
        //void set_rss_new(int nr,bool ny) { if (nr<antal) stack[nr]->nyt=ny; }                 // set new flag
        char *get_stream_name(int nr);                                          // get name
        char *get_stream_desc(int nr);                                          // get desc
        //char *get_stream_mythtvgfx_path(int nr) { if (nr<antal) return (stack[nr]->feed_gfx_mythtv); else return(0); }
        //char *get_stream_path(int nr) { if (nr<antal) return (stack[nr]->feed_path); }
        //char *get_stream_url(int nr) { if (nr<antal) return (stack[nr]->feed_streamurl); }
        //char *get_stream_gfx_url(int nr) { if (nr<antal) return (stack[nr]->feed_gfx_url); else return(0); }
        //unsigned int get_stream_groupantal(unsigned int nr) { if (nr<antal) return (stack[nr]->feed_group_antal); }
        //unsigned int get_stream_pathantal(unsigned int nr) { if (nr<antal) return (stack[nr]->feed_path_antal); }
        //long get_stream_intnr(unsigned int nr) { if (nr<antal) return (stack[nr]->intnr); }
        GLuint get_texture(int nr) { if (nr<antal) return(stack[nr]->textureId); else return(0); }
        //int get_antal_rss_feeds_sources(MYSQL *conn);                          // get # of rss feeds from db
        //int antalstreams() { return antal; };
        int antal_rss_streams() { return antalrss_feeds; };
        //int streams_loaded() { return stream_oversigt_loaded_nr; };
        //int streams_rss_loaded() { return stream_rssparse_nowloading; };
        void stopstream();
        void softstopstream();
        int playstream(int nr);
        int pausestream(int pause);
        int playstream_url(char *path);
        unsigned long get_length_in_ms();
        float jump_position(float ofset);
        spotify_class();
        ~spotify_class();
        int streamantal() { return(antal); }
        void clean_spotify_oversigt();
        int opdatere_stotify_oversigt(char *art,char *fpath);
        int loadrssfile(bool updaterssfile);                                    // download file from web
//        int opdatere_stream_oversigt(char *searchtxt);
        void playstream(char *url);
        float getstream_pos();
        int spotify_req_playlist();

        void print_depth_shift(int);
        void process_value(json_value*, int);
        void process_object(json_value*, int);
        void process_array(json_value*, int);


        void show_spotify_oversigt(GLuint normal_icon,GLuint empty_icon,GLuint empty_icon1,int _mangley,int stream_key_selected);
};

void *load_spotify_web(void *data);

char *b64_encode(const unsigned char *in, size_t len);

#endif
