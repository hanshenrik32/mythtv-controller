#ifndef MYCTRL_FONT
#define MYCTRL_FONT

class fontinf {
  public:
    GLint fontnumber;
    char fontname[200];
    char fonttype[20];
};

const int FONT_TYPE_MAX=200;

class fontctrl {
  public:
    unsigned int mastercount;			     // total antal font installed
    fontinf typeinfo[FONT_TYPE_MAX];
    int updatefontlist();
    int updatefontlist_old();
    int selectfont(char *fontname);
    fontctrl();                       // constructor
};


#endif
