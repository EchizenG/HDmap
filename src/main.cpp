#include <errno.h>
#include <stdio.h>
#include "TCPcommunication.hpp"
#include "CANcommunication.hpp"
#include "search.hpp"
#include "resolve.hpp"
#include "config.hpp"

// void keepConnect_thread(void)
// {
//   printf("we are in keepConnect_thread\n");

//   TCPcommunication *Contact = TCPcommunication::getInstance();

//   Contact->setIP((char *)"192.168.3.191");
//   Contact->setPORT(5050);

//   while(1) {
//     if(TCPflag)
//     {
//       if(true == Contact->connectTCP())
//       {
//         TCPflag = 0;
//       }
//       sleep(1);
//     }
//   }

// }

// void sendData_thread(void)
// {
//   printf("we are in sendData_thread\n");
//   int infoSIZE = 0;
//   char reqINFO[1000] = {'\0'};
//   char headINFO[20] = {'\0'};

//   TCPcommunication *Contact = TCPcommunication::getInstance();
//   Resolve *resolver = Resolve::getInstance();

//   while(1)
//   {
//     switch(resolver->getFlag())
//     {
//       case OBU_MSG_MAP_REQ:
//         infoSIZE = resolver->setMAPreq(headINFO, reqINFO);
//         Contact->sendREQ(headINFO, reqINFO, infoSIZE);
//         resolver->setFlag(0);
//         break;

//       case OBU_MSG_PARKING_SPACE_REQ:
//         infoSIZE = resolver->setLOTreq(headINFO, reqINFO);
//         Contact->sendREQ(headINFO, reqINFO, infoSIZE);
//         resolver->setFlag(0);
//         break;

//       case OBU_MSG_GLOBAL_PATH_REQ:
//         infoSIZE = resolver->setPATHreq(headINFO, reqINFO);
//         Contact->sendREQ(headINFO, reqINFO, infoSIZE);
//         resolver->setFlag(0);
//         break;
//     }
//     sleep(1);
//   }
// }

// void recvData_thread(void)
// {
//   TCPcommunication *Contact = TCPcommunication::getInstance();
//   Resolve *resolver = Resolve::getInstance();
//   Search *HDmapSearcher = Search::getInstance();
//   char mapDirectory[500] = "/home/gaoyang/HiRain_Project/eMapGo/bin/oushangPark.sqlite";


//   printf("we are in recvData_thread\n");
//   while(!TCPflag)
//   {
//     TCPflag = Contact->parseHEAD();

//     struct TCPcommunication::msg_header head;
//     char *bodybuffer;

//     head = Contact->getHEAD();

//     bodybuffer = new char[head.body_len + 1]();
//     Contact->getBODY(bodybuffer, head.body_len);
//     switch(head.msg_type)
//     {
//       case OBU_MSG_START_PARK_NOTIFY:
//         resolver->parseStartParkingInfo(bodybuffer);
//         resolver->setFlag(OBU_MSG_MAP_REQ);
//         break;

//       case OBU_MSG_MAP_RSP:
//         resolver->getMAP(bodybuffer);
//         resolver->setFlag(OBU_MSG_PARKING_SPACE_REQ);
//         break;

//       case OBU_MSG_PARKING_SPACE_RSP:
//         resolver->parseParkingLotInfo(bodybuffer);
//         resolver->setFlag(OBU_MSG_GLOBAL_PATH_REQ);
//         break;

//       case OBU_MSG_GLOBAL_PATH_RSP:
//         resolver->parsePath(bodybuffer);
//         HDmapSearcher->SetPosition(resolver->getPos());
//         HDmapSearcher->SetRadius(5);
//         HDmapSearcher->SetPathID(resolver->getPathIDs());
//         HDmapSearcher->loadMap(mapDirectory);
//         // HDmapSearcher->findPathLine();
//         HDmapSearcher->drawWholeHLane();
//         HDmapSearcher->findHLane();
//         break;

//       case OBU_MSG_CAR_POS_NOTIFY:
//         resolver->parsePos(bodybuffer);
//         break;

//       case OBU_MSG_BARRIERS_NOTIFY: 
//         resolver->parseObjects(bodybuffer);
//         break;

//       case OBU_MSG_APA_STATUS_NOTIFY:
//         resolver->parseAPAStatus(bodybuffer);
//         break;
//     }

//     delete bodybuffer;
//   }
// }

// int main(void)
// {
//   // system("ip link set can1 down");
//   // system("echo 66 > /sys/class/gpio/export");
//   // system("echo out > /sys/class/gpio/gpio66/direction");
//   // system("echo 1 > value");
//   // system("echo 0 > value");
//   // system("ip link set can1 type can bitrate 500000");
//   // system("echo 1000 > /sys/class/net/can1/tx_queue_len");
//   // system("ip link set can1 up");
//   // system("ifconfig eth0 192.168.3.192 netmask 255.255.255.0");

//   std::thread conTCP(keepConnect_thread);
//   sleep(1);
//   std::thread sendData(sendData_thread);
//   sleep(1);
//   std::thread recvData(recvData_thread);
//   sleep(1);
//   // std::thread sendCAN(sendCAN_thread);

//   conTCP.join();
//   sendData.join();
//   recvData.join();
//   // sendCAN.join();

// }

static bool connect(void);
static bool canInitial(void);
static bool prepare(void);
static bool work(void);
static void updatePosThread(void);
static void reportPosThread(void);
static void sendCANthread(void);

int main(void)
{
  while(1)
  {
    if(true != connect())
    {
      continue;
    }

    if(true != canInitial())
    {
      continue;
    }

    if(true != prepare())
    {
      continue;
    }

    work();
  }

  return 0;
}

bool connect(void)
{
  printf("connecting the server...\n");

  TCPcommunication *TCPcontact = TCPcommunication::getInstance();

  TCPcontact->setIP((char *)"192.168.3.191");
  TCPcontact->setPORT(5050);

  while(true != TCPcontact->connectTCP())
  {
    printf("can not connect to server: %m\n");
    return false;
  }

  return true;
}

bool canInitial(void)
{
  CANcommunication *CANcontact = CANcommunication::getInstance();

  if(0 != CANcontact->can_open())
  {
    return false;
  }

  return true;
}

bool prepare(void)
{
  printf("preparing the data...\n");
  TCPcommunication *Contact = TCPcommunication::getInstance();
  Resolve *resolver = Resolve::getInstance();
  Search *HDmapSearcher = Search::getInstance();

  char mapDirectory[500] = "/home/gaoyang/HiRain_Project/eMapGo/bin/oushangPark.sqlite";

  bool positionStatus = false;
  bool mapStatus = false;
  bool HLaneStatus = false;
  bool vehicleStatus = false;
  bool lotStatus = false;

  int infoSIZE = 0;
  char reqINFO[1000] = {'\0'};
  char headINFO[20] = {'\0'};

  while(vehicleStatus || positionStatus || mapStatus || HLaneStatus || lotStatus)
  {
    if(true != Contact->parseHEAD())
    {
      return false;
    }

    struct TCPcommunication::msg_header head;
    char *bodybuffer;

    head = Contact->getHEAD();

    bodybuffer = new char[head.body_len + 1]();
    Contact->getBODY(bodybuffer, head.body_len);

    switch(head.msg_type)
    {
      case OBU_MSG_START_PARK_NOTIFY:
        resolver->parseStartParkingInfo(bodybuffer);
        vehicleStatus = true;
        infoSIZE = resolver->setMAPreq(headINFO, reqINFO);
        Contact->sendREQ(headINFO, reqINFO, infoSIZE);
        break;

      case OBU_MSG_MAP_RSP:
        resolver->getMAP(bodybuffer);
        HDmapSearcher->loadMap();
        mapStatus = true;
        infoSIZE = resolver->setLOTreq(headINFO, reqINFO);
        Contact->sendREQ(headINFO, reqINFO, infoSIZE);
        break;

      case OBU_MSG_PARKING_SPACE_RSP:
        resolver->parseParkingLotInfo(bodybuffer);
        lotStatus = true;
        infoSIZE = resolver->setPATHreq(headINFO, reqINFO);
        Contact->sendREQ(headINFO, reqINFO, infoSIZE);
        break;

      case OBU_MSG_GLOBAL_PATH_RSP:
        resolver->parsePath(bodybuffer);
        HDmapSearcher->SetHLaneID(resolver->getHLaneIDs());
        HLaneStatus = true;
        break;

      case OBU_MSG_CAR_POS_RSP:
        resolver->parsePos(bodybuffer);
        positionStatus = true;
        break;

      case OBU_MSG_BARRIERS_NOTIFY: 
        resolver->parseObjects(bodybuffer);
        break;

      case OBU_MSG_APA_STATUS_NOTIFY:
        resolver->parseAPAStatus(bodybuffer);
        break;
    }

    delete bodybuffer;
  }

  return true;
}

bool work(void)
{
  std::thread update(updatePosThread);
  std::thread report(reportPosThread);
  std::thread send(sendCANthread);

  update.join();
  report.join();
  send.join();
}

void updatePosThread(void)
{
  TCPcommunication *Contact = TCPcommunication::getInstance();
  Resolve *resolver = Resolve::getInstance();
  Search *HDmapSearcher = Search::getInstance();
  // char mapDirectory[500] = "/home/gaoyang/HiRain_Project/eMapGo/bin/oushangPark.sqlite";

int TCPflag = 0;
  printf("we are in updatePosThread\n");
  while(!TCPflag)
  {
    TCPflag = Contact->parseHEAD();

    struct TCPcommunication::msg_header head;
    char *bodybuffer;

    head = Contact->getHEAD();

    bodybuffer = new char[head.body_len + 1]();
    Contact->getBODY(bodybuffer, head.body_len);

    if(OBU_MSG_CAR_POS_RSP == head.msg_type)
    {
      resolver->parsePos(bodybuffer);
      HDmapSearcher->SetPosition(resolver->getPos());
      HDmapSearcher->findHLane();
    }
    delete bodybuffer;
  }
}

void reportPosThread(void)
{
  CANcommunication *Contact = CANcommunication::getInstance();

  //TODO
  //read position from CAN and report to server
  
}

void sendCANthread(void)
{
  printf("we are in sendCAN_thread\n");

  CANcommunication *Contact = CANcommunication::getInstance();
  Search *searcher = Search::getInstance();

  while(1)
  {
    Contact->stringifyCAN(searcher->getHLanePoints());
    Contact->sendCAN();
    sleep(1);
  }
}