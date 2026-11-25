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
#include <string>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "checknet.h"
#include "utility.h"

extern FILE *logfile;
extern char confighostwlanname[256];

// name = interface name
// ip the ip in long

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
      if (((family==AF_INET) || (family==AF_INET6))) printf("interface %-12s ",ifa->ifa_name);
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
        printf("address: <%s>\n", host);
      }
    }
    if (ifaddr) freeifaddrs(ifaddr);
}



// *****************************************************************************************
//
// get active wifi network
//
// *****************************************************************************************

std::string getActiveWifiSSID() {
  FILE* pipe = popen("nmcli --colors no -t -e no -f IN-USE,SSID device wifi list --rescan no", "r");
  if (!pipe) {
    // throw std::runtime_error("Kunne ikke starte nmcli");
  }
  std::string ssid;
  char line[1024];
  while (fgets(line, sizeof(line), pipe)) {
    if (line[0] == '*' && line[1] == ':') {
      ssid = line + 2;  // Bevarer mellemrum og kolon i navnet
        if (ssid.empty() && ssid.back() == '\n') {
          ssid.pop_back();
        }
      }
  }
  if (pclose(pipe) != 0) {
    // throw std::runtime_error("nmcli kunne ikke hente Wi-Fi-status");
  }
  return ssid;
}



// load wifi net list from OS


bool wifinetdef::scanWifiNetworks() {
    wifinetid network;
    // SSID står sidst, så et kolon i netværksnavnet bevares.
    const char* command = "nmcli --colors no --terse --escape no --fields SIGNAL,SECURITY,SSID device wifi list --rescan yes";
    FILE* pipe = popen(command, "r");
    if (!pipe) {
      // throw std::runtime_error("Kunne ikke starte Wi-Fi-scanningen");
    }
    std::vector<wifinetdef> wifinetid;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), pipe)) {
      std::string line(buffer);
      if (!line.empty() && line.back() == '\n') {
      line.pop_back();
      }

      const auto first = line.find(':');
      if (first == std::string::npos) continue;

      const auto second = line.find(':', first + 1);
      if (second == std::string::npos) continue;
      
      network.signal = std::atoi(line.substr(0, first).c_str());
      network.security = line.substr(first + 1, second - first - 1);
      network.ssid = line.substr(second + 1);
      wifinetantal++;
      networks.push_back(network);
    }
    if (pclose(pipe) != 0) {
      // throw std::runtime_error("nmcli kunne ikke gennemføre Wi-Fi-scanningen");
    }
    active_wifinetwork_name=getActiveWifiSSID();
    return true;
}



// update wifi net list

void wifinetdef::update_wifi_netlist() {
  wifinetantal=0;
  scanWifiNetworks();
  std::cout << "Active Wifi networks : " << active_wifinetwork_name << "\n";
  std::cout << "Wifi networks found." << "\n";
  for (int i=0;i<wifinetantal;i++) {
    printf("%20s   %2d   %s \n",(networks.at(i).ssid.c_str()),networks.at(i).signal,networks.at(i).security.c_str());
  }
}


int wifinetdef::get_encryptstatus(int nr) {
      int crypt;
      // if (nr<=wifinetantal) crypt=wifilist[nr].get_encrypt();
      return(crypt);
}


int wifinetdef::get_networkid(int nr,char *id) {
      char hardware[18];
      int sign;
      int crypt;
      if (nr<=wifinetantal)
            // wifilist[nr].get_wifiid(hardware,id,&sign,&crypt);
      return(1);
}

int wifinetdef::get_networkhardwareadress(int nr,char *hardware) {
      int sign;
      int crypt;
      char id[80];
      // if (nr<=wifinetantal)wifilist[nr].get_wifiid(hardware,id,&sign,&crypt);
      return(1);
}


int wifinetdef::get_networksign(int nr,int *si) {
      char hardware[18];
      char id[80];
      int sign;
      int crypt;
      if (nr<=wifinetantal) {
            // wifilist[nr].get_wifiid(hardware,id,&sign,&crypt);
            *si=sign;
      } else *si=-1;
      return(1);
}

void wifinetdef::get_wifipass(int n,char *p) {
      // if (n<=wifinetantal) wifilist[n].get_pass(p);
}


void wifinetdef::put_wifipass(int n,char *p) {
      // if (n<=wifinetantal) wifilist[n].put_pass(p);
}
