#ifndef MYCTRL_FONT
#define MYCTRL_FONT

class fontinf {
  public:
    GLint fontnumber;
    char fontname[200];
    char fonttype[20];
};

const int TYPE_MAX=100;

class fontctrl {
  public:
    unsigned int mastercount;			// total antal font installed
    fontinf typeinfo[TYPE_MAX];
    int updatefontlist();
    int selectfont(char *fontname);
    fontctrl();
};


#endif

