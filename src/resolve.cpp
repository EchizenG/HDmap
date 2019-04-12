#include "resolve.hpp"
#include "errno.h"
#include <string>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/property_tree/detail/json_parser_read.hpp>
#include <boost/property_tree/detail/json_parser_write.hpp>
#include <boost/type_traits/make_unsigned.hpp>

using namespace boost::archive::iterators;

static bool Base64Encode( const std::string & input, std::string * output )
{
  typedef base64_from_binary<transform_width<std::string::const_iterator, 6, 8>> Base64EncodeIterator;
  std::stringstream result;
  try {
    copy( Base64EncodeIterator( input.begin() ),
          Base64EncodeIterator( input.end() ),
          std::ostream_iterator<char>( result ) );
  } catch ( ... ) {
    return false;
  }
  size_t equal_count = (3 - input.length() % 3) % 3;
  for ( size_t i = 0; i < equal_count; i++ )
  {
    result.put( '=' );
  }
  *output = result.str();
  return output->empty() == false;
}
 
static bool Base64Decode( const std::string & input, std::string * output )
{
  typedef transform_width<binary_from_base64<std::string::const_iterator>, 8, 6> Base64DecodeIterator;
  std::stringstream result;
  try {
    copy( Base64DecodeIterator( input.begin() ),
          Base64DecodeIterator( input.end() ),
          std::ostream_iterator<char>( result ) );
  } catch ( ... ) {
    return false;
  }
  *output = result.str();
  return output->empty() == false;
}

Resolve::Resolve()
{}

Resolve::~Resolve()
{}

bool Resolve::parseJSON(const std::string json, boost::property_tree::ptree& messages)
{
  //std::basic_istream<typename boost::property_tree::ptree::key_type::value_type> stream(json);
  std::stringstream stream(json);
  // printf("we get stream: %s\n", stream.str().c_str());
  boost::property_tree::json_parser::read_json_internal(stream, messages, std::string());
  return true;
}

bool Resolve::stringifyJSON(std::stringstream &sendData, const boost::property_tree::ptree pt)
{
  boost::property_tree::json_parser::write_json_internal(sendData, pt, std::string(), true);
  return true;
}

bool Resolve::getMAP(char *data)
{
  Resolve parseMap;

  boost::property_tree::ptree::iterator ite;
  boost::property_tree::ptree mapTree;
  boost::property_tree::ptree mapTree_result;
  boost::property_tree::ptree mapTree_files;
  boost::property_tree::ptree mapTree_oneMap;

  std::string mapTree_name, mapTree_size, mapTree_stream;

  parseMap.parseJSON(data, mapTree);
  mapTree_result = mapTree.get_child("result");
  mapTree_files = mapTree_result.get_child("map_files");

  for(ite = mapTree_files.begin(); ite != mapTree_files.end(); ite++)
  {
    mapTree_oneMap = ite->second;
    mapTree_name = mapTree_oneMap.get<std::string>("file_name");
    mapTree_size = mapTree_oneMap.get<std::string>("file_size");
    mapTree_stream = mapTree_oneMap.get<std::string>("file_stream");
  
    std::string mapdata(std::stoi(mapTree_size) + 10,'0');

    Base64Decode(mapTree_stream, &mapdata);

    int diff = mapdata.length() - std::stoi(mapTree_size);

    for(int i = 0; i < diff; i++)
    {
      mapdata.pop_back();
    }

    std::ofstream mapfile(mapTree_name);
    strcpy(_mapName, mapTree_name.c_str());
    
    mapfile << mapdata;
    mapfile.close();
  }

  return true;
}

char* Resolve::getMAPname(void)
{
  return _mapName;
}

bool Resolve::parseStartParkingInfo(char *data)
{
  Resolve parseParking;

  boost::property_tree::ptree parkTree;
  boost::property_tree::ptree parkTree_startAddress;

  std::string parkTree_vehicleID, parkTree_addressType, parkTree_address;


  parseParking.parseJSON(data, parkTree);
  parkTree_vehicleID = parkTree.get<std::string>("vehicleID");
  parkTree_startAddress = parkTree.get_child("start_address");

  parkTree_addressType = parkTree_startAddress.get<std::string>("addressType");
  parkTree_address = parkTree_startAddress.get<std::string>("address");
  
  _Resolve_mutex.lock();
  strncpy(vehicleID, parkTree_vehicleID.c_str(), 5);
  _Resolve_mutex.unlock();

  return true;
}

bool Resolve::parseParkingLotInfo(char *data)
{
  Resolve parseParkingLot;

  boost::property_tree::ptree parkTree;
  boost::property_tree::ptree parkTree_result;
  boost::property_tree::ptree parkTree_parkingSpacePara;


  std::string parkTree_parkingSpaceID, parkTree_parkingSpaceType,
              parkTree_length, parkTree_width, parkTree_height;

  parseParkingLot.parseJSON(data, parkTree);

  parkTree_result = parkTree.get_child("result");

  parkTree_parkingSpaceID = parkTree_result.get<std::string>("parkingSpaceID");
  parkTree_parkingSpacePara = parkTree_result.get_child("parkingSpacePara");
  parkTree_parkingSpaceType = parkTree_parkingSpacePara.get<std::string>("parkingSpaceType");
  parkTree_length = parkTree_parkingSpacePara.get<std::string>("length");
  parkTree_width = parkTree_parkingSpacePara.get<std::string>("width");
  parkTree_height = parkTree_parkingSpacePara.get<std::string>("height");

  _Resolve_mutex.lock();
  _parkLot_length = std::stoi(parkTree_length);
  _parkLot_width = std::stoi(parkTree_width);
  _parkLot_height = std::stoi(parkTree_height);
  _parkLot_ID = std::stoi(parkTree_parkingSpaceID);
  _Resolve_mutex.unlock();

  return true;
}

int Resolve::getParkLotID(void)
{
  std::unique_lock<std::mutex> lockTmp(_Resolve_mutex);
  return _parkLot_ID;
}
bool Resolve::parsePath(char *data)
{
  Resolve parsePath;

  boost::property_tree::ptree::iterator ite;
  boost::property_tree::ptree pathTree;
  boost::property_tree::ptree pathTree_result;
  boost::property_tree::ptree pathTree_point;
  boost::property_tree::ptree pathTree_paths;
  boost::property_tree::ptree pathTree_singlePath;

  std::string pathTree_srcX, pathTree_srcY, pathTree_srcZ,
              pathTree_pathID,
              pathTree_dstX, pathTree_dstY, pathTree_dstZ;

  parsePath.parseJSON(data, pathTree);

  pathTree_result = pathTree.get_child("result");

  pathTree_point = pathTree_result.get_child("start_point");
  pathTree_srcX = pathTree_point.get<std::string>("x");
  pathTree_srcY = pathTree_point.get<std::string>("y");
  pathTree_srcZ = pathTree_point.get<std::string>("z");
  pathTree_point.clear();

  pathTree_point = pathTree_result.get_child("end_point");
  pathTree_dstX = pathTree_point.get<std::string>("x");
  pathTree_dstY = pathTree_point.get<std::string>("y");
  pathTree_dstZ = pathTree_point.get<std::string>("z");
  pathTree_point.clear();

  pathTree_paths = pathTree_result.get_child("lanes");
  _Resolve_mutex.lock();
  for(ite = pathTree_paths.begin(); ite != pathTree_paths.end(); ite++)
  {
    pathTree_singlePath = ite->second;
    pathTree_pathID = pathTree_singlePath.get<std::string>("lane_id");

    _pathIDs.push_back(std::stol(pathTree_pathID.c_str()));
    // _HLaneIDs.push_back(std::stol(pathTree_pathID.c_str()));

  }
  _Resolve_mutex.unlock();

  return true;
}

std::vector<int64_t> Resolve::getPathIDs(void)
{
  std::unique_lock<std::mutex> lockTmp(_Resolve_mutex);
  return _pathIDs;
}

std::vector<int64_t> Resolve::getHLaneIDs(void)
{
  std::unique_lock<std::mutex> lockTmp(_Resolve_mutex);
  return _HLaneIDs;
}

double* Resolve::getPos(void)
{
  _tranmitPos[0] = _pos_X;
  _tranmitPos[1] = _pos_Y;
  _tranmitPos[2] = _pos_Z;

  return _tranmitPos;  
}

bool Resolve::parsePos(char *data)
{
  Resolve parsePos;

  boost::property_tree::ptree posTree;
  boost::property_tree::ptree posTree_result;
  boost::property_tree::ptree posTree_location;

  std::string posTree_locX, posTree_locY, posTree_locZ,
              posTree_heading, posTree_speed;

  parsePos.parseJSON(data, posTree);

  // posTree_vehicleID = posTree.get<std::string>("vehicleID");
  posTree_result = posTree.get_child("result");

  posTree_location = posTree_result.get_child("location");
  posTree_locX = posTree_location.get<std::string>("x");
  posTree_locY = posTree_location.get<std::string>("y");
  posTree_locZ = posTree_location.get<std::string>("z");

  posTree_heading = posTree_result.get<std::string>("heading");
  posTree_speed = posTree_result.get<std::string>("speed");

  // if(0 == posTree_vehicleID.compare(vehicleID))
  // {
    _Resolve_mutex.lock();
    _pos_X = std::stod(posTree_locX);
    _pos_Y = std::stod(posTree_locY);
    _pos_Z = std::stod(posTree_locZ);
    _pos_heading = std::stod(posTree_heading);
    _pos_speed = std::stod(posTree_speed);
    _Resolve_mutex.unlock();
  // }
  // else
  // {
  //   printf("This is not for our vehicle: %s, it is for %s\n", vehicleID, posTree_vehicleID.c_str());
  //   return false;
  // }

  return true;
}


bool Resolve::parseObjects(char *data)
{
  Resolve parseObj;

  boost::property_tree::ptree::iterator ite_obj, ite_loc;
  boost::property_tree::ptree objTree;
  boost::property_tree::ptree objTree_multiObjs;
  boost::property_tree::ptree objTree_singleObj;
  boost::property_tree::ptree objTree_result;
  boost::property_tree::ptree objTree_multiLocations;
  boost::property_tree::ptree objTree_singleLocation;


  std::string objTree_objectID, objTree_objectType, objTree_confidence,
              objTree_locX, objTree_locY, objTree_locZ,
              objTree_heading, objTree_speed;

  parseObj.parseJSON(data, objTree);
  objTree_multiObjs = objTree.get_child("objects");

  //TODO: add mutex
  for(ite_obj = objTree_multiObjs.begin(); ite_obj != objTree_multiObjs.end(); ite_obj++)
  {
    objTree_singleObj = ite_obj->second;

    objTree_objectID = objTree_singleObj.get<std::string>("objectID");
    objTree_objectType = objTree_singleObj.get<std::string>("objectType");
    objTree_confidence = objTree_singleObj.get<std::string>("confidence");
    objTree_heading = objTree_singleObj.get<std::string>("speed");
    objTree_speed = objTree_singleObj.get<std::string>("heading");
    objTree_multiLocations = objTree_singleObj.get_child("location");
  
    for(ite_loc = objTree_multiLocations.begin(); ite_loc != objTree_multiLocations.end(); ite_loc++)
    {
      objTree_singleLocation = ite_loc->second;

      objTree_locX = objTree_singleLocation.get<std::string>("x");
      objTree_locY = objTree_singleLocation.get<std::string>("y");
      objTree_locZ = objTree_singleLocation.get<std::string>("z");

      //TODO: define a vessel to contain the object
    }
  }

  return true;
}


bool Resolve::parseAPAStatus(char *data)
{
  // std::unique_lock<std::mutex> lockTmp(_Resolve_mutex);

  Resolve parseAPASta;

  boost::property_tree::ptree statuAPATree;

  std::string statuAPATree_parkingFinished,
              statuAPATree_callingFinished,
              statuAPATree_okline,
              statuAPATree_findingstopline,
              statuAPATree_stopline,
              statuAPATree_parking;

  parseAPASta.parseJSON(data, statuAPATree);

  statuAPATree_parkingFinished = statuAPATree.get<std::string>("parkingFinished");
  statuAPATree_callingFinished = statuAPATree.get<std::string>("callingFinished");
  statuAPATree_okline = statuAPATree.get<std::string>("okline");
  statuAPATree_findingstopline = statuAPATree.get<std::string>("findingstopline");
  statuAPATree_stopline = statuAPATree.get<std::string>("stopline");
  statuAPATree_parking = statuAPATree.get<std::string>("parking");

  //TODO: add a vessel and mutex
  return true;
}

void Resolve::constructHead(int type, int size)
{
  time_t timep; 
  time(&timep);

  _req_head[0] = type;
  _req_head[1] = 0x00;
  _req_head[2] = 0x00;
  _req_head[3] = 0x00;
  _req_head[4] = 0x00;
  _req_head[5] = ((size & 0xff000000) >> 24);
  _req_head[6] = ((size & 0xff0000) >> 16);
  _req_head[7] = ((size & 0xff00) >> 8);
  _req_head[8] =  (size & 0xff);
  _req_head[9] =  ((timep & 0xff00000000000000) >> 56);
  _req_head[10] =  ((timep & 0xff000000000000) >> 48);
  _req_head[11] =  ((timep & 0xff0000000000) >> 40);
  _req_head[12] =  ((timep & 0xff00000000) >> 32);
  _req_head[13] =  ((timep & 0xff000000) >> 24);
  _req_head[14] =  ((timep & 0xff0000) >> 16);
  _req_head[15] =  ((timep & 0xff00) >> 8);
  _req_head[16] =  (timep & 0xff);
}

char *Resolve::getHeader(void)
{
  return _req_head;
}

size_t Resolve::setMAPreq(char *head, char *data)
{
  std::unique_lock<std::mutex> lockTmp(_Resolve_mutex);

  boost::property_tree::ptree pt_singleItem;
  boost::property_tree::ptree pt_multeItems;
  boost::property_tree::ptree pt_reqInfo;

  char map_id[15] = "qianhai_001";
  char version[5] = "v1.0";
  char lastModifyTime[20] = "2019-2-8 10:11:23";

  pt_singleItem.put("map_id", map_id);
  pt_singleItem.put("version", version);
  pt_singleItem.put("lastModifyTime", lastModifyTime);

  pt_multeItems.push_back(std::make_pair("",pt_singleItem));
  pt_reqInfo.add_child("local_maps", pt_multeItems);

  std::stringstream ss;
  stringifyJSON(ss, pt_reqInfo);

  std::string reqInfo = ss.str();
  memcpy(data, reqInfo.c_str(), reqInfo.size() + 1); 

  constructHead(OBU_MSG_MAP_REQ, reqInfo.size());
  memcpy(head, getHeader(), 17);

  return reqInfo.size();
}

size_t Resolve::setLOTreq(char *head, char *data)
{
  std::unique_lock<std::mutex> lockTmp(_Resolve_mutex);

  boost::property_tree::ptree pt_singleItem;

  pt_singleItem.put("vehicleID", vehicleID);

  std::stringstream ss;
  stringifyJSON(ss, pt_singleItem);

  std::string reqInfo = ss.str();
  // printf("we build json: \n%ssize: %d", reqInfo.c_str(), reqInfo.size());

  memcpy(data, reqInfo.c_str(), reqInfo.size() + 1); 

  constructHead(OBU_MSG_PARKING_SPACE_REQ, reqInfo.size());
  memcpy(head, getHeader(), 17);

  _Resolve_mutex.unlock();
  return reqInfo.size();
}

size_t Resolve::setPATHreq(char *head, char *data)
{
  std::unique_lock<std::mutex> lockTmp(_Resolve_mutex);

  boost::property_tree::ptree pt_singleItem;
  boost::property_tree::ptree pt_location;
  boost::property_tree::ptree pt_reqInfo;

  char src_addressType[15] = "locPoint";//src: 
  // int src_address = 1100000004;//src: TCP: Huawei position in real-time("address":{"x":6.123,"y":5.123,"z":4.123})
  char dst_addressType[15] = "parkingSpace";
  int dst_address = 1100000153;//src: TCP: start parking vehicle req info

  pt_reqInfo.put("vehicleID", vehicleID);

  pt_singleItem.put("addressType", src_addressType);
    pt_location.put("x", _pos_X);
    pt_location.put("y", _pos_Y);
    pt_location.put("z", _pos_Z);
  pt_singleItem.add_child("address", pt_location);
  pt_reqInfo.add_child("srcAddress", pt_singleItem);
  pt_singleItem.clear();

  pt_singleItem.put("addressType", dst_addressType);
  pt_singleItem.put("address", dst_address);
  pt_reqInfo.add_child("dstAddress", pt_singleItem);
  pt_singleItem.clear();

  std::stringstream ss;
  stringifyJSON(ss, pt_reqInfo);

  std::string reqInfo = ss.str();
  // printf("we build json: \n%ssize: %d", reqInfo.c_str(), reqInfo.size());

  memcpy(data, reqInfo.c_str(), reqInfo.size() + 1); 

  constructHead(OBU_MSG_GLOBAL_PATH_REQ, reqInfo.size());
  memcpy(head, getHeader(), 17);

  _Resolve_mutex.unlock();
  return reqInfo.size();
}

void Resolve::setFlag(int flagValue)
{
  std::unique_lock<std::mutex> lockTmp(_Resolve_mutex);
  sendReqFlag = flagValue;
  _Resolve_mutex.unlock();
}

int Resolve::getFlag(void)
{
  std::unique_lock<std::mutex> lockTmp(_Resolve_mutex);
  return sendReqFlag;
}