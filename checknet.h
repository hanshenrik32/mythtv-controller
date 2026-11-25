

class wifinetid {
  private:
    char hadd[18];	// hardware adress
    char essid[80];	// network id
    char pass[80];
    int encrypt;
  public:
    std::string ssid;
    int signal;             // 0-100
    std::string security;
    wifinetid();
    void put_wifiid(char *hardware,char *id,int sign,int crypt);
    void get_wifiid(char *hardware,char *id,int *sign,int *crypt);
    void put_pass(char *p);
    void get_pass(char *p);
    int get_encrypt() { return(encrypt); }
};


class wifinetdef {
  private:
    int wifinetantal;
    char interfacename[200];
    char interfaceaddress[200];
    void find_interfacenames();
  public:
    std::string active_wifinetwork_name;
    std::vector<wifinetid> networks;
    wifinetdef();
    void update_wifi_netlist();
    int get_networkhardwareadress(int nr,char *hardwadress);
    int get_networkid(int nr,char *id);
    int get_networksign(int nr,int *si);
    int get_encryptstatus(int nr);
    int antal() { return wifinetantal; }
    void get_wifipass(int n,char *p);
    void put_wifipass(int n,char *p);
    bool scanWifiNetworks();
};


