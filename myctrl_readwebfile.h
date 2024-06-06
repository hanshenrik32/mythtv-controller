#ifndef MYCTRL_WEBFILE
#define MYCTRL_WEBFILE

int get_webfilename(char *fname,char *webpath);             // return string for filename (fname) or null
int get_webfilenamelong(char *fname,char *webpath);         // return string (fname) for filename long = last path + name as filename
int get_webfile(char *webpath,char *outfile);
int get_webfile2(char *webpath,char *outfile);              // used to download images from web
bool check_filename_ext(const char *filename);

#endif
