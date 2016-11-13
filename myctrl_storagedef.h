#ifndef MYCTRL_STORAGE
#define MYCTRL_STORAGE

//
// mydef of Storage group def in mythtv 0.24
// Used by main and myctrl_recorded
//

const int storagegroupantal=10;

struct storagedef {
    char path[256];
    char name[128];
};


#endif
