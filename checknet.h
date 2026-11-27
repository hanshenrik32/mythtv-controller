

class wifinetid {
  private:
    char hadd[18];	// hardware adress
    char essid[80];	// network id
    char pass[80];
    int encrypt;
  public:
    std::string ssid;
    std::string bssid;
    std::string rate;
    std::string ssid_password;   // wifi
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
    std::string connect_to_wifinetwork_name;
    std::vector<wifinetid> networks;
    bool wifi_ops_running;
    int wifi_ops_running_timeout;
    bool active_wifinetwork_fault;
    wifinetdef();
    void update_wifi_netlist();
    int get_networksign(int nr,int *si);
    int antal() { return wifinetantal; }
    void get_wifipass(int n,char *p);
    void put_wifipass(int n,char *p);
    bool scanWifiNetworks();
    int connectWifi(std::string ssid, std::string password);
    std::string ask_wifi_pass(std::string ssid);
    bool save_wifi_networks_list();
    bool load_wifi_networks_list();

    void connect_to_selected_wifi_thread(std::string connect_to_wifi_pass);
};

