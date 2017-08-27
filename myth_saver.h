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
    GLuint texture;
    GLuint texture_mask;
    GLuint pictexture;
    GLuint musictexture;
};


class boxarray {
  public:
    struct boxpos matrix[11][11];
    float tilt;
    boxarray();
    void set_new_textures(int mode,int type,GLuint newtexture);
    void settexture(struct music_oversigt_type *musicoversigt);
    void show_music_3d(int aangle,GLuint textureId,GLuint textureIdmask,GLuint textureIdmask1);
    void show_music_3d_new(int aangle,GLuint textureId);
    void show_music_3d1(float aangle,GLuint textureId);
    void loadboxpictures();
    void createboxpictures();
};




//void show_music_3d(int angle,GLuint textureId,GLuint textureIdmask,GLuint textureIdmask1);


#endif
