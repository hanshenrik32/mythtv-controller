#ifndef MYCTRL_VLCPLAYER
#define MYCTRL_VLCPLAYER

#include <vlc/vlc.h>

// vlc player class

// vlc stuf


class vlc_controller {
  private:
    libvlc_instance_t *vlc_inst;              //
    libvlc_media_player_t *vlc_mp;            //
    bool is_playing;
  public:
    vlc_controller();
    ~vlc_controller();
    int playmovie(char *path);
    void stopmovie();
    float get_position();
    void pause();
};


#endif
