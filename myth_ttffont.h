#ifndef MYCTRL_FONT
#define MYCTRL_FONT

#include <string>

class fontinf {
  public:
    GLint fontnumber;
    char fontname[200];
    std::string fontpath;
    char fonttype[20];
};

const int FONT_TYPE_MAX=2000;

class fontctrl { 
  public:
    unsigned int selected_font_nr=0;
    unsigned int mastercount;			     // total antal font installed
    fontinf typeinfo[FONT_TYPE_MAX];
    int updatefontlist();
    int updatefontlist_old();
    int selectfont(char *fontname);
    fontctrl();                       // constructor
};


#endif
