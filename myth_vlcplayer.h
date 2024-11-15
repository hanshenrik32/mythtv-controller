#ifndef MYCTRL_VLCPLAYER
#define MYCTRL_VLCPLAYER

#include <vlc/vlc.h>

// vlc player class

class vlc_controller {
  private:
    libvlc_instance_t *vlc_inst;              //
    libvlc_media_player_t *vlc_mp;            //
    bool is_playing;                          // do we play media
    bool is_pause;                          // do we play media
  public:
    bool vlc_in_playing();
    vlc_controller();
    ~vlc_controller();
    int playmedia(char *path);
    int playwebmedia(char *path);
    void stopmedia();
    float get_position();
    float set_position(float pos);
    float jump_position(float ofset);
    unsigned long get_length_in_ms();
    void pnext_chapter();
    void plast_chapter();
    void volume_up(int volume);
    void volume_down(int volume);
    void setvolume(int volume);
    void pause(int pause);
};


#endif
