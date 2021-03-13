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

class tv_oversigt_prgtype {					// char type fra mythtv ver 0.22/24
    public:
        char program_navn[80];
        char sub_title[128];		                                      // char type from mythtv ver 0.22/24
        char starttime[20];				                                   	// format yyyy-mm-dd tt:mm:ss
        char endtime[20];				                                     	// format yyyy-mm-dd tt:mm:ss
        char description[description_length];                         // prg description
        int starttimeinmin;					                                  // start tid i minuter
        int program_length_minuter;		                             		// længde i minuter
        unsigned long starttime_unix;		                             	// unix time start time
        unsigned long endtime_unix;				                            // unix time end time
        int prg_type;					                                       	// prg type
        bool aktiv;                                                   // aktiv
        bool brugt;           						                            // findes der et prg
        bool updated;                                                 // er den updated
        int recorded;				             	// er tv program optaget med mythtv (0 > yes) Se http://www.mythtv.org/wiki/Record_table, recorded=type from mysql table record can be 0/1/2/3/4/5/6/7/8/9/10
        bool settorecord;				          // er tv program optaget med mythtv (0 > yes) Se http://www.mythtv.org/wiki/Record_table, recorded=type from mysql table record can be 0/1/2/3/4/5/6/7/8/9/10
        bool getprogramupdated() { return (updated); }
        tv_oversigt_prgtype();
        ~tv_oversigt_prgtype();
        int putprograminfo(char *prgname,char *stime,char *etime,char *prglength,char *sunixtime,char *eunixtime,char *desc,char *subtitle,int ptype,int prgrecorded);	// push program info
        void getprograminfo(char *prgname,char *stime,char *etime,int *prglength,unsigned long *sunixtime,unsigned long *eunixtime,char *desc,char *subtitle,int *ptype,bool *bgt,int *prgrecorded);
        void getprogramrecinfo(char *prgname,char *stime,char *etime);
};


class tv_oversigt_pr_kanal {
    private:
        unsigned int programantal;                                  // # of program guide records
        GLuint channel_icon;                                        // channel icon
        char channel_icon_name[200];                                // channel icon file name
    public:
        char chanel_name[20];						                         		// kanal navn
        unsigned int chanid;
        tv_oversigt_prgtype tv_prog_guide[maxprogram_antal];				// program oversigt over kanal navn (pointer array)
        tv_oversigt_pr_kanal();
        ~tv_oversigt_pr_kanal();
        void putkanalname(char *kname);
        char *getkanalname(void) { return(chanel_name); };
        int program_antal() { return programantal; }
        void set_program_antal(int antal) { programantal=antal; }
        GLuint get_kanal_icon() { return(channel_icon); }
        void set_kanal_icon(GLuint icon) { channel_icon=icon; }
        void set_kanal_icon_file(char *file) { strcpy(channel_icon_name,file); }
        void get_kanal_icon_file(char *file) { strcpy(file,channel_icon_name); }
        void cleanprogram_kanal();
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
    public:
        tv_oversigt_pr_kanal tvkanaler[MAXKANAL_ANTAL];                                                       //
        int tvprgrecorded(char *fstarttime,char *ftitle,char *fchannelid);					                          // return type (1/2/3) found of tv program to record
        int tvprgrecordedbefore(char *ftitle,unsigned int fchannelid);                                        //
        bool vistvguidecolors;                                                                                     // vis tv guide kl
        int vistvguidekl;                                                                                     // vis tv guide kl
        tv_oversigt();                                                                                        // constructor
        ~tv_oversigt();                                                                                       // destructor
        //int gettvprogramrecinfo(int channelnr,int prgnr,char *prgname,char *stime,char *etime) { tvkanaler[channelnr].tv_prog_guide[prgnr].getprogramrecinfo(prgname,stime,etime); return(1); }
        int tv_kanal_antal() { return (kanal_antal); }                                                        // return nr of th channels
        void opdatere_tv_oversigt(char *mysqlhost,char *mysqluser,char *mysqlpass,time_t starttid);           //
        void show_fasttv_oversigt(int selectchanel,int selectprg,bool do_update_xmltv_show);                  //
        void show_canal_names();                                                                              //
        //void showandsetprginfo(int kanalnr,int tvprgnr);					                                         	// show the prg info in
        void showandsetprginfo(int tvvalgtrecordnr,int tvsubvalgtrecordnr);                                  	// show the prg info in
        int kanal_prg_antal(int kanalnr) { return tvkanaler[kanalnr].program_antal(); }                       //
        bool changetime(time_t difftime) { starttid+=difftime; sluttid+=difftime; return(true); }             //
        int cleanchannels();                                                                                  // clear all tv channels
        int tvprgrecord_addrec(int tvvalgtrecordnr,int tvsubvalgtrecordnr);                                   //
        int findguidetvtidspunkt(int kanalnr,time_t tidspunkt);	                                  						// finder record nr som svare til starttid i timer
        time_t hentprgstartklint(int kanalnr,int prgnr);                                                      //
        int parsexmltv(const char *filename);                                                                 // parse and update db from tvguide.xml file from users homedir
        time_t getlastupdate() { return lastupdated; }                                                        //
        time_t setlastupdate(time_t timestamp) { lastupdated=timestamp; }                                     //
        void cleartvguide();                                                                                  // drop db in backend
        int find_start_pointinarray(int selectchanel);
        unsigned long getprogram_endunixtume(int selectchanel,int selectprg);
        unsigned long getprogram_startunixtume(int selectchanel,int selectprg);
        char *getprogram_prgname(int selectchanel,int selectprg);                                             // return pointer to prgname in tvguide
        void reset_tvguide_time();                                                                            // reset show tv guide to now (time)
        int saveparsexmltvdb();                                                                               // tvguidedb filename
        int loadparsexmltvdb();                                                                               // tvguidedb filename
        void set_program_torecord(int selectchanel,int selectprg);                                            // tvguidedb filename
        int set_channel_state(channel_list_struct *channel_list);
        //int find_start_kl_returnpointinarray(int selectchanel,int findtime);
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

//bool check_tvguide_process_running(char *processname);
int parsexmltv();                                                               // parse xml file
int get_tvguide_fromweb();                                                      //
unsigned long get_cannel_id(MYSQL *conn,char *channelname);                     //

#endif
