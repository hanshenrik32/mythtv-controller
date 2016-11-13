#ifndef MYCTRL_RADIO
#define MYCTRL_RADIO


#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

//#include <irrKlang.h>

//const char *radioiconpath="/usr/share/mythtv-controller/images/radiostations/";
const int stationamelength=40;
const int statiodesclength=200;
const int statiourl_homepage=200;

// station info struct

struct radio_oversigt_type {
    char station_name[stationamelength+1];			// station name
    char desc[statiodesclength];
    char streamurl[statiourl_homepage];
    char homepage[statiourl_homepage];
    char gfxfilename[stationamelength];
    int art;
    int land;
    bool online;
    bool aktiv;
    unsigned int kbps;
    GLuint textureId;
    long intnr;
};

struct radiotype_type {
    char radiosortopt[60];			// type navn
    int radiosortoptart;			// sort type
    int antal;					// antal af typen
};


int set_radio_popular(int stationid);

const int radiooptionsmax=40;					// mxa antal typer af radio stationer

class radiostation_class {
    private:
        enum { maxantal=1000 };						// MAX antal radio stationer
        radio_oversigt_type *stack[maxantal];				// radio stack
        int antal;							// Antal
        radiotype_type radiosortopt[radiooptionsmax];			// radio typer
        int radiooptionsselect;						// bruges til valgt af radio type som skal vises
        void set_texture(int nr,GLuint idtexture);
        int opdatere_radiostation_gfx(int nr,char *gfxpath);		//
        bool startup_loaded;						// load radio statios list
        bool check_radio_online_bool();        
        
    public:
        unsigned long check_radio_online(unsigned int startrecnr);	// check and set radio station online flag
        void nextradiooptselect();					// select next type in radio type oversigt
        void lastradiooptselect();        				//
        void show_radio_options();					//
        int getradiooptionsselect()  { return(radiosortopt[radiooptionsselect].radiosortoptart); }
        int set_radio_populars(int stationid);
        //
        char *get_station_name(int nr) { return (stack[nr]->station_name); }
        char *get_station_gfxfile(int nr) { return (stack[nr]->gfxfilename); }
        char *get_stream_url(int nr) { return (stack[nr]->streamurl); }
        char *get_homepage(int nr) { return (stack[nr]->homepage); }
        char *get_desc(int nr) { return (stack[nr]->desc); }
        void set_kbps(int nr,int kbps) { stack[nr]->kbps=kbps; }
        int load_radio_stations_gfx();					// load all radio stations gfx
        GLuint get_texture(int nr) { return (stack[nr]->textureId); }
        int get_kbps(int nr) { return (stack[nr]->kbps); }
        int set_radio_popular(int stationid);
        int set_radio_online(int stationid,bool onoff);
        int set_radio_aktiv(int stationid,bool onoff);
        int set_radio_intonline(int arraynr);
        int get_radio_online(int stationid);
        radiostation_class();
        ~radiostation_class();
        int radioantal() { return(antal); }
        void clean_radio_oversigt();
        int opdatere_radio_oversigt(int radiosortorder);
        int opdatere_radio_oversigt(char *searchtxt);
        void show_radio_oversigt(GLuint normal_icon,GLuint normal_icon_mask,GLuint back_icon,GLuint dirplaylist_icon,int _mangley);
        bool show_radio_oversigt1(GLuint normal_icon,GLuint normal_icon_mask,GLuint back_icon,GLuint dirplaylist_icon,int _mangley);
};

#endif
