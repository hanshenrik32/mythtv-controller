


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

int initFreeType(const char *fontPath);
void drawText(const char *text, float x, float y, float scale,int color);
void drawLinesOfText(const std::string& text, float x, float y, float scale,int maxWidth,int maxlines,int color,bool center);
// void set_color(float r,float g,float b) {};
// void set_default_color() {};

// *******************************************

