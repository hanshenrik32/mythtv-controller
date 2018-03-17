#ifndef MYCTRL_MUSIC
#define MYCTRL_MUSIC


#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

//#include <irrKlang.h>

//used in music oversigt

struct music_oversigt_type {
    unsigned int album_id;
    unsigned int artist_id;
    char album_name[256];			      // music name
    int album_year;
    unsigned int directory_id;			// dir id fra myth mysql database
    unsigned int parent_id;
    char album_coverfile[256];			// coverfilename
    char album_path[256];	      		// path to cover
    GLuint textureId;			        	// texture id hvis der findes en cover til musiken
    char oversigttype;				      // type =0 normal dir oversigt 1 = playlist
};


// class tempplate to music oversigt;

template <class T> class list {
    public:
        list *insert(T val);
    private:
        list (T Linkalue,list * next);
        T value;
        list *ptrtonext;
};


// gfx loaded
static bool music_oversigt_loaded=false;


const int COSTABLE_ANTAL=37;
const unsigned int MUSIC_OVERSIGT_TYPE_SIZE=2000;                                       // MAX cd er i oversigt
//int music_key_selected=1;
//void zoom_music_info();
// used internt til music db
int opdatere_music_oversigt_nodb(char *dirpath,music_oversigt_type musicoversigt[]);
//void draw_musiccover();
int opdatere_music_oversigt(music_oversigt_type musicoversigt[],unsigned int directory_id);
// bruges til at finde kunstnere eller sange hvis art >0
int opdatere_music_oversigt_searchtxt(music_oversigt_type musicoversigt[],char *searchtxt,int search_art);
// henter playlist oversigt
int opdatere_music_oversigt_playlists(music_oversigt_type musicoversigt[]);				// mythtv playlist
//void show_music_oversigt1(music_oversigt_type musicoversigt[],GLuint textureId,GLuint textureId_mask,GLuint textureId_covermask,GLuint textureId_playlist,GLuint textureId_playlist_mask,GLuint textureId_playlistback,GLuint textureId_playlistback_mask,float music_angley);
void show_music_oversigt(music_oversigt_type *musicoversigt,GLuint normal_icon,GLuint back_icon,GLuint dirplaylist_icon,GLuint dirplaylist_icon_mask,int _mangley,int music_key_selected);		// new ver
//void show_newmusic_oversigt(music_oversigt_type *musicoversigt,GLuint normal_icon,GLuint normal_icon_mask,GLuint back_icon,GLuint dirplaylist_icon,GLuint dirplaylist_icon_mask,int _mangley);
// load playliste find_dir_id
void get_music_pick_playlist(long find_dir_id,bool *music_list_select_array);
void zoom_music_info();

int load_music_covergfx(music_oversigt_type musicoversigt[]);

// check music exist internal db return true if exist
bool global_use_internal_music_loader_system_exist();

#endif
