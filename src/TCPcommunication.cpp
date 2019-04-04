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

  std::unique_lock<std::mutex> lockTmp(TCPmutex);

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

int TCPcommunication::parseHEAD(void)
{
  char buffer[18] = {'0'};

  if(17 == read(conInfo.sHandler,buffer,17))
  {
    header.msg_type = buffer[0];
    header.body_len = ( ( buffer[5] & 0xff) << 24 ) + ( ( buffer[6] & 0xff ) << 16 ) + ((buffer[7] & 0xff) << 8) + (buffer[8] & 0xff);

    return 0;
  }
  else
  {
    printf("can not read msg head, errno: %d\n", errno);
    return errno;
  }
}

struct TCPcommunication::msg_header TCPcommunication::getHEAD(void)
{
  return header;
}

bool TCPcommunication::getBODY(char *data, int size)
{
  std::unique_lock<std::mutex> lockTmp(TCPmutex);

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

bool TCPcommunication::sendREQ(char *data, int size)
{
  std::unique_lock<std::mutex> lockTmp(TCPmutex);

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

