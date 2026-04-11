#ifndef MYCTRL_RADIO
#define MYCTRL_RADIO

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include "json-parser/json.h"
//#include <irrKlang.h>

const int stationamelength=40;


// station info struct
struct radio_oversigt_type {
    char station_name[stationamelength+1];			// station name
    std::string desc;
    std::string streamurl;
    std::string homepage;
    std::string gfxfilename;
    int art;
    int land;
    bool noiconloaded;
    bool online;
    bool aktiv;
    bool texture_r_loaded=false;

    unsigned int kbps;
    GLuint textureId;
    GLuint textureId_r;
    long intnr;
};

struct radiotype_type {
    char radiosortopt[60];			// type navn
    int radiosortoptart;		  	// sort type
    int antal;				        	// antal af typen
};


int set_radio_popular(int stationid);                //
const int radiooptionsmax=40;				                 // mxa antal typer af radio stationer


struct Color2 {
    float r, g, b, a;
};



class radiostation_class {
  private:
    enum { maxantal=10000 };				          		                        // MAX antal radio stationer
    std::vector<radio_oversigt_type> stack;
    int antal;							                                            // Antal
    radiotype_type radiosortopt[radiooptionsmax];			                        // radio typer
    int radiooptionsselect;						                                    // bruges til valgt af radio type som skal vises
    void set_texture(int nr,GLuint idtexture);
    int opdatere_radiostation_gfx(int nr,char *gfxpath);		                    // update gfx
    bool startup_loaded;			                                         	    // load radio statios list
    bool check_radio_online_bool();                                                 // check and set radio station online flag (DO NOT WORK)
    bool hentradioart=false;
  public:
    int playingstationnr=-1;		                                                // nr in stack of playing radio station
    bool search_radio_online_done;
    bool search_loaded;
    int selected_icon_in_view=1;
    bool playing;                                                                   // playing radio station
    unsigned long check_radio_online(unsigned int startrecnr);	                    // check and set radio station online flag (DO NOT WORK)
    void nextradiooptselect();		                                    			// select next type in radio type oversigt
    void lastradiooptselect();                                      				//
    void show_radio_options();			                                     		//
    int getradiooptionsselect()  { return(radiosortopt[radiooptionsselect].radiosortoptart); }
    int set_radio_populars(int stationid);
    //
    char *get_station_name(int nr) { return (stack[nr].station_name); }
    const char *get_station_gfxfile(int nr) { return (stack[nr].gfxfilename.c_str()); }
    const char *get_stream_url(int nr) { return (stack[nr].streamurl.c_str()); }
    const char *get_homepage(int nr) { return (stack[nr].homepage.c_str()); }
    const char *get_desc(int nr) { return (stack[nr].desc.c_str()); }
    void set_kbps(int nr,int kbps) { stack[nr].kbps=kbps; }
    int load_radio_stations_gfx();		                                			// load all radio stations gfx
    GLuint get_texture(int nr) { return (stack[nr].textureId); }
    int get_kbps(int nr) { return (stack[nr].kbps); }
    int set_radio_popular(int stationid);
    int set_radio_online(int stationid,bool onoff);
    int set_radio_aktiv(int stationid,bool onoff);
    int set_radio_intonline(int arraynr);
    int get_radio_online(int stationid);
    radiostation_class();
    ~radiostation_class();
    int radioantal() { return(stack.size()); }
    void clean_radio_oversigt();
    int opdatere_radio_oversigt(int radiosortorder);
    int opdatere_radio_oversigt(char *searchtxt);
    int opdatere_radio_oversigt();

    // bool show_radio_oversigt(GLuint normal_icon,GLuint normal_icon_mask,GLuint back_icon,GLuint dirplaylist_icon,int _mangley);

    int downTimeMs=0;
    bool moved=false;
    bool gettouchbutton;
    int downX=0;
    int downY=0;

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

    // new scroll vars
    float scrollPos = 0.0f;
    float scrollVel = 0.0f;

    const float friction = 0.90f;
    float velocityX = 0.0f;
    float velocityY = 0.0f;
    const float accel    = 2.0f;

    int itemsPerRow = 8;
    int rowHeight   = 198;
    int itemWidth   = 198;

    int search_startX = 20;
    int search_startY = 762;
    int search_viewHeight = 660;

    int startX = 20;
    int startY = 882;
    int viewHeight = 780;
    // end new scroll vars
    void onScroll(float delta) { scrollVel += delta * accel; }
    void draw_radio_item(int x, int y,int ii,GLuint normal_icon,GLuint empty_icon, int stream_key_selected);  // draw single stream item
    void draw_radio_search_item(int x, int y,int ii,GLuint normal_icon,GLuint empty_icon, int radio_key_selected);
    bool show_radio_oversigt(GLuint normal_icon,GLuint normal_icon_mask,GLuint back_icon,GLuint dirplaylist_icon,int _mangley);


    GLuint get_texture_r(int nr);
    // json parser
    int load_radio_stations_from_json_file();
    int radio_download_image(char *imgurl,char *filename);
    void process_object_radio(json_value* value, int depth);
    void process_array_radio(json_value* value, int depth);
    void process_value_radio(json_value* value, int depth,int x);

};

#endif
