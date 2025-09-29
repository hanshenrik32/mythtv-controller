#ifndef MYCTRL_UTILITY
#define MYCTRL_UTILITY

bool file_exists(const char * filename);
int gotoxy(int x, int y);
int clearscreen();
int getuserhomedir(char *homedir);
int check_zerro_bytes_file(char *filename);
unsigned int hourtounixtime(int hour);

int write_logfile(FILE *fhandler,char *logentry);

bool directory_empty(char *path);
char *b64_encode(const unsigned char *in, size_t len);

unsigned int countEntriesInDir(const char* dirname);

#endif
