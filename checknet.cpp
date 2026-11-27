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
#include <iostream>
#include <stdexcept>
#include <vector>
#include <system_error>
#include <sys/wait.h>
#include <spawn.h>

#include "checknet.h"
#include "utility.h"

extern FILE *logfile;
extern char confighostwlanname[256];
extern int do_show_setup_select_linie;


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
  wifi_ops_running=false;
  wifi_ops_running_timeout=0;  
  active_wifinetwork_fault=false;
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
    printf("nmcli not found.");
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





void wifinetdef::connect_to_selected_wifi_thread(std::string connect_to_wifi_pass) {
  wifi_ops_running=true;
  wifi_ops_running_timeout=0;
  int connected=connectWifi(connect_to_wifinetwork_name ,connect_to_wifi_pass);
  if (connected == 0) {
    // connected ok
    active_wifinetwork_name=networks.at(do_show_setup_select_linie).ssid;
    save_wifi_networks_list();
    active_wifinetwork_fault=false;
  } else {
    active_wifinetwork_name="";
    active_wifinetwork_fault=true;
  }
  wifi_ops_running_timeout=1;
  wifi_ops_running=false;
}



// *******************************************************************
// 
// Save wifi list
//
// *******************************************************************

bool wifinetdef::save_wifi_networks_list() {
  FILE *fptr;
  fptr=fopen("wifi_networks.dat","w");
  if (fptr) {
    int wifiantal=0;
    while (wifiantal<networks.size()) {
      if (networks.at(wifiantal).ssid_password.length()>0) {
        fwrite(&networks[wifiantal], sizeof(wifinetid), 1, fptr); 
      }
      wifiantal++;
    }
    fclose(fptr);
  }
  return(1);
}


// *******************************************************************
// 
// Load wifi list
//
// *******************************************************************

bool wifinetdef::load_wifi_networks_list() {
  FILE *fptr;
  wifinetid wifissids;  
  fptr=fopen("wifi_networks.dat","r");
  if (fptr) {
    fread(&wifissids, sizeof(wifinetid), 1, fptr); 
    // networks[wifiantal]
    fclose(fptr);
  }
  
}


// *****************************************************************************************************
//
// Load wifi net list from OS by nmcli command
//
// *****************************************************************************************************


bool wifinetdef::scanWifiNetworks() {
  wifinetid network;
  // SSID står sidst, så et kolon i netværksnavnet bevares.
  const char* command = "nmcli --colors no --terse --escape no --fields SIGNAL,SECURITY,SSID,RATE device wifi list --rescan yes";
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
    const auto fird = line.find(':', second + 1);
    if (fird == std::string::npos) continue;
    network.signal = std::atoi(line.substr(0, first).c_str());
    network.security = line.substr(first + 1, second - first - 1);
    network.ssid = line.substr(second + 1, fird - second - 1);
    network.rate = line.substr(fird + 1);
    wifinetantal++;
    networks.push_back(network);
  }
  if (pclose(pipe) != 0) {
    // throw std::runtime_error("nmcli kunne ikke gennemføre Wi-Fi-scanningen");
  }
  active_wifinetwork_name=getActiveWifiSSID();
  connect_to_wifinetwork_name="";

  save_wifi_networks_list();

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



// *****************************************************************************************************
//
// Connect to wifi net. 
//
// *****************************************************************************************************

static int runNmcli(std::vector<std::string> arguments) {
  std::string program = "nmcli";
  std::vector<char*> argv;
  argv.push_back(program.data());
  for (auto& argument : arguments)
      argv.push_back(argument.data());
  argv.push_back(nullptr);
  pid_t pid;
  const int error = posix_spawnp(&pid, program.c_str(), nullptr, nullptr, argv.data(), environ);
  if (error != 0) {
    printf("Kunne ikke starte nmcli: \n");
    return -1;
  }
  int status;
  while (waitpid(pid, &status, 0) == -1) {
    if (errno == EINTR)
      continue;
    std::perror("waitpid");
    return -1;
  }
  return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}


// *****************************************************************************************************
//
// Connect to wifi net (main func). 
//
// *****************************************************************************************************

int wifinetdef::connectWifi(std::string ssid, std::string password) {
    if (ssid.empty() || password.empty()) {
      std::fprintf(stderr, "Wi-Fi: SSID eller kodeord mangler\n");
      return -1;
    }
    const std::string profile = ssid;
    const std::string keyMgmt = "wpa-psk";  // Brug "sae" for ren WPA3-Personal
    int result = runNmcli({
      "connection", "modify", "id", profile,
      "wifi-sec.key-mgmt", keyMgmt,
      "wifi-sec.psk", password
    });
    if (result == 10) {
      result = runNmcli({
        "connection", "add", "type", "wifi",
        "ifname", "*",
        "con-name", profile,
        "ssid", ssid,
        "wifi-sec.key-mgmt", keyMgmt,
        "wifi-sec.psk", password
      });
    }
    if (result != 0)
      return result;
    return runNmcli({"--wait", "60", "connection", "up", "id", profile});
}


// *****************************************************************************************************
//
// Ask for pass.
//
// *****************************************************************************************************

std::string wifinetdef::ask_wifi_pass(std::string ssid) {
  char filenamepath[16381];
  std::string wifinetname="/usr/bin/zenity --modal --title=\"";
  wifinetname=wifinetname + ssid;
  wifinetname=wifinetname + "\" --entry --modal --text=\"Password\"";
  wifinetname=wifinetname + " 2> /dev/null";
  FILE *f = popen(wifinetname.c_str(), "r");
  fgets(filenamepath, 16380, f);
  if (!(f)) {
    return 0;
  }  
  fclose(f);
  return(filenamepath);
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
