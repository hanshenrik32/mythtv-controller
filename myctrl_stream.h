#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <string>
#include <vlc/vlc.h>
#include "myth_vlcplayer.h"


const int feed_namelength=80;
const int feed_desclength=200;
const int feed_pathlength=200;
const int feed_url=400;


struct StreamItem {
  std::string title;
  GLuint cover;
  bool selected = false;
};

struct Color {
  float r, g, b, a;
};

struct stream_oversigt_type {
  // char feed_showtxt[feed_namelength+1];			// show name
  std::string feed_showtxt;
  std::string feed_name;				// mythtv db feedtitle
  std::string feed_desc;				// desc
  std::string feed_path;				// mythtv db path
  std::string feed_gfx_url;
  std::string feed_gfx_mythtv;				  // icon gfx path in mythtv system
  std::string feed_streamurl;
  unsigned int feed_group_antal;
  unsigned int feed_path_antal;
  bool nyt;
  GLuint textureId;
  long intnr;
};

class stream_class : public vlc_controller {
  private:
    int stream_optionselect;				                                        // bruges til valgt af stream type som skal vises
    int stream_rssparse_nowloading;				                                  // denne tæller op når der loades rss
  public:
    bool toplevel;
    enum { maxantal=3000 };					                                        // MAX antal rss stream in wiew
    std::vector<stream_oversigt_type> FeedCatalog;
    std::vector<stream_oversigt_type> FeedCatalog_search_view;
    int antal;					                       	                            // Antal streams
    int antalrss_feeds;                                                     // antal feeds
    int antal_in_search_view;

    bool stream_oversigt_loaded;
    int stream_oversigt_loaded_nr;
    int stream_oversigt_nowloading;				                                  // denne tæller op når der loades gfx
    std::string rss_search_podcast_string;

    bool stream_is_playing;
    bool stream_is_pause;

    int selected_icon_in_view=1;
    // touch screen scroll
    bool dragging=false;
    float scrollSpeed=1.0f;
    int lastX=0;
    int lastY=0;
    int lastDX=0;
    int lastDY=0;
    float viewOffsetX=0.0f;
    float viewOffsetY=0.0f;
    // end touch screen scroll
    int downTimeMs=0;
    bool moved=false;
    bool gettouchbutton;
    int downX=0;
    int downY=0;

    // new scroll vars
    float scrollPos = 0.0f;
    float scrollVel = 0.0f;
    const float friction = 0.90f;
    float velocityX = 0.0f;
    float velocityY = 0.0f;
    const float accel    = 2.0f;
    int itemsPerRow = 8;
    int rowHeight   = 198+20;
    int itemWidth   = 198;
    int startX = 20;
    int startY = 60;
    int startY_search_view = 882;
    int viewHeight = 780;
    // end new scroll vars

    int type;
    bool gfx_loaded;

    stream_class();
    ~stream_class();
    int streamantal() { return(antal); }
    void clean_stream_oversigt();
    void draw_stream_item(int x, int y,int ii,GLuint normal_icon,GLuint empty_icon, int stream_key_selected);
    void show_stream_oversigt(GLuint normal_icon, GLuint empty_icon, int stream_key_selected);
    void playstream(char *url);
    int update_search_podcast_stream_view();
    void onScroll(float delta) { scrollVel += delta * accel; }
    void stopstream();
    void softstopstream();
    float jump_position(float ofset);
    unsigned long get_length_in_ms();
    unsigned long get_position_in_ms();
    int loadrssfile(bool updaterssfile);
    int opdatere_stream_oversigt(char *art,char *fpath);
    int loadweb_stream_iconoversigt();
    int parsexmlrssfile(char *filename,char *baseiconfile);
    void set_rss_new(int nr,bool ny) { if (nr<antal) FeedCatalog[nr].nyt=ny; }                 // set new flag
    bool get_rss_new(int nr) { return(FeedCatalog[nr].nyt); }                 // set new flag
    const char *get_stream_name(int nr);
    const char *get_stream_path(int nr) { if (nr<antal) return (FeedCatalog[nr].feed_path.c_str()); else return(0); }
    const char *get_stream_gfx_url(int nr) { if (nr<antal) return (FeedCatalog[nr].feed_gfx_url.c_str()); else return(0); }
    const char *get_stream_mythtvgfx_path(int nr) { if (nr<antal) return (FeedCatalog[nr].feed_gfx_mythtv.c_str()); else return(0); }
    unsigned int get_stream_groupantal(unsigned int nr) { if (nr<antal) return (FeedCatalog[nr].feed_group_antal); else return(0); }
    unsigned int get_stream_pathantal(unsigned int nr) { if (nr<antal) return (FeedCatalog[nr].feed_path_antal); else return(0); }
    long get_stream_intnr(unsigned int nr) { if (nr<antal) return (FeedCatalog[nr].intnr); else return(0); }
    GLuint get_texture(int nr) { if (nr<antal) return(FeedCatalog[nr].textureId); else return(0); }
    int antalstreams() { return antal; };
    int antal_rss_streams() { return antalrss_feeds; };
    int streams_loaded() { return stream_oversigt_loaded_nr; };
    int streams_rss_loaded() { return stream_rssparse_nowloading; };   
    char *get_stream_url(int nr);
    int FeedCatalog_search_antalstreams();
    const char *get_stream_desc(int nr);
    int playstream_url(char *path); // play by vlc
    void update_rss_nr_of_view(char *url);
    int get_play_status();
};

