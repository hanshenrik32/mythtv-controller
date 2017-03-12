#ifndef MYCTRL_VLCPLAYER
#define MYCTRL_VLCPLAYER

#include <vlc/vlc.h>

// vlc player class

class vlc_controller {
  private:
    libvlc_instance_t *vlc_inst;              //
    libvlc_media_player_t *vlc_mp;            //
    bool is_playing;                          // do we play media
  public:
    bool vlc_in_playing();
    vlc_controller();
    ~vlc_controller();
    int playmedia(char *path);
    void stopmedia();
    float get_position();
    void pause();
};


#endif
