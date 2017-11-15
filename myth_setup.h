#ifndef MYTV_SETUP
#define MYTV_SETUP

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

const int MAXPRGLIST_ANTAL=200;

struct tv_graber_config {
  int graberantal=24;
  int graberaktivnr=0;                                                          // avtive tvguide nr in list
  const char *graberland[35]={"None","North America","Holland","Spain","Israel","North America","Latin America","Osa 9/10","Europe tv","Sweden","Portugal","France","United Kingdom","Hungary/Romania","Switzerland","Italy","Iceland","Finland","North America","Tyrkiye","German speaking area","Danmark","Sweden","Argentina","France","UK"};
  const char *grabercmd[35]={"","tv_grab_na_dd","tv_grab_nl","tv_grab_es_laguiatv","tv_grab_il","tv_grab_na_tvmedia","tv_grab_dtv_la","tv_grab_fi","tv_grab_eu_dotmedia","tv_grab_se_swedb","tv_grab_pt_meo","tv_grab_fr","tv_grab_uk_bleb","tv_grab_huro","tv_grab_ch_search","tv_grab_it","tv_grab_is","tv_grab_fi_sv","tv_grab_na_dtv","tv_grab_tr","tv_grab_eu_egon","tv_grab_dk_dr","tv_grab_se_tvzon","tv_grab_ar","tv_grab_fr_kazer","tv_grab_uk_tvguide"};
};

struct channel_list_struct {
  bool selected;
  char id[80];                                                                  // channel_list array used in setup graber
  char name[80];                                                                // channel_list array used in setup graber
};


class channel_configfile {
  private:
    int maxconfigfilesize=400;
    int configfilesize=0;
    char configtext[400][80];
  public:
    void readgraber_configfile();
    void writegraber_configfile();
    void graber_configbuild();
};



bool save_channel_list();
int load_channel_list();

void show_setup_interface();
void show_setup_screen();
void show_setup_video();
void show_setup_sound();
void show_setup_sql();
void show_setup_tema();
void show_setup_network();
void show_setup_font(int startofset);
void show_setup_keys();
void show_setup_tv_graber(int startofset);
void load_channel_list_from_graber();
void show_wlan_networks(int valgtnr);

#endif
