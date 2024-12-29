#ifndef MYCTRL_RECORDED
#define MYCTRL_RECORDED

#include <string>
#include <string.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>



class recorded_oversigt_type {		// data types from mythtv ver 0.22
    private:
        char title[128+1];				    // program title
        char subtitle[128+1];				// sub title (desc)
        char recordedpath[255+1];			// recorded file path
        char startdato[20+1];
        char enddato[20+1];
        char description[200+1];
    public:
        std::string start_prg_image;             // image string to file from recorded tv
        recorded_oversigt_type();
        void put_recorded(char *tit,char *subtit,char *startdag,char *enddag,char *filepath,char *desc);
        void get_recorded(int recnr,char *tit,char *subtit,char *startdag,char *enddag,char *desc);
        void get_recorded_filepath(char *filepath);
};

class recorded_top_type {
    private:
        char title[128+1];
    public:
        unsigned int prg_antal;
        recorded_oversigt_type recorded_programs[200+1];
        recorded_oversigt_type *recorded_programsp[];
        recorded_top_type();
        void put_recorded_top(char *title);
        void get_recorded_top(char *title);
};


class recorded_overigt {
    private:
        recorded_top_type programs[40];
        recorded_top_type **programsp;
        unsigned int title_antal;
    public:
        recorded_overigt() { title_antal=0; }
        unsigned int programs_type_antal(int nr) { return(programs[nr].prg_antal); }
        int opdatere_recorded_oversigt();
        void show_recorded_oversigt(int valgtnr,int subvalgtnr);
        unsigned int top_antal() { return(title_antal); }
        void set_top_antal(unsigned int ant) { title_antal=ant; }
        void get_recorded_filepath(char *filepath,int valgtnr,int subvalgtnr);
};




#endif
