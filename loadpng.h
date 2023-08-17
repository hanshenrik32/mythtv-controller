#ifndef LOADPNG
#define LOADPNG

#include <IL/il.h>

GLuint loadpng_texture(char *filename);
GLuint save_texture_dds(ILuint image,char *filename);

#endif
