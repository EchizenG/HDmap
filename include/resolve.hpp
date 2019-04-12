#ifndef _RESOLVE_HPP
#define _RESOLVE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "boost/property_tree/ptree.hpp"
#include "config.hpp"



class Resolve
{
public:

  static Resolve* getInstance(){
    static Resolve instance;
    return &instance;
  }
  //Resolve();
  ~Resolve();

  struct msg_header
  {
    uint8_t  msg_type;
    uint16_t seq_nUM;
    uint16_t sid;
    uint32_t body_len;
    int64_t timestamp;
  };

  bool parseJSON(const std::string json, boost::property_tree::ptree& messages);
  bool stringifyJSON(std::stringstream &sendData, const boost::property_tree::ptree pt);
  bool getMAP(char *bodybuffer);
  bool parseStartParkingInfo(char *data);
  bool parseParkingLotInfo(char *data);
  bool parsePath(char *data);
  bool parsePos(char *data);
  bool parseObjects(char *data);
  bool parseAPAStatus(char *data);


  size_t setMAPreq(char *head, char *data);
  size_t setLOTreq(char *head, char *data);
  size_t setPATHreq(char *head, char *data);

  void   setFlag(int flagValue);
  int    getFlag(void);

  std::vector<int64_t> getPathIDs(void);
  std::vector<int64_t> getHLaneIDs(void);
  
  int getParkLotID(void);
  double* getPos(void);
  char* getMAPname(void);


private:
  Resolve(const Resolve &other);
  Resolve();//for single instance
  char *getHeader(void);
  void constructHead(int type, int size);

  std::vector<int64_t> _pathIDs;
  std::vector<int64_t> _HLaneIDs;

  std::mutex _Resolve_mutex;
  int sendReqFlag;
  char vehicleID[5];
  char _mapName[50];
  int _parkLot_length, _parkLot_width, _parkLot_height, _parkLot_ID;
  double _pos_X, _pos_Y, _pos_Z, _pos_heading, _pos_speed;
  double _tranmitPos[3];
  char _req_head[20];
};
#endif //define _RESOLVE_HPP