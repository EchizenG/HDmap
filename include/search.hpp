#ifndef _SEARCH_HPP_
#define _SEARCH_HPP_

#include <iostream>
#include <kdtree.h>
#include <mutex>
#include "dataManager/datamanager.h"

class Search
{
public:
  static Search* getInstance(){
        static Search instance;
        return &instance;
    }

  bool loadMap(char *mapDirectory);
  bool findParkingLot(void);
  bool findPathLine(void);//negotiate
  bool findObstacle(void);//input:position,radius
  bool findHLane(void);//input:pathID, postion, radius, 30 points

  
  bool SetPosition(const double *curPosition);
  bool SetRadius(const double curradius);
  bool SetPathID(const OID curID);
  bool getNPointsFromHere(const int number);


  struct parkinglot GetParkinglot(void);
  struct obstacle GetObstacle(void);
  struct hlane GetHLane(void);
  std::vector<OGRPoint> getHLanePoints(void);

  ~Search();
private:
  Search();
  Search(const Search &other);

  bool buildHLane(ILayer *pLayer);
  bool buildHLaneFromPathline(ILayer *pLayer);
  bool buildHLaneGeneralInfo(void);
  bool buildHLaneStruct(OGRFeature *pOGRFeature);
  bool buildNavPointsKDtree(void);
  bool getNearest(const double *curPos);

  DataManager *pDataManager;

  struct parkinglot
  {
    int PSID;
    int PBGID;
    int PARKCODE;
    int LHEIGHT;
    int LINKID;
    char HPTYPE;
    char DTYPE;
  };

  struct pathline
  {
    int LINKID;
    int FJCID;
    int TJCID;
    int PBGID;
    int LENGTH;
    int WIDTH;
    char TYPE;
    char DT;
  };

  struct obstacle
  {
    int OBDID;
    int AHEIGHT;
    int PBGID;
    char LINKIDS;
    char TYPE;
  };

  struct hlane
  {
    int HLaneID;
    int SLNodeID;
    int ELNodeID;
    int PBGID;
    int LWIDTH;
    int LINKID;
    int HGNodeID;
    int LRBID;
    int RRBID;
    char HLPType;
  };

  struct parkinglot parkingLot;
  struct pathline pathLine;
  struct obstacle obsInfo;
  struct hlane hLane;
  struct kdtree *HLaneTree;

  double Position[2];
  double nearHDPoint[3];
  double radius;
  OGRPoint OGRPosition;
  OID parkinglotID;

  std::vector<OID> pathIDs;
  std::vector<IFeature*> vHLaneFeatures;
  std::vector<OGRPoint> vPoints;

  std::mutex SEAmutex;

};
#endif //define _SEARCH_HPP_