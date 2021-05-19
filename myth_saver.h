#ifndef MYCTRL_SAVER
#define MYCTRL_SAVER

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "myctrl_music.h"


struct boxpos {
    float xpos;
    float ypos;
    float zpos;
    float sizex;
    float sizey;
    float sizez;
    GLuint texture;           // texture default to show
    GLuint pictexture;        // other texture to use
    GLuint musictexture;      // other texture to use
};


class boxarray {
  private:
    void build_default_array();
  public:
    struct boxpos matrix[11][11];
    float tilt;
    boxarray();
    void set_new_textures(int mode,int type,GLuint newtexture);
    void settexture(struct music_oversigt_type *musicoversigt);
    void show_music_3d(int aangle,GLuint textureId,GLuint textureId2,GLuint textureId3);
    //void show_music_3d_new(int aangle,GLuint textureId);
    void show_music_3d_2(float aangle,GLuint textureId);
    void loadboxpictures();
    void createboxpictures();
};




//void show_music_3d(int angle,GLuint textureId,GLuint textureIdmask,GLuint textureIdmask1);


#endif
