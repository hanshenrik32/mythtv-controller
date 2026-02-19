#ifndef MYCTRL_MUSIC
#define MYCTRL_MUSIC

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <string>

//#include <irrKlang.h>
//used in music oversigt

// class for music oversigt

class music_oversigt_type {
  public:
    unsigned int album_id;
    unsigned int artist_id;
    char album_name[256];			      // music name
    int album_year;
    unsigned int directory_id;			// dir id fra myth mysql database
    unsigned int parent_id;         //
    char album_coverfile[256];			// coverfilename
    char album_path[256];	      		// path to cover
    GLuint textureId;			        	// texture id hvis der findes en cover til musiken
    char oversigttype;				      // type =0 normal dir oversigt 1 = playlist
};



struct Color3 {
    float r, g, b, a;
};


class musicoversigt_class {
  private:
    unsigned int antal_music_oversigt;
    std::vector <music_oversigt_type> musicoversigt;
    bool do_play;
    bool music_is_playing;                                                // do we play ?   
  public:
    int selected_icon_in_view=1;
    char overview_show_band_name[256];                                  // show band name in overview
    bool search_loaded;
    int searchtype;
    bool loaded_begin;
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

    int startX = 20;
    int startY = 895;
    int viewHeight = 780;
    // end new scroll vars

    int search_startX = 20;
    int search_startY = 762;
    int search_viewHeight = 660;
    // end new scroll vars
    bool search_music_online_done;
    bool play() { return(do_play);}    
    void clean_music_oversigt();
    bool set_search_loaded() { search_loaded=true; return(1); }
    
    // load covers
    void opdatere_music_oversigt_icons();
    int opdatere_music_oversigt_nodb();
    int opdatere_music_oversigt(unsigned int directory_id);
    int opdatere_music_oversigt_searchtxt(char *keybuffer,int type);
    // bruges til at finde kunstnere eller sange hvis art >0
    
    // load all covers
    int load_music_covergfx();
    int save_music_oversigt_playlists(char *playlistname);
    int load_music_oversigt_playlists(char *playlistname);
    // load playliste find_dir_id
    int get_music_pick_playlist(long find_dir_id,bool *music_list_select_array);
    // show playlistes in opengl
    // void show_music_oversigt(GLuint normal_icon,GLuint back_icon,GLuint dirplaylist_icon,int _mangley,int music_key_selected);
    void show_search_music_oversigt(GLuint normal_icon,GLuint back_icon,GLuint dirplaylist_icon,int _mangley,int music_key_selected);
    // henter playlist oversigt
    int opdatere_music_oversigt_playlists();
    char *get_album_name(int nr);
    char get_album_type(int nr);
    unsigned int get_directory_id(int nr);
    char *get_album_path(int nr);
    GLuint get_textureId(int nr);
    int play_songs(bool setplay) { do_play=setplay; return(1); }
    bool play_songs_status() { return(do_play); }
    int update_afspillinger_music_song(char *filename);
    void set_music_is_playing(bool flag) { music_is_playing = flag; }
    bool get_music_is_playing() { return(music_is_playing); }
    int antal() { return(musicoversigt.size()); }


    // new version 2 with kinetic scroll
    void onScroll(float delta) { scrollVel += delta * accel; }
    void draw_music_item(int x, int y,int ii,GLuint normal_icon,GLuint empty_icon,GLuint back_icon, int stream_key_selected);
    void draw_music_search_item(int x, int y,int ii,GLuint normal_icon,GLuint empty_icon,GLuint back_icon, int stream_key_selected);
    void show_music_oversigt(GLuint normal_icon,GLuint back_icon,GLuint dirplaylist_icon,int _mangley,int music_key_selected);
    void show_search_music_oversigt1(GLuint normal_icon,GLuint back_icon,GLuint dirplaylist_icon,int _mangley,int music_key_selected);
};

void get_music_pick_playlist(long find_dir_id,bool *music_list_select_array);

// gfx loaded
static bool music_oversigt_loaded=false;
const int COSTABLE_ANTAL=37;
const unsigned int MUSIC_OVERSIGT_TYPE_SIZE=2000;                                       // MAX cd er i oversigt
// check music exist internal db return true if exist
bool global_use_internal_music_loader_system_exist();

#endif
