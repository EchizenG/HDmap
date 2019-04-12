#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <stdbool.h>
#include "TCPcommunication.hpp"

TCPcommunication *TCPcommunication::instance = NULL;

TCPcommunication* TCPcommunication::getInstance(void)
{
  if(NULL == instance)
  {
    instance = new TCPcommunication();
  }

  return instance;
}

TCPcommunication::TCPcommunication(void)
{
}

TCPcommunication::~TCPcommunication()
{
  close(conInfo.sHandler);
}

void TCPcommunication::setIP(char *IP)
{
  strncpy(conInfo.IP, IP, 15);
}

void TCPcommunication::setPORT(int port)
{
  conInfo.port = port;
}

bool TCPcommunication::connectTCP(void)
{
  std::cout<<"running connectTCP"<<std::endl;

  TCPmutex.lock();

  conInfo.sHandler = socket(PF_INET, SOCK_STREAM, 0);
  if(conInfo.sHandler == -1){
    TCPmutex.unlock();
    printf("wrong handler: %m\n");
    return false;
  }
  memset(&conInfo.sin, 0, sizeof(conInfo.sin));
  conInfo.sin.sin_family = AF_INET;
  conInfo.sin.sin_addr.s_addr = inet_addr(conInfo.IP);
  conInfo.sin.sin_port = htons(conInfo.port);
  if(connect(conInfo.sHandler, (struct sockaddr*) &conInfo.sin, sizeof(conInfo.sin))==-1){
    TCPmutex.unlock();
    printf("connect error:%m\n");
    return false;
  }
  else{
    TCPmutex.unlock();
    printf("connected ...\n");
    return true;
  }
}

bool TCPcommunication::parseHEAD(void)
{
  char buffer[18] = {0x0f};
  std::unique_lock<std::mutex> lockTmp(TCPmutex);

  if(17 == read(conInfo.sHandler,buffer,17))
  {
    header.msg_type = buffer[0];
    header.body_len = ( ( buffer[5] & 0xff) << 24 ) +
                      ( ( buffer[6] & 0xff ) << 16 ) +
                      ( ( buffer[7] & 0xff) << 8) +
                      ( buffer[8] & 0xff);
    TCPmutex.unlock();
    return true;
  }
  else
  {
    TCPmutex.unlock();
    printf("can not read msg head: %m\n");
    return false;
  }
}

struct TCPcommunication::msg_header TCPcommunication::getHEAD(void)
{
  std::unique_lock<std::mutex> lockTmp(TCPmutex);
  return header;
}

bool TCPcommunication::getBODY(char *data, int size)
{
  std::unique_lock<std::mutex> lockTmp(TCPmutex);

  if(read(conInfo.sHandler, data, size))
  {
    TCPmutex.unlock();
    return true;
  }
  else
  {
    TCPmutex.unlock();
    printf("can not read msg body: %m\n");
    return false;
  }
}

bool TCPcommunication::sendREQ(char *head, char *data, int size)
{
  if(write(conInfo.sHandler, head, 17))
  {
    if(write(conInfo.sHandler, data, size))
    {
      TCPmutex.unlock();
      return true;
    }
  }
  else
  {
    TCPmutex.unlock();
    printf("can not write request msg: %m\n");
    return false;
  }
}

