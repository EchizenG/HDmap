#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdbool.h>
#include "CANcommunication.hpp"

CANcommunication *CANcommunication::instance = NULL;

CANcommunication::CANcommunication(void)
{
}

CANcommunication::~CANcommunication(void)
{
  can_close();
}

CANcommunication* CANcommunication::getInstance(void)
{
  if(NULL == instance)
  {
    instance = new CANcommunication();
  }

  return instance;
}

bool CANcommunication::stringifyCAN(std::vector<OGRPoint> vPoints)
{
  std::unique_lock<std::mutex> lockTmp(CANmutex);
  char Point1_x[2], Point1_y[2], Point2_x[2], Point2_y[2];
  int count = vPoints.size();
  int j = 0;

  for(int i = 0; i < count; i += 2)
  {
    //TODO: get DBC to parse it;note: double 2 char use sscanf()
    sscanf(Point1_x, "%lf", &vPoints.at(i));
    sscanf(Point1_y, "%lf", &vPoints.at(i));
    if(i+1 < count)
    {
      sscanf(Point2_x, "%lf", &vPoints.at(i+1));
      sscanf(Point2_y, "%lf", &vPoints.at(i+1));
    }

    send_frame.can_id = 0x700 + j++;
    send_frame.can_dlc = 8;
    send_frame.data[0] = Point1_x[0];
    send_frame.data[1] = Point1_x[1];
    send_frame.data[2] = Point1_y[0];
    send_frame.data[3] = Point1_y[1];
    send_frame.data[4] = Point2_x[0];
    send_frame.data[5] = Point2_x[1];
    send_frame.data[6] = Point2_y[0];
    send_frame.data[7] = Point2_y[1];

    Vsend_frame.push_back(send_frame);
  }
  CANmutex.unlock();

  return true;
}

bool CANcommunication::sendCAN(void)
{
  can_write();
  return true;
}

int CANcommunication::can_open() {
  int ret;
  std::unique_lock<std::mutex> lockTmp(CANmutex);
  cfg.ifname = (char*)"can1";

  hdl = (struct can_hdl *) malloc(sizeof(struct can_hdl));
  if (!hdl) {
    ret = -1;
    goto err;
  }

  memset(hdl, 0, sizeof(struct can_hdl));

  ret = can_socket_cfg();
  if (ret != 0)
    goto err;

  goto out;
 err:
  free(hdl);
  hdl = NULL;
 out:

  CANmutex.unlock();
  return ret;
}

int CANcommunication::can_close(void) {
  int ret;
  std::unique_lock<std::mutex> lockTmp(CANmutex);

  if (!&hdl || !(hdl)) {
    ret = -1;
    goto out;
  }

  ret = close((hdl)->fd);
  if (ret < 0)
    ret = -errno;

  free(hdl);
  hdl = NULL;
 out:
  CANmutex.unlock();
  return ret;
}

ssize_t CANcommunication::can_read(void) {
  std::unique_lock<std::mutex> lockTmp(CANmutex);

  return read(hdl->fd, &rcv_frame, sizeof(struct can_frame));
}

ssize_t CANcommunication::can_write(void) {
  ssize_t nbytes = -1;
  struct sockaddr_can addr;
  std::unique_lock<std::mutex> lockTmp(CANmutex);

  addr.can_ifindex = hdl->ifindex;
  addr.can_family  = AF_CAN;

  // printf("id:%x len:%d\n", send_frame.can_id, send_frame.can_dlc);
  // for (int i = 0; i < send_frame.can_dlc; i++) {
  //         printf("%x ", send_frame.data[i]);
  // }
  // printf("\n");

  int framesize = Vsend_frame.size();
  for(int i = 0; i < framesize; i++)
  {
    nbytes = sendto(hdl->fd, &Vsend_frame.at(i), sizeof(struct can_frame), MSG_DONTWAIT,
        (struct sockaddr *)&addr, sizeof(addr));
  }

  if ((nbytes < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
    /* Socket TX buffer is full. This could happen if the unit is
     * not connected to a CAN bus.
     */
    nbytes = 0;
  printf("sendCAN: %m\n");
  CANmutex.unlock();
  return nbytes;
}

int CANcommunication::can_socket_cfg(void) {
  int ret = 0;
  struct sockaddr_can addr;
  struct ifreq ifr;

  memset(&ifr, 0, sizeof(ifr));
  strcpy(ifr.ifr_name, cfg.ifname);

  hdl->fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (hdl->fd < 0) {
    fprintf(stderr, "%s Error while opening socket: %m\n",
      __func__);
    ret = -1;
    goto out;
  }

  if ((ioctl(hdl->fd, SIOCGIFINDEX, &ifr)) == -1) {
    fprintf(stderr, "%s Error getting interface index: %m\n",
      __func__);
    ret = -2;
    goto out;
  }

  hdl->ifindex = ifr.ifr_ifindex;

  if (cfg.mtu > CAN_MTU) {
    /* check if the frame fits into the CAN netdevice */
    if (ioctl(hdl->fd, SIOCGIFMTU, &ifr) == -1) {
      fprintf(stderr,
        "%s Error getting interface MTU: %m\n",
        __func__);
      ret = -3;
      goto out;
    }

    // if (mtu != CANFD_MTU) {
    //  fprintf(stderr,
    //    "%s Error: Interface MTU (%d) is not valid. Expected %zu\n",
    //    __func__, mtu, CANFD_MTU);
    //  ret = -4;
    //  goto out;
    // }
  }

  addr.can_family  = AF_CAN;
  addr.can_ifindex = hdl->ifindex;
  ret = bind(hdl->fd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) {
    fprintf(stderr, "%s Error in socket bind: %m\n",
      __func__);
    ret = -6;
    goto out;
  }

  // if (cfg->rx_filter)
  //  /* Setup CAN ID filter*/
  //  setsockopt(hdl->fd, SOL_CAN_RAW, CAN_RAW_FILTER,
  //       cfg->rx_filter, cfg->rx_filter_len);

 out:
  return ret;
}

