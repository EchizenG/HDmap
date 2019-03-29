#ifndef _COMMUNICATION_HPP_
#define _COMMUNICATION_HPP_

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

class Communication
{
public:
 	Communication();
 	~Communication();

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

	bool                    connectTCP(void);
	struct msg_header      getHEAD(void);
	bool                    getBODY(char *data);
	void                    setIP(char *IP);
	void                    setPORT(int port);
  	
	int     can_open(struct can_hdl **hdl, struct can_cfg *cfg);
	int     can_close(struct can_hdl **hdl);
	ssize_t can_read(struct can_hdl *hdl, struct can_frame *frame);
	ssize_t can_write(struct can_hdl *hdl, const struct can_frame *frame);

protected:
	struct connectInfo conInfo;
	struct msg_header header;

private:
	int can_socket_cfg(struct can_hdl *hdl, struct can_cfg *cfg);

};
#endif //define _COMMUNICATION_HPP_