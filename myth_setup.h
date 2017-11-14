#ifndef MYTV_SETUP
#define MYTV_SETUP

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

const int MAXPRGLIST_ANTAL=200;

struct channel_list_struct {
  bool selected;
  char id[80];                             // channel_list array used in setup graber
  char name[80];                             // channel_list array used in setup graber
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
