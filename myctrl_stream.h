#ifndef MYCTRL_STREAM
#define MYCTRL_STREAM


#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "myth_vlcplayer.h"


//const char *radioiconpath="/usr/share/mythtv-controller/images/radiostations/";
const int feed_namelength=80;
const int feed_desclength=200;
const int feed_pathlength=200;
const int feed_url=400;

struct stream_oversigt_type {
    char feed_showtxt[feed_namelength+1];			// show name
    char feed_name[feed_namelength+1];				// mythtv db feedtitle
    char feed_desc[feed_desclength+1];				// desc
    char feed_path[feed_pathlength+1];				// mythtv db path
    char feed_gfx_url[feed_url+1];
    char feed_gfx_mythtv[feed_url+1];				// mythtv icon gfx path in mythtv system
    char feed_streamurl[feed_url+1];
    unsigned int feed_group_antal;
    unsigned int feed_path_antal;
    GLuint textureId;
    long intnr;
};

class stream_class : vlc_controller {
    private:
        enum { maxantal=3000 };					                                        // MAX antal radio stationer
        stream_oversigt_type *stack[maxantal];			                            // radio stack
        int antal;					                       	                            // Antal
        int stream_optionselect;				                                        // bruges til valgt af stream type som skal vises
        void set_texture(int nr,GLuint idtexture);
        int opdatere_stream_gfx(int nr,char *gfxpath);		                      //
        bool startup_loaded;					                                          // load stream icons statios list
        bool stream_oversigt_loaded;
        int stream_oversigt_loaded_nr;
        int stream_oversigt_nowloading;				                                  // denne tæller op når der loades gfx
    public:
        bool stream_is_playing;
        int loadweb_stream_iconoversigt();			                                // load web gfx in to cache dir
        int type;
        bool gfx_loaded;					                                              //
        char *get_stream_name(int nr) { return (stack[nr]->feed_name); }
        char *get_stream_desc(int nr) { return (stack[nr]->feed_desc); }
        char *get_stream_mythtvgfx_path(int nr) { if (nr<antal) return (stack[nr]->feed_gfx_mythtv); else return(0); }
        char *get_stream_path(int nr) { if (nr<antal) return (stack[nr]->feed_path); }
        char *get_stream_url(int nr) { if (nr<antal) return (stack[nr]->feed_streamurl); }
        char *get_stream_gfx_url(int nr) { if (nr<antal) return (stack[nr]->feed_gfx_url); else return(0); }
        unsigned int get_stream_groupantal(unsigned int nr) { return (stack[nr]->feed_group_antal); }
        unsigned int get_stream_pathantal(unsigned int nr) { return (stack[nr]->feed_path_antal); }
        long get_stream_intnr(unsigned int nr) { return (stack[nr]->intnr); }
        GLuint get_texture(int nr) { return (stack[nr]->textureId); }
        stream_class();
        ~stream_class();
        int streamantal() { return(antal); }
        void clean_stream_oversigt();
        int opdatere_stream_oversigt(char *art,char *fpath);
        int loadrssfile();                                                      // download file from web
        int parsexmlrssfile(char *filename);                                    // parse file from web
//        int opdatere_stream_oversigt(char *searchtxt);
        void playstream(char *url);
        void show_stream_oversigt(GLuint normal_icon,GLuint icon_mask,GLuint empty_icon,int _mangley);
        void show_stream_oversigt1(GLuint normal_icon,GLuint empty_icon,GLuint empty_icon1,int _mangley);
};

void *loadweb(void *data);

void *load_all_stream_gfx(void *data);

#endif
