#ifndef _CANCOMMUNICATION_HPP_
#define _CANCOMMUNICATION_HPP_

#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/types.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <vector>
#include <thread>
#include <mutex>

#include "dataManager/datamanager.h"


class CANcommunication
{
public:

    static CANcommunication* getInstance();

    struct can_hdl
    {
        int fd;
        int ifindex;
        bool fd_mode;
    };

    struct can_cfg
    {
      char *ifname;
      size_t mtu;
      struct can_filter *rx_filter;
      size_t rx_filter_len;
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
    int                  parseHEAD(void);
    bool                 getBODY(char *data, int size);
    struct msg_header    getHEAD(void);
    void                 setIP(char *IP);
    void                 setPORT(int port);
    bool                 sendREQ(char *data, int size);
    bool                 stringifyCAN(std::vector<OGRPoint> vPoints);
    bool                 sendCAN(void);

    
    int     can_open(void);
    int     can_close(void);
    ssize_t can_read(void);
    ssize_t can_write(const struct can_frame *frame);
    
    ~CANcommunication();
private:
    CANcommunication();

    static CANcommunication *instance;


    int can_socket_cfg(void);
    
    std::mutex CANmutex;

    struct can_hdl *hdl;
    struct can_frame *frame;
    struct can_cfg cfg;
};
#endif //define _CANCOMMUNICATION_HPP_