#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <stdbool.h>
#include "communication.hpp"

Communication::Communication()
{
}

Communication::~Communication()
{
	close(conInfo.sHandler);
}

void Communication::setIP(char *IP)
{
	strncpy(conInfo.IP, IP, 15);
}

void Communication::setPORT(int port)
{
	conInfo.port = port;
}

bool Communication::connectTCP(void)
{
  std::cout<<"running connectTCP"<<std::endl;
 
  conInfo.sHandler = socket(PF_INET, SOCK_STREAM, 0);
  if(conInfo.sHandler == -1){
  	std::cout << "wrong handler" << errno << std::endl;
    return false;
  }
  memset(&conInfo.sin, 0, sizeof(conInfo.sin));
  conInfo.sin.sin_family = AF_INET;
  conInfo.sin.sin_addr.s_addr = inet_addr(conInfo.IP);
  conInfo.sin.sin_port = htons(conInfo.port);
  if(connect(conInfo.sHandler, (struct sockaddr*) &conInfo.sin, sizeof(conInfo.sin))==-1){
    std::cout << "connect error" << errno << std::endl;
    return false;
  }
  else{
    std::cout << "connected ...\n" << std::endl;
    return true;
  }
}

struct Communication::msg_header Communication::getHEAD(void)
{
	char buffer[18] = {'0'};

	if(17 == read(conInfo.sHandler,buffer,17))
	{
		header.msg_type = buffer[0];
		header.body_len = ( ( buffer[5] & 0xff) << 24 ) + ( ( buffer[6] & 0xff ) << 16 ) + ((buffer[7] & 0xff) << 8) + (buffer[8] & 0xff);

		return header;
	}
	else
	{
		printf("can not read msg head, errno: %d\n", errno);
		//return 0;
	}
}

bool Communication::getBODY(char *data, int size)
{
	if(read(conInfo.sHandler, data, size))
	{
		return true;
	}
	else
	{
		printf("can not read msg body, errno: %d\n", errno);
		return false;
	}
}

bool Communication::sendREQ(char *data, int size)
{
	if(write(conInfo.sHandler, data, size))
	{
		return true;
	}
	else
	{
		printf("can not write request msg, errno: %d\n", errno);
		return false;
	}
}

int Communication::can_open(struct can_hdl **hdl, struct can_cfg *cfg) {
	int ret;

	*hdl = (struct can_hdl *) malloc(sizeof(struct can_hdl));
	if (!*hdl) {
		ret = -1;
		goto err;
	}

	memset(*hdl, 0, sizeof(struct can_hdl));

	ret = can_socket_cfg(*hdl, cfg);
	if (ret != 0)
		goto err;

	goto out;
 err:
	free(*hdl);
	*hdl = NULL;
 out:
	return ret;
}

int Communication::can_close(struct can_hdl **hdl) {
	int ret;

	if (!hdl || !(*hdl)) {
		ret = -1;
		goto out;
	}

	ret = close((*hdl)->fd);
	if (ret < 0)
		ret = -errno;

	free(*hdl);
	*hdl = NULL;
 out:
	return ret;
}

ssize_t Communication::can_read(struct can_hdl *hdl, struct can_frame *frame) {
	return read(hdl->fd, frame, sizeof(struct can_frame));
}

ssize_t Communication::can_write(struct can_hdl *hdl, const struct can_frame *frame) {
	ssize_t nbytes = -1;
	struct sockaddr_can addr;

	addr.can_ifindex = hdl->ifindex;
	addr.can_family  = AF_CAN;

	// printf("id:%x len:%d\n", frame->can_id, frame->can_dlc);
  // for (int i = 0; i < frame->can_dlc; i++) {
  //         printf("%x ", frame->data[i]);
  // }
  // printf("\n");

	nbytes = sendto(hdl->fd, frame, sizeof(struct can_frame), MSG_DONTWAIT,
			(struct sockaddr *)&addr, sizeof(addr));


	if ((nbytes < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
		/* Socket TX buffer is full. This could happen if the unit is
		 * not connected to a CAN bus.
		 */
		nbytes = 0;
	// printf("%d\n",errno);
	return nbytes;
}


int Communication::can_socket_cfg(struct can_hdl *hdl, struct can_cfg *cfg) {
	int mtu = 0;
	int ret = 0;
	struct sockaddr_can addr;
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, cfg->ifname);

	hdl->fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (hdl->fd < 0) {
		fprintf(stderr, "%s Error while opening socket. errno: %d\n",
			__func__, errno);
		ret = -1;
		goto out;
	}

	if ((ioctl(hdl->fd, SIOCGIFINDEX, &ifr)) == -1) {
		fprintf(stderr, "%s Error getting interface index. errno: %d\n",
			__func__, errno);
		ret = -2;
		goto out;
	}

	hdl->ifindex = ifr.ifr_ifindex;

	if (cfg->mtu > CAN_MTU) {
		/* check if the frame fits into the CAN netdevice */
		if (ioctl(hdl->fd, SIOCGIFMTU, &ifr) == -1) {
			fprintf(stderr,
				"%s Error getting interface MTU. errno: %d\n",
				__func__, errno);
			ret = -3;
			goto out;
		}

		mtu = ifr.ifr_mtu;
		// if (mtu != CANFD_MTU) {
		// 	fprintf(stderr,
		// 		"%s Error: Interface MTU (%d) is not valid. Expected %zu\n",
		// 		__func__, mtu, CANFD_MTU);
		// 	ret = -4;
		// 	goto out;
		// }
	}

	addr.can_family  = AF_CAN;
	addr.can_ifindex = hdl->ifindex;
	ret = bind(hdl->fd, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		fprintf(stderr, "%s Error in socket bind. errno: %d\n",
			__func__, errno);
		ret = -6;
		goto out;
	}

	// if (cfg->rx_filter)
	// 	/* Setup CAN ID filter*/
	// 	setsockopt(hdl->fd, SOL_CAN_RAW, CAN_RAW_FILTER,
	// 		   cfg->rx_filter, cfg->rx_filter_len);

 out:
	return ret;
}

