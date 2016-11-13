

class wifinetid {
  private:
      char hadd[18];	// hardware adress
      char essid[80];	// network id
      char pass[80];
      int signal;
      int encrypt;
  public:
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
      wifinetid wifilist[20];
      int create_wifilist();
  public:
      wifinetdef();
      void update_wifi_netlist();
      int get_networkhardwareadress(int nr,char *hardwadress);
      int get_networkid(int nr,char *id);
      int get_networksign(int nr,int *si);
      int get_encryptstatus(int nr);
      int antal() { return wifinetantal; }
      void get_wifipass(int n,char *p);
      void put_wifipass(int n,char *p);
      int findaktiv_wifi(char *aktivhardwareadress);		// hent aktic mac adress
};

