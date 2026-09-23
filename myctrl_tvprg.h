#ifndef MYCTRL_TVPRG
#define MYCTRL_TVPRG

#include <time.h>
#include <string.h>
#include "myth_setup.h"

// antal programer pr kanal
const int maxprogram_antal=400;
// antal kanaler max
const int MAXKANAL_ANTAL=200;
const int description_length=2048;
const char tvguidedbfilename[]={"tvguidedb.dat"};

// tv oversigt
// denne table inden holder aktiv tv program for i dag. hentet fra mythtv eller internal database samme format

class DRProgram {
  public:
    std::string channel;
    std::string broadcastChannel;
    std::string title;
    std::string description;
    std::string start;
    std::string end;
    std::string preview_image;
    std::string id;
    std::string keywords;
    int program_length_minuter;		                             		// længde i minuter
    GLuint texture=0;
    GLuint preview_image_texture=0;
    unsigned long starttime_unix;		                             	// unix time start time
    unsigned long endtime_unix;				                            // unix time end time
    int recorded;
    bool set_to_record;
    bool aktive;
    int program_idnr;
};



class tv_oversigt_pr_kanal {
  private:
    unsigned int programantal;                                  // # of program guide records
  public:
    std::string channel_icon_name;                                // channel icon file name
    GLuint channel_icon=0;                                        // channel icon
    // char chanel_name[20];						                         		// kanal navn
    std::string kanal_name;						                         		// kanal navn
    unsigned int chanid;
    // new from dr.dk json file
    std::vector<DRProgram> programs;
    // old not in use anymore
    tv_oversigt_pr_kanal();
    ~tv_oversigt_pr_kanal();
    std::string getkanalname(void) { return(kanal_name); };
    int program_antal() { return programs.size(); }
    void set_program_antal(int antal) { programantal=antal; }
    GLuint get_kanal_icon() { return(channel_icon); }
    void set_kanal_icon(GLuint icon) { channel_icon=icon; }
};


class tv_oversigt {
  private:
    int kanal_antal;                                                                                      // # of channels in array
    int vis_kanal_antal;                                                                                  // # of channels max show in tv guide on same screen
    time_t starttid;
    time_t sluttid;
    char mysqllhost[200];
    char mysqlluser[200];
    char mysqllpass[200];
    char loadinginfotxt[200];
    int removetvprgrecorded(char *fstarttime,char *ftitle,char *fchannelid);                              //
    time_t lastupdated;                                                                                   // last updated unix date
    void opdatere_tv_oversigt_kanal_icons();                                                              // load kanal icons

    // new scroll vars
    float scrollPos = 0.0f;
    float scrollVel = 0.0f;

    const float friction = 0.90f;
    float velocityX   = 0.0f;
    float velocityY   = 0.0f;
    const float accel = 2.0f;

    int itemsPerRow = 1;
    int rowHeight   = 62;
    int itemWidth   = 198;

    int startX = 240;
    int startY = 300;                                           // start y
    int viewHeight = 760;                  // 780;
    // end new scroll vars
    int createdb_if_not_exist();
    tv_oversigt_pr_kanal new_kanal;                                 // used to add new entrys
  public:
    bool vis_tv_guide=false;                                        // used as flag to show tv guide in movie playing overlay
    float fade=0.6f;
    std::time_t visdato_unixtime=0;
    int dags_ofset=0;
    int start_ofset=0;                                                                                    // vis kanal start ofset i vector programs 
    int vis_kanal_nr;
    int vis_program_nr;
    int edit_mode=0;
    // new from dr.dk json file
    std::vector<tv_oversigt_pr_kanal> tvkanaler;
    int tvprgrecorded(char *fstarttime,char *ftitle,char *fchannelid);					                  // return type (1/2/3) found of tv program to record
    int tvprgrecordedbefore(char *ftitle,unsigned int fchannelid);                                        //
    bool vistvguidecolors;                                                                                // vis tv guide kl
    int vistvguidekl;                                                                                     // vis tv guide kl
    tv_oversigt();                                                                                        // constructor
    ~tv_oversigt();                                                                                       // destructor
    int tv_kanal_antal() { return (kanal_antal); }                                                        // return nr of th channels
    void show_tv_oversigt(int selectchanel,int selectprg,bool do_update_xmltv_show);
    int cleanchannels();                                                                                  // clear all tv channels
    int tvprgrecord_addrec(int tvvalgtrecordnr,int tvsubvalgtrecordnr);                                   //
    time_t hentprgstartklint(int kanalnr,int prgnr);                                                      //
    time_t getlastupdate() { return lastupdated; }                                                        //
    time_t setlastupdate(time_t timestamp) { lastupdated=timestamp; return(timestamp); }
    void cleartvguide();                                                                                  // drop db in backend
    int find_start_pointinarray(int selectchanel);
    unsigned long getprogram_endunixtume(int selectchanel,int selectprg);
    unsigned long getprogram_startunixtume(int selectchanel,int selectprg);
    std::string getprogram_prgname(int selectchanel,int selectprg);                                             // return pointer to prgname in tvguide
    void reset_tvguide_time();                                                                            // reset show tv guide to now (time)
    int saveparsexmltvdb();                                                                               // tvguidedb filename
    int loadparsexmltvdb();                                                                               // tvguidedb filename
    void set_program_torecord(int selectchanel,int selectprg);                                            // tvguidedb filename
    int set_channel_state(channel_list_struct *channel_list);
    // in use
    void showandsetprginfo();                                  	                                          // show the prg info in
    void onScroll(float delta) { scrollVel += delta * accel; }
    int findguidetvtidspunkt(int kanalnr,time_t tidspunkt);
    int get_dr_proguide(int ofset_from_today);
    int update_guide();
    int opdatere_tv_oversigt(char *mysqlhost,char *mysqluser,char *mysqlpass);
};


struct tvrec {
  char name[80];
  char dato[40];
  char endtime[12];
};


class earlyrecorded {
  private:
    tvrec programinfo[200];
    int antal;
  public:
    earlyrecorded();
    void earlyrecordedload(char *mysqlhost,char *mysqluser,char *mysqlpass);
    void showearlyrecorded();
    void getrecordprogram(char *mysqlhost,char *mysqluser,char *mysqlpass);
    void showtvreclist();
};

const char ugedage[7][12]={"Søndag","Mandag","Tirsdag","Onsdag","Torsdag","Fredag","Lørdag"};
const char ugedageus[7][12]={"Sunday","Monday","tuesday","Wednesday","Thursday","Friday","Saturday"};
const char upcommingrec[]="Upcoming Recordings.";
const char oldrecordning[]="Old recordeds.";
unsigned long get_cannel_id(MYSQL *conn,char *channelname);                     //
int hent_tv_kanal_icons();
#endif
