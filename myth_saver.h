#ifndef MYCTRL_SAVER
#define MYCTRL_SAVER

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "myctrl_music.h"

// screen saver type in config file

// analog     urtype=ANALOG - Analog ur
// digital    urtype=DIGITAL - Float digital clock
// 3D2        urtype=SAVER3D2 - giver rotate box
// 3D         urtype=SAVER3D -  giver wave rotate box
// PICTURE3D  urtype=PICTURE3D - OK
// MUSICMETER urtype=MUSICMETER - OK

struct boxpos {
    float xpos;               // pos
    float ypos;
    float zpos;
    float sizex;              // size
    float sizey;
    float sizez;
    GLuint texture;           // texture default to show
    GLuint pictexture;        // other texture to use
    GLuint musictexture;      // other texture to use
};

const int BOX_MATRIX_ANTAL=11;

class boxarray {
  private:
    void build_default_array();
    void set_new_textures(int mode,int type,GLuint newtexture);
    struct boxpos matrix[BOX_MATRIX_ANTAL][BOX_MATRIX_ANTAL];                  // box matrix
    float tilt;                                                                // tilt array
  public:
    boxarray();
    bool get_loaded_status() { return(matrix[0][0].pictexture); }             // return true if texture is loaded
    void settexture();
    void show_music_3d(int aangle,GLuint textureId,GLuint textureId2,GLuint textureId3);
    //void show_music_3d_new(int aangle,GLuint textureId);
    void show_music_3d_2(float aangle,GLuint textureId);
    int loadboxpictures();
    void createboxpictures();
    void roll_array();
};


#endif
