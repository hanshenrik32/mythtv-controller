#ifndef MYCTRL_XBMC
#define MYCTRL_XBMC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <mysql.h>


#include <sqlite3.h>                    // sqlite interface to xbmc
#include "utility.h"
#include "myctrl_movie.h"
#include "myctrl_music.h"

// web file loader
#include "myctrl_readwebfile.h"

extern film_oversigt_typem film_oversigt;
extern musicoversigt_class musicoversigt;
extern int configxbmcver;
extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];


#define MS_ABSOLUTE    0x01

#define BTN_USE_NAME   0x01
#define BTN_DOWN       0x02
#define BTN_UP         0x04
#define BTN_USE_AMOUNT 0x08
#define BTN_QUEUE      0x10
#define BTN_NO_REPEAT  0x20
#define BTN_VKEY       0x40
#define BTN_AXIS       0x80

#define PT_HELO         0x01
#define PT_BYE          0x02
#define PT_BUTTON       0x03
#define PT_MOUSE        0x04
#define PT_PING         0x05
#define PT_BROADCAST    0x06
#define PT_NOTIFICATION 0x07
#define PT_BLOB         0x08
#define PT_LOG          0x09
#define PT_ACTION       0x0A
#define PT_DEBUG        0xFF

#define ICON_NONE       0x00
#define ICON_JPEG       0x01
#define ICON_PNG        0x02
#define ICON_GIF        0x03

#define XBMC_STD_PORT 9777
#define MAX_PACKET_SIZE  1024
#define HEADER_SIZE      32
#define MAX_PAYLOAD_SIZE (MAX_PACKET_SIZE - HEADER_SIZE)

#define ACTION_EXECBUILTIN 0x01
#define ACTION_BUTTON      0x02

#define MAJOR_VERSION 2
#define MINOR_VERSION 0


class xbmc_adress {
  private:
   struct sockaddr_in addr;
  public:
   // constructor
   xbmc_adress(int Port = XBMC_STD_PORT) {
      addr.sin_family = AF_INET;
      addr.sin_port = htons(Port);
      addr.sin_addr.s_addr = INADDR_ANY;
      memset(addr.sin_zero, '\0', sizeof addr.sin_zero);
   }
   // controuctor
   xbmc_adress(const char *address,int Port = XBMC_STD_PORT) {
     addr.sin_port = htons(Port);
     struct hostent *h;
     if (address == NULL || (h=gethostbyname(address)) == NULL) {
        if (address) printf("Error get host by name\n");
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_family = AF_INET;
     } else {
        addr.sin_family = h->h_addrtype;
        addr.sin_addr = *((struct in_addr *)h->h_addr);
     }
     memset(addr.sin_zero, '\0', sizeof addr.sin_zero);
   }

   void setxbmc_port(int port) {
     addr.sin_port = htons(port);
   }

   const sockaddr *GetAddress() {
     return((struct sockaddr *) &addr);
   }

   bool Bind(int sockfd) {
      return (bind(sockfd, (struct sockaddr *)&addr, sizeof addr) == 0);
   }

};


class XBMCClientUtils {
public:
  XBMCClientUtils() {}
  ~XBMCClientUtils() {}
  static unsigned int GetUniqueIdentifier()
  {
    static time_t id = time(NULL);
    return id;
  }

  static void Clean()
  {
  #ifdef _WIN32
    WSACleanup();
  #endif
  }

  static bool Initialize()
  {
  #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData))
      return false;
  #endif
    return true;
  }
};






class CPacket {
/*   Base class that implements a single event packet.

     - Generic packet structure (maximum 1024 bytes per packet)
     - Header is 32 bytes long, so 992 bytes available for payload
     - large payloads can be split into multiple packets using H4 and H5
       H5 should contain total no. of packets in such a case
     - H6 contains length of P1, which is limited to 992 bytes
     - if H5 is 0 or 1, then H4 will be ignored (single packet msg)
     - H7 must be set to zeros for now

         -----------------------------
         | -H1 Signature ("XBMC")    | - 4  x CHAR                4B
         | -H2 Version (eg. 2.0)     | - 2  x UNSIGNED CHAR       2B
         | -H3 PacketType            | - 1  x UNSIGNED SHORT      2B
         | -H4 Sequence number       | - 1  x UNSIGNED LONG       4B
         | -H5 No. of packets in msg | - 1  x UNSIGNED LONG       4B
         | -H6 Payload size          | - 1  x UNSIGNED SHORT      2B
         | -H7 Client's unique token | - 1  x UNSIGNED LONG       4B
         | -H8 Reserved              | - 10 x UNSIGNED CHAR      10B
         |---------------------------|
         | -P1 payload               | -
         -----------------------------
*/
public:
  CPacket()
  {
    m_PacketType = 0;
  }
  virtual ~CPacket()
  { }

  bool Send(int Socket, xbmc_adress &Addr, unsigned int UID = XBMCClientUtils::GetUniqueIdentifier())
  {
    if (m_Payload.size() == 0)
      ConstructPayload();
    bool SendSuccessfull = true;
    int NbrOfPackages = (m_Payload.size() / MAX_PAYLOAD_SIZE) + 1;
    int Send = 0;
    int Sent = 0;
    int Left = m_Payload.size();
    for (int Package = 1; Package <= NbrOfPackages; Package++)
    {
      if (Left > MAX_PAYLOAD_SIZE)
      {
        Send = MAX_PAYLOAD_SIZE;
        Left -= Send;
      }
      else
      {
        Send = Left;
        Left = 0;
      }

      ConstructHeader(m_PacketType, NbrOfPackages, Package, Send, UID, m_Header);
      char t[MAX_PACKET_SIZE];
      int i, j;
      for (i = 0; i < 32; i++)
        t[i] = m_Header[i];

      for (j = 0; j < Send; j++)
        t[(32 + j)] = m_Payload[j + Sent];

      int rtn = sendto(Socket, t, (32 + Send), 0, Addr.GetAddress(), sizeof(struct sockaddr));

      if (rtn != (32 + Send))
        SendSuccessfull = false;

      Sent += Send;
    }
    return SendSuccessfull;
  }
protected:
  char            m_Header[HEADER_SIZE];
  unsigned short  m_PacketType;

  std::vector<char> m_Payload;

  static void ConstructHeader(int PacketType, int NumberOfPackets, int CurrentPacket, unsigned short PayloadSize, unsigned int UniqueToken, char *Header)
  {
    sprintf(Header, "XBMC");
    for (int i = 4; i < HEADER_SIZE; i++)
      Header[i] = 0;
    Header[4]  = MAJOR_VERSION;
    Header[5]  = MINOR_VERSION;
    if (CurrentPacket == 1)
    {
      Header[6]  = ((PacketType & 0xff00) >> 8);
      Header[7]  =  (PacketType & 0x00ff);
    }
    else
    {
      Header[6]  = ((PT_BLOB & 0xff00) >> 8);
      Header[7]  =  (PT_BLOB & 0x00ff);
    }
    Header[8]  = ((CurrentPacket & 0xff000000) >> 24);
    Header[9]  = ((CurrentPacket & 0x00ff0000) >> 16);
    Header[10] = ((CurrentPacket & 0x0000ff00) >> 8);
    Header[11] =  (CurrentPacket & 0x000000ff);

    Header[12] = ((NumberOfPackets & 0xff000000) >> 24);
    Header[13] = ((NumberOfPackets & 0x00ff0000) >> 16);
    Header[14] = ((NumberOfPackets & 0x0000ff00) >> 8);
    Header[15] =  (NumberOfPackets & 0x000000ff);

    Header[16] = ((PayloadSize & 0xff00) >> 8);
    Header[17] =  (PayloadSize & 0x00ff);

    Header[18] = ((UniqueToken & 0xff000000) >> 24);
    Header[19] = ((UniqueToken & 0x00ff0000) >> 16);
    Header[20] = ((UniqueToken & 0x0000ff00) >> 8);
    Header[21] =  (UniqueToken & 0x000000ff);
  }

  virtual void ConstructPayload()
  { }
};

class CPacketHELO : public CPacket
{
    /************************************************************************/
    /* Payload format                                                       */
    /* %s -  device name (max 128 chars)                                    */
    /* %c -  icontype ( 0=>NOICON, 1=>JPEG , 2=>PNG , 3=>GIF )              */
    /* %s -  my port ( 0=>not listening )                                   */
    /* %d -  reserved1 ( 0 )                                                */
    /* %d -  reserved2 ( 0 )                                                */
    /* XX -  imagedata ( can span multiple packets )                        */
    /************************************************************************/
private:
  std::vector<char> m_DeviceName;
  unsigned short m_IconType;
  char *m_IconData;
  unsigned short m_IconSize;
public:
  virtual void ConstructPayload()
  {
    m_Payload.clear();

    for (unsigned int i = 0; i < m_DeviceName.size(); i++)
      m_Payload.push_back(m_DeviceName[i]);

    m_Payload.push_back('\0');

    m_Payload.push_back(m_IconType);

    m_Payload.push_back(0);
    m_Payload.push_back('\0');

    for (int j = 0; j < 8; j++)
      m_Payload.push_back(0);

    for (int ico = 0; ico < m_IconSize; ico++)
      m_Payload.push_back(m_IconData[ico]);
  }

  CPacketHELO(const char *DevName, unsigned short IconType, const char *IconFile = NULL) : CPacket()
  {
    m_PacketType = 0;				// m_PacketType = PT_HELO;

    unsigned int len = strlen(DevName);
    for (unsigned int i = 0; i < len; i++)
      m_DeviceName.push_back(DevName[i]);

    m_IconType = IconType;

    if (IconType == ICON_NONE || IconFile == NULL)
    {
      m_IconData = NULL;
      m_IconSize = 0;
      return;
    }

    std::ifstream::pos_type size;

    std::ifstream file (IconFile, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
      size = file.tellg();
      m_IconData = new char [size];
      file.seekg (0, std::ios::beg);
      file.read (m_IconData, size);
      file.close();
      m_IconSize = size;
    }
    else
    {
      m_IconType = ICON_NONE;
      m_IconSize = 0;
    }
  }

  virtual ~CPacketHELO()
  {
    m_DeviceName.clear();
    if (m_IconData)
      free(m_IconData);
  }
};





class CPacketNOTIFICATION : public CPacket
{
    /************************************************************************/
    /* Payload format:                                                      */
    /* %s - caption                                                         */
    /* %s - message                                                         */
    /* %c - icontype ( 0=>NOICON, 1=>JPEG , 2=>PNG , 3=>GIF )               */
    /* %d - reserved ( 0 )                                                  */
    /* XX - imagedata ( can span multiple packets )                         */
    /************************************************************************/
private:
  std::vector<char> m_Title;
  std::vector<char> m_Message;
  unsigned short m_IconType;
  char *m_IconData;
  unsigned short m_IconSize;
public:
  virtual void ConstructPayload()
  {
    m_Payload.clear();

    for (unsigned int i = 0; i < m_Title.size(); i++) {
      m_Payload.push_back(m_Title[i]);
    }

    m_Payload.push_back('\0');

    for (unsigned int i = 0; i < m_Message.size(); i++) {
     m_Payload.push_back(m_Message[i]);
    }

    m_Payload.push_back('\0');

    m_Payload.push_back(m_IconType);

    for (int i = 0; i < 4; i++) {
      m_Payload.push_back(0);
    }

    for (int ico = 0; ico < m_IconSize; ico++) {
      m_Payload.push_back(m_IconData[ico]);
    }
  }

  CPacketNOTIFICATION(const char *Title, const char *Message, unsigned short IconType, const char *IconFile = NULL) :  CPacket()
  {
    //m_PacketType = 0;    //m_PacketType = PT_NOTIFICATION;
    m_IconData = NULL;
    m_IconSize = 0;
    unsigned int len = 0;
    if (Title != NULL)
    {
      len = strlen(Title);
      for (unsigned int i = 0; i < len; i++)
        m_Title.push_back(Title[i]);
    }

    if (Message != NULL)
    {
      len = strlen(Message);
      for (unsigned int i = 0; i < len; i++)
        m_Message.push_back(Message[i]);
    }
    m_IconType = IconType;

    if (IconType == ICON_NONE || IconFile == NULL)
      return;

    std::ifstream::pos_type size;

    std::ifstream file (IconFile, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
      size = file.tellg();
      m_IconData = new char [size];
      file.seekg (0, std::ios::beg);
      file.read (m_IconData, size);
      file.close();
      m_IconSize = size;
    }
    else
      m_IconType = ICON_NONE;
  }

  virtual ~CPacketNOTIFICATION()
  {
    m_Title.clear();
    m_Message.clear();
    if (m_IconData)
      free(m_IconData);
  }
};



class CPacketBUTTON : public CPacket
{
    /************************************************************************/
    /* Payload format                                                       */
    /* %i - button code                                                     */
    /* %i - flags 0x01 => use button map/name instead of code               */
    /*            0x02 => btn down                                          */
    /*            0x04 => btn up                                            */
    /*            0x08 => use amount                                        */
    /*            0x10 => queue event                                       */
    /*            0x20 => do not repeat                                     */
    /*            0x40 => virtual key                                       */
    /*            0x40 => axis key                                          */
    /* %i - amount ( 0 => 65k maps to -1 => 1 )                             */
    /* %s - device map (case sensitive and required if flags & 0x01)        */
    /*      "KB" - Standard keyboard map                                    */
    /*      "XG" - Xbox Gamepad                                             */
    /*      "R1" - Xbox Remote                                              */
    /*      "R2" - Xbox Universal Remote                                    */
    /*      "LI:devicename" -  valid LIRC device map where 'devicename'     */
    /*                         is the actual name of the LIRC device        */
    /*      "JS<num>:joyname" -  valid Joystick device map where            */
    /*                           'joyname'  is the name specified in        */
    /*                           the keymap. JS only supports button code   */
    /*                           and not button name currently (!0x01).     */
    /* %s - button name (required if flags & 0x01)                          */
    /************************************************************************/
private:
  std::vector<char> m_DeviceMap;
  std::vector<char> m_Button;
  unsigned short m_ButtonCode;
  unsigned short m_Amount;
  unsigned short m_Flags;
public:
  virtual void ConstructPayload()
  {
    m_Payload.clear();

    if (m_Button.size() != 0)
    {
      if (!(m_Flags & BTN_USE_NAME)) // If the BTN_USE_NAME isn't flagged for some reason
        m_Flags |= BTN_USE_NAME;
      m_ButtonCode = 0;
    }
    else
      m_Button.clear();

    if (m_Amount > 0)
    {
      if (!(m_Flags & BTN_USE_AMOUNT))
        m_Flags |= BTN_USE_AMOUNT;
    }
    if (!((m_Flags & BTN_DOWN) || (m_Flags & BTN_UP))) //If none of them are tagged.
      m_Flags |= BTN_DOWN;

    m_Payload.push_back(((m_ButtonCode & 0xff00) >> 8));
    m_Payload.push_back( (m_ButtonCode & 0x00ff));

    m_Payload.push_back(((m_Flags & 0xff00) >> 8) );
    m_Payload.push_back( (m_Flags & 0x00ff));

    m_Payload.push_back(((m_Amount & 0xff00) >> 8) );
    m_Payload.push_back( (m_Amount & 0x00ff));


    for (unsigned int i = 0; i < m_DeviceMap.size(); i++)
      m_Payload.push_back(m_DeviceMap[i]);

    m_Payload.push_back('\0');

    for (unsigned int i = 0; i < m_Button.size(); i++)
      m_Payload.push_back(m_Button[i]);

    m_Payload.push_back('\0');
  }

  CPacketBUTTON(const char *Button, const char *DeviceMap, unsigned short Flags, unsigned short Amount = 0) : CPacket()
  {
    m_PacketType = PT_BUTTON;
    m_Flags      = Flags;
    m_ButtonCode = 0;
    m_Amount     = Amount;

    unsigned int len = strlen(DeviceMap);
    for (unsigned int i = 0; i < len; i++)
      m_DeviceMap.push_back(DeviceMap[i]);

    len = strlen(Button);
    for (unsigned int i = 0; i < len; i++)
      m_Button.push_back(Button[i]);
  }

  CPacketBUTTON(unsigned short ButtonCode, const char *DeviceMap, unsigned short Flags, unsigned short Amount = 0) : CPacket()
  {
    m_PacketType = PT_BUTTON;
    m_Flags      = Flags;
    m_ButtonCode = ButtonCode;
    m_Amount     = Amount;

    unsigned int len = strlen(DeviceMap);
    for (unsigned int i = 0; i < len; i++)
      m_DeviceMap.push_back(DeviceMap[i]);
  }

  CPacketBUTTON(unsigned short ButtonCode, unsigned short Flags, unsigned short Amount = 0) : CPacket()
  {
    m_PacketType = PT_BUTTON;
    m_Flags      = Flags;
    m_ButtonCode = ButtonCode;
    m_Amount     = Amount;
  }

  // Used to send a release event
  CPacketBUTTON() : CPacket()
  {
    m_PacketType = PT_BUTTON;
    m_Flags = BTN_UP;
    m_Amount = 0;
    m_ButtonCode = 0;
  }

  virtual ~CPacketBUTTON()
  {
    m_DeviceMap.clear();
    m_Button.clear();
  }

  inline unsigned short GetFlags() { return m_Flags; }
  inline unsigned short GetButtonCode() { return m_ButtonCode; }
};






class CPacketPING : public CPacket
{
    /************************************************************************/
    /* no payload                                                           */
    /************************************************************************/
public:
  CPacketPING() : CPacket()
  {
    m_PacketType = PT_PING;
  }
  virtual ~CPacketPING()
  { }
};







class CPacketBYE : public CPacket
{
    /************************************************************************/
    /* no payload                                                           */
    /************************************************************************/
public:
  CPacketBYE() : CPacket()
  {
    m_PacketType = PT_BYE;
  }
  virtual ~CPacketBYE()
  { }
};







class CPacketMOUSE : public CPacket
{
    /************************************************************************/
    /* Payload format                                                       */
    /* %c - flags                                                           */
    /*    - 0x01 absolute position                                          */
    /* %i - mousex (0-65535 => maps to screen width)                        */
    /* %i - mousey (0-65535 => maps to screen height)                       */
    /************************************************************************/
private:
  unsigned short m_X;
  unsigned short m_Y;
  unsigned char  m_Flag;
public:
  CPacketMOUSE(int X, int Y, unsigned char Flag = MS_ABSOLUTE)
  {
    m_PacketType = PT_MOUSE;
    m_Flag = Flag;
    m_X = X;
    m_Y = Y;
  }

  virtual void ConstructPayload()
  {
    m_Payload.clear();

    m_Payload.push_back(m_Flag);

    m_Payload.push_back(((m_X & 0xff00) >> 8));
    m_Payload.push_back( (m_X & 0x00ff));

    m_Payload.push_back(((m_Y & 0xff00) >> 8));
    m_Payload.push_back( (m_Y & 0x00ff));
  }

  virtual ~CPacketMOUSE()
  { }
};







class CPacketLOG : public CPacket
{
    /************************************************************************/
    /* Payload format                                                       */
    /* %c - log type                                                        */
    /* %s - message                                                         */
    /************************************************************************/
private:
  std::vector<char> m_Message;
  unsigned char  m_LogLevel;
  bool m_AutoPrintf;
public:
  CPacketLOG(int LogLevel, const char *Message, bool AutoPrintf = true)
  {
    m_PacketType = PT_LOG;

    unsigned int len = strlen(Message);
    for (unsigned int i = 0; i < len; i++)
      m_Message.push_back(Message[i]);

    m_LogLevel = LogLevel;
    m_AutoPrintf = AutoPrintf;
  }

  virtual void ConstructPayload()
  {
    m_Payload.clear();

    m_Payload.push_back( (m_LogLevel & 0x00ff) );

    if (m_AutoPrintf)
    {
      char* str=&m_Message[0];
      printf("%s\n", str);
    }
    for (unsigned int i = 0; i < m_Message.size(); i++)
      m_Payload.push_back(m_Message[i]);

    m_Payload.push_back('\0');
  }

  virtual ~CPacketLOG()
  { }
};





class CPacketACTION : public CPacket
{
    /************************************************************************/
    /* Payload format                                                       */
    /* %c - action type                                                     */
    /* %s - action message                                                  */
    /************************************************************************/
private:
  unsigned char     m_ActionType;
  std::vector<char> m_Action;
public:
  CPacketACTION(const char *Action, unsigned char ActionType = ACTION_EXECBUILTIN)
  {
    m_PacketType = PT_ACTION;

    m_ActionType = ActionType;
    unsigned int len = strlen(Action);
    for (unsigned int i = 0; i < len; i++)
      m_Action.push_back(Action[i]);
  }

  virtual void ConstructPayload()
  {
    m_Payload.clear();

    m_Payload.push_back(m_ActionType);
    for (unsigned int i = 0; i < m_Action.size(); i++)
      m_Payload.push_back(m_Action[i]);

    m_Payload.push_back('\0');
  }

  virtual ~CPacketACTION()
  { }
};



// xbmc conector class

class CXBMCClient {
private:
  xbmc_adress   m_Addr;
  int           m_Socket;
  unsigned int  m_UID;
public:
  CXBMCClient(const char *IP = "127.0.0.1", int Port = 9777, int Socket = -1, unsigned int UID = 0) {
    m_Addr = xbmc_adress(IP, Port);
    if (Socket == -1)
      m_Socket = socket(AF_INET, SOCK_DGRAM, 0);
    else
      m_Socket = Socket;

    if (UID)
      m_UID = UID;
    else
      m_UID = XBMCClientUtils::GetUniqueIdentifier();
  }

  void SendNOTIFICATION(const char *Title, const char *Message, unsigned short IconType, const char *IconFile = NULL)
  {
    if (m_Socket < 0)
      return;

    CPacketNOTIFICATION notification(Title, Message, IconType, IconFile);
    notification.Send(m_Socket, m_Addr, m_UID);
  }

  void SendHELO(const char *DevName, unsigned short IconType, const char *IconFile = NULL)
  {
    if (m_Socket < 0)
      return;

    CPacketHELO helo(DevName, IconType, IconFile);
    helo.Send(m_Socket, m_Addr, m_UID);
  }

  void SendButton(const char *Button, const char *DeviceMap, unsigned short Flags, unsigned short Amount = 0)
  {
    if (m_Socket < 0)
      return;

    CPacketBUTTON button(Button, DeviceMap, Flags, Amount);
    button.Send(m_Socket, m_Addr, m_UID);
  }

  void SendButton(unsigned short ButtonCode, const char *DeviceMap, unsigned short Flags, unsigned short Amount = 0)
  {
    if (m_Socket < 0)
      return;

    CPacketBUTTON button(ButtonCode, DeviceMap, Flags, Amount);
    button.Send(m_Socket, m_Addr, m_UID);
  }

  void SendButton(unsigned short ButtonCode, unsigned Flags, unsigned short Amount = 0)
  {
    if (m_Socket < 0)
      return;

    CPacketBUTTON button(ButtonCode, Flags, Amount);
    button.Send(m_Socket, m_Addr, m_UID);
  }

  void SendMOUSE(int X, int Y, unsigned char Flag = MS_ABSOLUTE)
  {
    if (m_Socket < 0)
      return;

    CPacketMOUSE mouse(X, Y, Flag);
    mouse.Send(m_Socket, m_Addr, m_UID);
  }

  void SendLOG(int LogLevel, const char *Message, bool AutoPrintf = true)
  {
    if (m_Socket < 0)
      return;

    CPacketLOG log(LogLevel, Message, AutoPrintf);
    log.Send(m_Socket, m_Addr, m_UID);
  }

  void SendACTION(const char *ActionMessage, int ActionType = ACTION_EXECBUILTIN)
  {
    if (m_Socket < 0)
      return;

    CPacketACTION action(ActionMessage, ActionType);
    action.Send(m_Socket, m_Addr, m_UID);
  }
};

//
// ************************ sqllite access to xbmc *********************************
//

class xbmcsqlite {
private:
  int sqlerr;
  sqlite3 *sqlitedb_obj;
  char dbfilename[256];
  char dbmusicname[256];
  char dbmoviename[256];
  char dbmovienpath[256];
  char dbmusicpath[256];
  int filmantal;
  int musicantal;

  static int xbmc_load_sqldb_callback_movie(void *data, int argc, char **argv, char **azColName);

  // callback to fill music db in mythtv-controller
  static int xbmc_load_sqldb_callback_music(void *data, int argc, char **argv, char **azColName);

  // callback to fill version db from
  static int xbmc_load_sqldb_callback_version(void *data, int argc, char **argv, char **azColName) {
    configxbmcver=75;
    printf("XBMC - version loader \n");
  }


public:
  //
  // open default music database (xbmc database names = MyMusic18.db MyVideos60.db Addons15.db
  //
  int opensqldb(char *dbname) {
    if (sqlitedb_obj) closesqldb();
    if (sqlite3_open(dbname,&sqlitedb_obj)!=SQLITE_OK) {
      fprintf(stderr, "XBMC - open SQL error: \n");
      sqlitedb_obj=0;
    }
  }

  int closesqldb() {
    return(sqlite3_close(sqlitedb_obj));
    sqlitedb_obj=NULL;
  }
  //
  // constructor
  //
  // old ver xbmcsqlite(const char *IP = "127.0.0.1",const char *dbname="/home/hans/.kodi/userdata/Database/MyVideos99.db",const char *musicname="/home/hans/.kodi/userdata/Database/MyMusic56.db",const char *moviename="/home/hans/.kodi/userdata/Database/MyVideos99.db") {
  xbmcsqlite(const char *IP = "127.0.0.1",const char *dbname="/home/hans/.kodi/userdata/Database/MyVideos107.db",const char *musicname="/home/hans/.kodi/userdata/Database/MyMusic60.db",const char *moviename="/home/hans/.kodi/userdata/Database/MyVideos107.db") {
    filmantal=0;
    musicantal=0;
    strcpy(dbfilename,dbname);  				// gem fil navn i object
    strcpy(dbmusicname,musicname);
    strcpy(dbmoviename,moviename);
  }
  // xbmc music db load
  void xbmc_readmusicdb();
  // xbmc movie db load
  void xbmc_readmoviedb();
  int xbmcloadversion();
  int getxmlfilepath();
};


#endif
