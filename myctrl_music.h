#ifndef MYCTRL_MUSIC
#define MYCTRL_MUSIC

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

//#include <irrKlang.h>
//used in music oversigt


// class for music oversigt

class music_oversigt_type {
  public:
    music_oversigt_type *insert();
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


const unsigned int MAX_MUSIC_OVERSIGT_SIZE=300;                                       // MAX cd er i oversigt

class musicoversigt_class {
  private:
    // const unsigned int MAX_MUSIC_OVERSIGT_SIZE=300;                                       // MAX cd er i oversigt
    unsigned int antal_music_oversigt;
    music_oversigt_type musicoversigt[MAX_MUSIC_OVERSIGT_SIZE+1];
    bool do_play;
    bool music_is_playing;                                                // do we play ?
    
  public:
    char overview_show_band_name[256];                                  // show band name in overview
    bool search_loaded;
    int searchtype;
    bool loaded_begin;
    
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
    void show_music_oversigt(GLuint normal_icon,GLuint back_icon,GLuint dirplaylist_icon,int _mangley,int music_key_selected);
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
};

void get_music_pick_playlist(long find_dir_id,bool *music_list_select_array);

// gfx loaded
static bool music_oversigt_loaded=false;
const int COSTABLE_ANTAL=37;
const unsigned int MUSIC_OVERSIGT_TYPE_SIZE=2000;                                       // MAX cd er i oversigt
// check music exist internal db return true if exist
bool global_use_internal_music_loader_system_exist();

#endif
