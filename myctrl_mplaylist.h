#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>


const int MAX_IN_PLAYLIST=512;		// max numbers of songs in the aktiv playlist
const int MAX_SONGLENGTH_PATH=255;

struct playlist {
  char filename[MAX_SONGLENGTH_PATH];		// sang navn + path
  char songname[MAX_SONGLENGTH_PATH];		// sang navn far db
  char artistname[MAX_SONGLENGTH_PATH];		// artist navn fra db
  unsigned int length;
  unsigned int song_id;
  unsigned int artist_id;
  char albumname[100];				// 255 from mythtv
  GLuint textureId;  				// texture id for sang
  bool selected;				// sang valgt til playlist default
};


class mplaylist {
    playlist aktiv_playlist[MAX_IN_PLAYLIST]; 				// MAX_IN_PLAYLIST sange max
    int playlist_length;						// antal aktive sangle i playliste
  public:  
//    void m_play_playlist();
    int m_add_playlist(char *file,char *songid,char *artistid,char *albumname,char *songname,char *artistname,char *songlength,int position,GLuint texture);
    int numbers_in_playlist();
    int m_play_playlist(char *path,int recnr);			// hent path sti til mp3/ogg/flac file to play
    GLuint get_textureid(int nr);
    unsigned int get_songid(int recnr);
    unsigned int get_artistid(int recnr);
    int get_albumname(char *resl,int recnr);
    unsigned int get_length(int recnr);
    int get_songname(char *songname,int recnr);			// hent song navn
    int get_artistname(char *resl,int recnr);			// hent artist navn fra database
    bool get_selected(int recnr);
    bool set_selected(int recnr);
    void clean_playlist();					// clean playlist
    mplaylist();
    ~mplaylist();  
};
