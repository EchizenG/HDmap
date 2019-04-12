#ifndef _TCPCOMMUNICATION_HPP_
#define _TCPCOMMUNICATION_HPP_

#include <sys/types.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include <mutex>

class TCPcommunication
{
public:
    static TCPcommunication* getInstance();

    ~TCPcommunication();

    struct connectInfo
    {
      char IP[16];
      int port;
      int sHandler;
      sockaddr_in sin;
    };

    struct msg_header
    {
        uint8_t  msg_type;
        uint16_t seq_nUM;
        uint16_t sid;
        uint32_t body_len;
        int64_t timestamp;
    };

    bool                 connectTCP(void);
    bool                  parseHEAD(void);
    bool                 getBODY(char *data, int size);
    struct msg_header    getHEAD(void);
    void                 setIP(char *IP);
    void                 setPORT(int port);
    bool                 sendREQ(char *head, char *data, int size);

private:
    TCPcommunication();
    
    static TCPcommunication *instance;
    
    std::mutex TCPmutex;

    struct  connectInfo conInfo;
    struct  msg_header header;
};
#endif //define _TCPCOMMUNICATION_HPP_