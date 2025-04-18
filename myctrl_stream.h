#ifndef MYCTRL_STREAM
#define MYCTRL_STREAM

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include "myth_vlcplayer.h"

const int feed_namelength=80;
const int feed_desclength=200;
const int feed_pathlength=200;
const int feed_url=400;


// not in use for now.

template<typename streamoversigt_type>
struct Data {
  Data(const std::size_t size)
    : data(new streamoversigt_type[size]) {
  }
  ~Data() { delete [] data; }
  streamoversigt_type *data;
};



struct stream_oversigt_type {
    char feed_showtxt[feed_namelength+1];			// show name
    char feed_name[feed_namelength+1];				// mythtv db feedtitle
    char feed_desc[feed_desclength+1];				// desc
    char feed_path[feed_pathlength+1];				// mythtv db path
    char feed_gfx_url[feed_url+1];
    char feed_gfx_mythtv[feed_url+1];				  // icon gfx path in mythtv system
    char feed_streamurl[feed_url+1];
    unsigned int feed_group_antal;
    unsigned int feed_path_antal;
    bool nyt;
    GLuint textureId;
    long intnr;
};

class stream_class : vlc_controller {
    private:
        enum { maxantal=3000 };					                                        // MAX antal rss stream in wiew
        stream_oversigt_type *stack[maxantal];			                            // radio stack array
        int antal;					                       	                            // Antal streams
        int antalrss_feeds;                                                     // antal feeds
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
    public:
        bool stream_is_playing;
        bool stream_is_pause;
        int loadweb_stream_iconoversigt();			                                // load web gfx in to cache dir
        int type;
        bool gfx_loaded;					                                              //
        void update_rss_nr_of_view(char *url);                                  // save rss to db file (struct)
        void set_rss_new(int nr,bool ny) { if (nr<antal) stack[nr]->nyt=ny; }                 // set new flag
        char *get_stream_name(int nr);                                          // get name
        char *get_stream_desc(int nr);                                          // get desc
        char *get_stream_mythtvgfx_path(int nr) { if (nr<antal) return (stack[nr]->feed_gfx_mythtv); else return(0); }
        char *get_stream_path(int nr) { if (nr<antal) return (stack[nr]->feed_path); else return(0); }
        char *get_stream_url(int nr) { if (nr<antal) return (stack[nr]->feed_streamurl); else return(0);  }
        char *get_stream_gfx_url(int nr) { if (nr<antal) return (stack[nr]->feed_gfx_url); else return(0); }
        unsigned int get_stream_groupantal(unsigned int nr) { if (nr<antal) return (stack[nr]->feed_group_antal); else return(0); }
        unsigned int get_stream_pathantal(unsigned int nr) { if (nr<antal) return (stack[nr]->feed_path_antal); else return(0); }
        long get_stream_intnr(unsigned int nr) { if (nr<antal) return (stack[nr]->intnr); else return(0); }
        GLuint get_texture(int nr) { if (nr<antal) return(stack[nr]->textureId); else return(0); }
        //int get_antal_rss_feeds_sources(MYSQL *conn);                          // get # of rss feeds from db
        int antalstreams() { return antal; };
        int antal_rss_streams() { return antalrss_feeds; };
        int streams_loaded() { return stream_oversigt_loaded_nr; };
        int streams_rss_loaded() { return stream_rssparse_nowloading; };
        void stopstream();
        void softstopstream();
        int playstream(int nr);
        int pausestream(int pause);
        int playstream_url(char *path);
        unsigned long get_length_in_ms();
        float jump_position(float ofset);
        stream_class();
        ~stream_class();
        int streamantal() { return(antal); }
        void clean_stream_oversigt();
        int opdatere_stream_oversigt(char *art,char *fpath);
        int loadrssfile(bool updaterssfile);                                    // download file from web
//        int opdatere_stream_oversigt(char *searchtxt);
        void playstream(char *url);
        float getstream_pos();
        void show_stream_oversigt(GLuint normal_icon,GLuint empty_icon,GLuint empty_icon1,int _mangley,int stream_key_selected);
};

void *loadweb(void *data);

#endif
