#ifndef MYCTRL_PICTURE
#define MYCTRL_PICTURE

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>


const int MAX_PICTURES=100;
const int MAX_PATH_LENGTH=200;


class picture_saver {
 private:
   int loaded;
   int picture_antal;					//
   GLuint screensaver_pic;				//
   char *screensaverpicturepaths[MAX_PICTURES+1];	//
   int dir_loader(char *dirpath);
 public:
   picture_saver(void);
   ~picture_saver();
   void show_pictures();
};

#endif

