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

  for(std::vector<OGRPoint>::iterator ite = vPoints.begin(); ite != vPoints.end(); ite++)
  {
    //TODO: get DBC to parse it;note: double 2 char use sscanf()
    // ite.getX();
    // ite.getY();
  }
  return true;
}

bool CANcommunication::sendCAN(void)
{
  std::unique_lock<std::mutex> lockTmp(CANmutex);
  can_write(frame);
  return true;
}

int CANcommunication::can_open() {
  int ret;
  std::unique_lock<std::mutex> lockTmp(CANmutex);
  cfg.ifname = "can0";

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
  return ret;
}

ssize_t CANcommunication::can_read(void) {
  std::unique_lock<std::mutex> lockTmp(CANmutex);

  return read(hdl->fd, frame, sizeof(struct can_frame));
}

ssize_t CANcommunication::can_write(const struct can_frame *frame) {
  ssize_t nbytes = -1;
  struct sockaddr_can addr;
  std::unique_lock<std::mutex> lockTmp(CANmutex);

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

int CANcommunication::can_socket_cfg(void) {
  int mtu = 0;
  int ret = 0;
  struct sockaddr_can addr;
  struct ifreq ifr;

  memset(&ifr, 0, sizeof(ifr));
  strcpy(ifr.ifr_name, cfg.ifname);

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

  if (cfg.mtu > CAN_MTU) {
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
    fprintf(stderr, "%s Error in socket bind. errno: %d\n",
      __func__, errno);
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

