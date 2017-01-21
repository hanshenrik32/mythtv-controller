#ifndef MYCTRL_UTILITY
#define MYCTRL_UTILITY

bool file_exists(const char * filename);
int gotoxy(int x, int y);
int clearscreen();
int getuserhomedir(char *homedir);
int check_zerro_bytes_file(char *filename);

#endif
