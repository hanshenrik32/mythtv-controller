#ifndef MYCTRL_VLCPLAYER
#define MYCTRL_VLCPLAYER

#include <vlc/vlc.h>
#include <mutex>
#include <vector>

// vlc player class for opengl can play direct in opengl by render 
// AddVideoTextureRect(int id,GLuint videoTexture,float x,float y,float w,float h,float r,float g,float b,float a) call

class subtitle_def {
  public:
    int subtitle_id;
    std::string subtitle_name;
};

class audiotracks_def {
  public:
    int track_id;
    std::string track_name;
};

class vlc_controller {
  private:
    libvlc_instance_t *vlc_inst=nullptr;;              //
    bool is_playing;                          // do we play media
    bool is_pause;                            // do we play media
    int width=1920;
    int height=1080;
    std::vector<unsigned char> pixels;
    std::mutex mutex;
    bool newFrame=false;
    static void *lock(void *opaque, void **planes);
    static void unlock(void *opaque, void *picture, void *const *planes);
    static void display(void *opaque, void *picture);
    GLuint videoTexture=0;
    int subtrack_id=0;    
    bool show_subtitles=true;
    // subtitle vector
    std::vector<subtitle_def> subtitle_tracks = {};
    std::vector<audiotracks_def> audio_tracks = {}; 
    bool audioTracksLoaded=false;
    bool subtitleTracksLoaded=false;
    int audiotrack_active=0;
  public:
    libvlc_media_player_t *vlc_mp=nullptr;            //
    bool vlc_in_playing();
    std::string active_audiotrack_name();
    int active_audiotrack_id();
    vlc_controller();
    ~vlc_controller();
    int playmedia(char *path);
    int playmedia(const char* path,const char* subtitlePath);
    void GetSubtitleTracks();
    bool SelectSubtitle(const std::string& search);
    // audio tracks
    int GetAudioTracks();
    int playwebmedia(char *path);
    void stopmedia();
    unsigned long get_position();
    float set_position(float pos);
    float jump_position(float ofset);
    unsigned long get_length_in_ms();
    void pnext_chapter();
    void plast_chapter();
    void volume_up(int volume);
    void volume_down(int volume);
    void setvolume(int volume);
    void pause(int pause);    
    void createTexture();
    void updateTexture();
    GLuint getVideoTexture() {
        return videoTexture;
    }
    void initOpenGL();
    void tilbage10sec();
    void frem10sec();
    void tilbage60sec();
    void frem60sec();
    int antal_sub_tracks() { return(subtitle_tracks.size()); }
    int antal_audio_tracks() { return(audio_tracks.size()); }
};


#endif
