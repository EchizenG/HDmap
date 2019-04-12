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

    bool                 stringifyCAN(std::vector<OGRPoint> vPoints);
    bool                 sendCAN(void);

    
    int     can_open(void);
    int     can_close(void);
    ssize_t can_read(void);
    ssize_t can_write(void);
    
    ~CANcommunication();
private:
    CANcommunication();

    static CANcommunication *instance;


    int can_socket_cfg(void);
    
    std::mutex CANmutex;

    struct can_hdl *hdl;
    struct can_frame send_frame;
    struct can_frame rcv_frame;
    struct can_cfg cfg;
    std::vector<struct can_frame> Vsend_frame;
};
#endif //define _CANCOMMUNICATION_HPP_