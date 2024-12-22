#ifndef MYCTRL_MOVIE
#define MYCTRL_MOVIE
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include "myth_vlcplayer.h"

// #include <vlc/vlc.h>

// structur som bruges når der skal vises list over film i 3d view
// hentet fra mythtv eller xbmc databasen
// underfilm class

const unsigned int FILM_OVERSIGT_TYPE_SIZE=2000;                                       // MAX movies in overview
const int castlinieantal=20;

class film_oversigt_type {
  private:
    unsigned int nr;	               		// record nr
    GLuint textureId;                   // texture id for 3D cover hvis der findes en cover til filmen
    GLuint frontcover;                  // normal cover
    unsigned int film_id;               // filmid i mysql
    unsigned long length;                // film length
    unsigned int year;                  // aar som den udkom
    unsigned int userrating;            // bruger rating
    unsigned int Bitrate;               // movie Bitrate
    unsigned int Frate;    
    unsigned int Width;
    unsigned int High;
    unsigned long Flesize;

    char *format;
    char *rating;			                  // imdb rating size = 100
    char *film_imdbnr;                  // movie imdb nr if any size = 20
    char *film_title;                   // film title size = 128
    char *film_coverfile;               // path to created (by convert) 3d gfx file 256
    char *film_filename;                // path to file 256
    char *film_adddate;			            // date added to db
    bool cover3d;		                   	// show 3d cover in movie display
  public:
    char *film_subtitle;                // film info (sub title) 1024
    char *category_name;		            // film type (tal = database) size = 128
    char *genre;			                  // size = 200
    char cast[castlinieantal][200];			//
    film_oversigt_type();		            // constructor
    ~film_oversigt_type();		          // destructor
    // functions
    // swap movies
    void swap_film(film_oversigt_type *film1,film_oversigt_type *film2);
    void setBitrate(int rate) { Bitrate=rate; }
    int getBitrate() { return(Bitrate); }
    void setFramerate(int rate) { Frate=rate; }
    int getFramerate() { return(Frate); }
    void setWidth(int wid) { Width=wid; }
    int  getWidth() { return(Width); }
    void setFormat(char *vformat) { if (format) strcpy(format,vformat); }
    char *getFormat() { return(format); }
    void setHigh(int hi) { High=hi; }
    int  getHigh() { return(High); }
    void setSize(int siz) { Flesize=siz; }
    unsigned long getSize() { return(Flesize); }
    void setfilmnr(unsigned int intnr) { nr=intnr; }
    unsigned int getfilmnr() { return(nr); }
    bool getcover3d() { return(cover3d); }
    void setcover3d(bool value) { cover3d=value; }
    GLuint gettextureid() { return (textureId); }
    void settextureid(GLuint value) { textureId=value; }
    void settextureidfile(char *filename);
    GLuint getfronttextureid() { return (frontcover); }
    void setfronttextureid(GLuint value) { frontcover=value; }
    // void setfronttextureidfile(char *filename);
    // GLuint getbacktextureid() { return (backcover); }
    // void setbacktextureid(GLuint value) { backcover=value; }
    // void setbacktextureidfile(char *filename);
    // void loadbacktextureidfile();
    // GLuint getsidetextureid() { return (sidecover); }
    // void setsidetextureid(GLuint value) { sidecover=value; }
    // void setsidetextureidfile(char *filename);
    // void loadsidetextureidfile();
    void setfilmid(unsigned int value) { film_id=value; }
    unsigned int getfilmid() { return(film_id); }
    void setfilmlength(unsigned long value) { length=value; }
    unsigned int getfilmlength() { return(length); }
    void setfilmaar(unsigned int value) { year=value; }
    unsigned int getfilmaar() { return(year); }
    void setfilmrating(unsigned int value) { userrating=value; }
    unsigned int getfilmrating() { return(userrating); }
    void setimdbfilmrating(char *value) { strcpy(rating,value); }
    char *getimdbfilmrating() { return(rating); }
    void setfilmimdbnummer(char *value) { strcpy(film_imdbnr,value); }
    char *getfilmimdbnummer() { return(film_imdbnr); }
    void setfilmtitle(char *value) { strcpy(film_title,value); }
    char *getfilmtitle() { return(film_title); }
    void setfilmcoverfile(char *value) { strcpy(film_coverfile,value); }
    char *getfilmcoverfile() { return(film_coverfile); }
    void setfilmfilename(char *value) { strcpy(film_filename,value); }
    char *getfilmfilename() { return(film_filename); }
    void setfilm_adddate(char *value) { strncpy(film_adddate,value,24); }
    char *getfilm_adddate() { return(film_adddate); }
    void setfilmgenre(char *value) { strcpy(genre,value); }
    char *getfilmgenre() { return(genre); }
    void setfilmsubtitle(char *value) { strcpy(film_subtitle,value); }
//    char *getfilmsubtitle() { return(film_subtitle); }
    void resetfilm();

    bool get_media_info_from_file(char *moviepath);

};



// main film class

class film_oversigt_typem : vlc_controller {
  private:
      unsigned int filmoversigtsize;			                                      // antal elementer i array MAX def
      bool film_oversigt_loaded;
      bool film_is_pause;                                                       // pause ?
      unsigned int filmoversigt_antal;			  	                                // loaded antal
      int volume;
      // load vlc stuf
//      libvlc_instance_t *vlc_inst;              //
//      libvlc_media_player_t *vlc_mp;            //
      float getmovieposition();
      bool show_search_view;
  public:
      bool get_search_view() { return (show_search_view); }
      void set_search_view(bool val) { show_search_view=val; }
      bool film_is_playing;                                                     // playing ?
      film_oversigt_type *filmoversigt;                                         // all movie array to show in movie overview
      void sortfilm(int type);
      unsigned int film_antal(void) { return(filmoversigt_antal); }
      void set_film_antal(unsigned int antal) { filmoversigt_antal=antal; }
      int get_film_antal() { return (filmoversigt_antal); }
      // show movie oversigt
      void show_film_oversigt(float _mangley,int filmnr);			// new ver
      // show startup new added movies
      void show_minifilm_oversigt(float _mangley,int filmnr);
      // overload func
      int opdatere_film_oversigt(void);
      int opdatere_film_oversigt(char *movietitle);
      void resetallefilm();
      int load_film_dvcovers();
      int playmovie(int nr);
      void stopmovie();
      void next_movie_chapther();
      void prevous_movie_chapther();
      void volumeup();
      void volumedown();
      void setcolume(int volume);                                         // do not store volume in volume member int vaule
      void softstopmovie();
      void pausemovie();      
      film_oversigt_typem(unsigned int antal);
      ~film_oversigt_typem();
};


#endif
