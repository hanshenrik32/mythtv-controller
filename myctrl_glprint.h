
#include <map>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H



// ******************************************
// used to write text in opengl
// Struktur til at gemme skrifttypeinformation
typedef struct {
  unsigned int texture;
  int width;
  int height;
  int bearingX;
  int bearingY;
  int advance;
} Character;


struct mFont {
  std::map<unsigned int, Character> characters;
  int size;
};


int initFreeType(const char *fontPath);
// mFont loadFont(FT_Face face, int pixelSize);
void drawText(mFont& font, const char* text, float x, float y, float scale, int color);
void drawLinesOfText(const std::string& text, float x, float y, float scale,int maxWidth,int maxlines,int color,bool center);
// void set_color(float r,float g,float b) {};
// void set_default_color() {};

// *******************************************

