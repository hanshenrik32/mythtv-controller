#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <netdb.h>
#include <ifaddrs.h>
#include "checknet.h"

extern int debugmode;


extern char confighostwlanname[256];

// name = interface name
// ip the ip in long

/*
static int set_ip_using(const char *name, int c, unsigned long ip) {
      struct ifreq ifr;
      struct sockaddr_in sin;
      safe_strncpy(ifr.ifr_name, name, IFNAMSIZ);
      memset(&sin, 0, sizeof(struct sockaddr));
      sin.sin_family = AF_INET;
      sin.sin_addr.s_addr = ip;
      memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
      if (ioctl(skfd, c, &ifr) < 0) return -1;
      return 0;
}
*/


wifinetid::wifinetid() {
      strcpy(hadd,"");    // hardware adress
      strcpy(essid,"");   // network id
      strcpy(pass,"");	  // password to network
      signal=0;
      encrypt=0;
}

// save wifinet info

void wifinetid::put_wifiid(char *hardware,char *id,int sign,int crypt) {
      strcpy(hadd,hardware);
      strcpy(essid,id);
      signal=sign;
      encrypt=crypt;
}

// get wifinet info

void wifinetid::get_wifiid(char *hardware,char *id,int *sign,int *crypt) {
      strcpy(hardware,hadd);
      strcpy(id,essid);
      *sign=signal;
      *crypt=encrypt;
}

void wifinetid::put_pass(char *p) {
      strcpy(pass,p);
}


void wifinetid::get_pass(char *p) {
      strcpy(p,pass);
}



// constructor

wifinetdef::wifinetdef() {
      wifinetantal=0;
      strcpy(interfacename,"");
      create_wifilist();			// create list of wifinets in range
      printf("Wifinet found %d \n",wifinetantal);
}


// update wifi net list
void wifinetdef::update_wifi_netlist() {
      wifinetantal=0;
      create_wifilist();			// create list of wifinets in range
}


// find wifi network interface name

void wifinetdef::find_interfacenames() {
    struct ifaddrs *ifaddr, *ifa;
    int family, s, n;
    char host[NI_MAXHOST];
    bool interfacefound=false;
    if (getifaddrs(&ifaddr) == -1) {
      perror("getifaddrs");
      exit(EXIT_FAILURE);
    }
    // loop all interfaces found by getifaddrs call
    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
      if (ifa->ifa_addr == NULL)
        continue;
      family = ifa->ifa_addr->sa_family;
      if ((debugmode & 1) && ((family==AF_INET) || (family==AF_INET6))) printf("interface %-12s ",ifa->ifa_name);
      // For an AF_INET* interface address, display the address
        if (family == AF_INET || family == AF_INET6) {
        s = getnameinfo(ifa->ifa_addr,(family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),host, NI_MAXHOST,NULL, 0, NI_NUMERICHOST);
        if (s != 0) {
          printf("getnameinfo() failed: %s\n", gai_strerror(s));
          exit(EXIT_FAILURE);
        }
        if ((interfacefound==false) && (strncmp(ifa->ifa_name,"wl",2)==0)) {
            interfacefound=true;
            strcpy(interfacename,ifa->ifa_name);                                // save in config
            strcpy(interfaceaddress,host);                                      // save ipadress in config
        }
        if (debugmode & 1) printf("address: <%s>\n", host);
      }
    }
    if (ifaddr) freeifaddrs(ifaddr);
}



// load wifi net list from OS

int wifinetdef::create_wifilist() {
      FILE *fp=NULL;
      char *sted;				// bruges til string search
      //int status;
      int maxlength=20000;
      char result[20000];
      char tmptxt[80];
      char hadd[18];                  // hardware adress
      char essid[80];                 // network id
      char foundinterfacename[200];
      int signal=0;
      int encrypt=0;
      int finish=0;
      if (debugmode & 1) fprintf(stderr,"Scanning for wifi networks.\n");
      find_interfacenames();                      // get interface names and select wifi networks
      strcpy(foundinterfacename,"/sbin/iwlist ");
      strcat(foundinterfacename,this->interfacename);       // add found interface name found by find_interfacenames
      strcat(foundinterfacename," scan");
      fp=popen(foundinterfacename,"r");
      sleep(3);                                           // wait for scan
      if (fp) pclose(fp);
      fp=popen(foundinterfacename,"r");
      if (fp) {
            while(fgets(result,maxlength-1,fp)!=NULL) {
                  if ((sted=strstr(result,"Address:"))) {
                        strcpy(hadd,sted+9);
                        hadd[17]=0;
                  }
                  if ((sted=strstr(result,"ESSID:"))) {
                        strcpy(essid,sted+6+1);
                        int l=strlen(essid);
                        if (l>2) essid[l-2]=0;
                        if (l==2) strcpy(essid,"Hidden");
                  }
                  if ((sted=strstr(result,"Encryption key"))) {
                        if (strncmp(sted+15,"off",3)==0) encrypt=0; else encrypt=1;
                  }
                  if ((sted=strstr(result,"level="))) {
                        strcpy(tmptxt,sted+6);
                        if (strlen(tmptxt)>3) tmptxt[strlen(tmptxt)-3]=0;
                        signal=atoi(tmptxt);
                  }
                  if ((sted=strstr(result,"Mode"))) finish=1; else finish=0;
                  if (finish) {
                        if (wifinetantal==0) strcpy(confighostwlanname,essid);		// det default essid
                        // save net info array
                        wifilist[wifinetantal].put_wifiid(hadd,essid,signal,encrypt);
                        wifinetantal++;
                        finish=0;
                  }
            }
      } else if (debugmode & 1) fprintf(stderr,"ERROR loading wifi network list by missing command /sbin/iwlist\n");
      if (fp) pclose(fp);
      return(wifinetantal);
}


// get aktiv essid from iwconfig

int wifinetdef::findaktiv_wifi(char *aktivhardwareadress) {
      FILE *fp;
      char *sted;				// bruges til string search
//      int status;
      int maxlength=10000;
      char result[10000];
      bool finish=false;
      char hard[18];
      fp=popen("iwconfig","r");
      if (fp) {
            while(fgets(result,maxlength-1,fp)!=NULL) {
                  if ((sted=strstr(result,"Access Point:"))) {
                        strcpy(aktivhardwareadress,sted+14);
                        finish=true;
                  }
            }
      } else if (debugmode & 1) fprintf(stderr,"Warning no wifi interface found,\n");
      if (fp) {
            pclose(fp);
            if (finish) {
                  for(int i=0;i<wifinetantal;i++) {
                        if (get_networkhardwareadress(i,hard)) return(i);
                  }
            }
      }
      return(0);
}


int wifinetdef::get_encryptstatus(int nr) {
      int crypt;
      if (nr<=wifinetantal) crypt=wifilist[nr].get_encrypt();
      return(crypt);
}


int wifinetdef::get_networkid(int nr,char *id) {
      char hardware[18];
      int sign;
      int crypt;
      if (nr<=wifinetantal)
            wifilist[nr].get_wifiid(hardware,id,&sign,&crypt);
      return(1);
}

int wifinetdef::get_networkhardwareadress(int nr,char *hardware) {
      int sign;
      int crypt;
      char id[80];
      if (nr<=wifinetantal)
            wifilist[nr].get_wifiid(hardware,id,&sign,&crypt);
      return(1);
}


int wifinetdef::get_networksign(int nr,int *si) {
      char hardware[18];
      char id[80];
      int sign;
      int crypt;
      if (nr<=wifinetantal) {
            wifilist[nr].get_wifiid(hardware,id,&sign,&crypt);
            *si=sign;
      } else *si=-1;
      return(1);
}

void wifinetdef::get_wifipass(int n,char *p) {
      if (n<=wifinetantal) wifilist[n].get_pass(p);
}


void wifinetdef::put_wifipass(int n,char *p) {
      if (n<=wifinetantal) wifilist[n].put_pass(p);
}
