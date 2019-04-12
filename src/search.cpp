#include <search.hpp>
#include <errno.h>
#include <sys/types.h>
#include "resolve.hpp"

#include <fstream>
#include <iostream>

Search::Search()
{
}

Search::~Search()
{
}

bool Search::SetPosition(const double *curPosition)
{
  Position[0] = curPosition[0];
  Position[1] = curPosition[1];
  OGRPosition.setX(curPosition[0]);
  OGRPosition.setY(curPosition[1]);
  OGRPosition.setZ(0);

  return true;
}

bool Search::SetRadius(const double curradius)
{
  radius = curradius;

  return true;
}

bool Search::SetPathID(const std::vector<OID> pathIDs_parsed)
{
  pathIDs = pathIDs_parsed;

  return true;
}

bool Search::SetHLaneID(const std::vector<OID> HLaneIDs_parsed)
{
  HLaneIDs = HLaneIDs_parsed;

  return true;
}


bool Search::loadMap(void)
{
  DataManager *pDataManager = DataManager::getInstance();
  Resolve *resolver = Resolve::getInstance();

  bool breval = pDataManager->useDB(resolver->getMAPname());
  if(true == breval){
  	pDataManager->loadData();

  	return true;
  }

  return false;
}

void Search::setParkLotID(int ID)
{
  parkinglotID = ID;
}

bool Search::findParkingLot(void)
{
  DataManager *pDataManager = DataManager::getInstance();

  // get layer by layer name
  ILayer *pLayer = pDataManager->getLayer("parkings");
  if(pLayer)
  {
    printf("layer name: %s,feature count:%d\n",pLayer->GetName().c_str(),(int)pLayer->GetCount());

    //get feature
    IFeature *pFeature = pLayer->GetFeature(parkinglotID);
    while(pFeature)
    {
      //get ogrfeature
      OGRFeature *pOGRFeature = pFeature->GetOGRFeatureRef();
      if(pOGRFeature)
      {
        //get field
        parkingLot.PSID = pOGRFeature->GetFieldAsInteger("PSID");
        parkingLot.PBGID = pOGRFeature->GetFieldAsInteger("PBGID");
        parkingLot.HPTYPE = pOGRFeature->GetFieldAsInteger("HPTYPE");
        parkingLot.PARKCODE = pOGRFeature->GetFieldAsInteger("PARKCODE");
        parkingLot.LHEIGHT = pOGRFeature->GetFieldAsInteger("LHEIGHT");
        parkingLot.DTYPE = pOGRFeature->GetFieldAsInteger("DTYPE");
        parkingLot.LINKID = pOGRFeature->GetFieldAsInteger("LINKID");
        // printf("PSID: %d\n",parkingLot.PSID);
        // printf("PBGID: %d\n",parkingLot.PBGID);
        // printf("HPTYPE: %d\n",parkingLot.HPTYPE);
        // printf("PARKCODE: %d\n",parkingLot.PARKCODE);
        // printf("LHEIGHT: %d\n",parkingLot.LHEIGHT);
        // printf("DTYPE: %d\n",parkingLot.DTYPE);
        // printf("LINKID: %d\n",parkingLot.LINKID);
        // printf("-----------\n");
      }
      else
      {
        printf("wrong OGRFeature, can't get OGRFeature: %m\n");
        return false;
      }
      //get geometry
      OGRGeometry *pGeometry = pFeature->GetOGRGeometry();
      if(pGeometry)
      {
        //OGRPolygon *pPos = (OGRPolygon *)pGeometry;
        //OGRLinearRing *exRing = (OGRLinearRing *)pPos->getExteriorRing();
        //double area = exRing->isClockwise();

        return true;
      }
      else
      {
        printf("wrong OGRGeometry, can't get OGRGeometry: %m\n");
        return false;
      }
    }
    printf("wrong parkingID, can't get feature: %m\n");
    return false;
  }
  printf("wrong parkings layer, can't get layer: %m\n");
  return false;
}

bool Search::findPathLine(void)
{
  DataManager *pDataManager = DataManager::getInstance();

  ILayer *pLayer = pDataManager->getLayer("pathline");

  if(pLayer)
  {
    printf("layer name: %s,feature count:%d\n",pLayer->GetName().c_str(),(int)pLayer->GetCount());

    //get feature
    // IFeature *pFeature = pLayer->GetFeature(1100000046);
    IFeature *pFeature = pLayer->GetFirst();

    while(pFeature)
    {
      //get ogrfeature
      OGRFeature *pOGRFeature = pFeature->GetOGRFeatureRef();
      if(pOGRFeature)
      {
        //get field
        pathLine.LINKID = pOGRFeature->GetFieldAsInteger("LINKID");
        pathLine.TYPE = pOGRFeature->GetFieldAsInteger("TYPE");
        pathLine.DT = pOGRFeature->GetFieldAsInteger("DT");
        pathLine.LENGTH = pOGRFeature->GetFieldAsInteger("LENGTH");
        pathLine.FJCID = pOGRFeature->GetFieldAsInteger("FJCID");
        pathLine.TJCID = pOGRFeature->GetFieldAsInteger("TJCID");
        pathLine.WIDTH = pOGRFeature->GetFieldAsInteger("WIDTH");
        pathLine.PBGID = pOGRFeature->GetFieldAsInteger("PBGID");
        printf("LINKID: %d\n",pathLine.LINKID);
        // printf("TYPE: %d\n",pathLine.TYPE);
        // printf("DT: %d\n",pathLine.DT);
        // printf("LENGTH: %d\n",pathLine.LENGTH);
        // printf("FJCID: %d\n",pathLine.FJCID);
        // printf("TJCID: %d\n",pathLine.TJCID);
        // printf("WIDTH: %d\n",pathLine.WIDTH);
        // printf("PBGID: %d\n",pathLine.PBGID);
       
        printf("-----------\n");
      }
      //get geometry
      OGRGeometry *pGeometry = pFeature->GetOGRGeometry();
      if(pGeometry)
        {
          OGRLineString *pLine = (OGRLineString *)pGeometry;
          for(int i = 0; i< pLine->getNumPoints(); i++)
          {
            //to do
            // printf("%.6f,%.6f,%.2f\n",pLine->getX(i),pLine->getY(i),pLine->getZ(i));
          }
        }
      // pFeature = 0;
      pFeature = pLayer->GetNext();
      // return true;
    }
  }
  return false;
}

bool Search::findObstacle(void)
{
  std::vector<IFeature *> vFeatures;

  DataManager *pDataManager = DataManager::getInstance();

  // get layer by layer name
  ILayer *pLayer = pDataManager->getLayer("obstacle");
  if(pLayer)
  {
    printf("layer name: %s,feature count:%d\n",pLayer->GetName().c_str(),(int)pLayer->GetCount());

    //get feature
    pLayer->getFeaturesByPt(OGRPosition, radius, vFeatures);

    while(vFeatures.size())
    {
      //get ogrfeature
      OGRFeature *pOGRFeature = vFeatures[0]->GetOGRFeatureRef();
      if(pOGRFeature)
      {
        //get field
        obsInfo.OBDID = pOGRFeature->GetFieldAsInteger("OBDID");
        obsInfo.TYPE = pOGRFeature->GetFieldAsInteger("TYPE");
        obsInfo.LINKIDS = pOGRFeature->GetFieldAsInteger("LINKIDS");
        obsInfo.AHEIGHT = pOGRFeature->GetFieldAsInteger("AHEIGHT");
        obsInfo.PBGID = pOGRFeature->GetFieldAsInteger("PBGID");
        // printf("OBDID: %d\n",obsInfo.OBDID);
        // printf("TYPE: %d\n",obsInfo.TYPE);
        // printf("LINKIDS: %d\n",obsInfo.LINKIDS);
        // printf("AHEIGHT: %d\n",obsInfo.AHEIGHT);
        // printf("PBGID: %d\n",obsInfo.PBGID);
        // printf("-----------\n");
      }
      //get geometry
      OGRGeometry *pGeometry = vFeatures[0]->GetOGRGeometry();
      if(pGeometry)
      {
        //OGRPolygon *pPos = (OGRPolygon *)pGeometry;
        //OGRLinearRing *exRing = pPos->getExteriorRing();
        //OGRPoint *start;
        //exRing->StartPoint(start);
        // double x = start->getZ();

        return true;
      }
      else
      {
        printf("wrong OGRGeometry, can't get OGRGeometry: %m\n");
        return false;
      }
    }
    printf("wrong Position or radius, can't get obstacle: %m\n");
    return false;
  }
  printf("wrong obstacle layer, can't get layer: %m\n");
  return false;
}

bool Search::findHLane(void)
{
  DataManager *pDataManager = DataManager::getInstance();

  ILayer *pLayer = pDataManager->getLayer("HLane");

  if(buildHLane(pLayer))
  {
    getNearest(Position);
    getNPointsFromHere(30);
    return true;
  }
  else
  {
    return false;
  }
}


bool Search::buildHLane(ILayer *pLayer)
{  
  if(pLayer)
  {
    printf("layer name: %s,feature count:%d\n",pLayer->GetName().c_str(),(int)pLayer->GetCount());

    // buildHLaneFromPathline(pLayer);
    buildHLaneFromHLaneID(pLayer);

    if (vHLaneFeatures.size())
    {
      buildHLaneGeneralInfo();
      buildNavPointsKDtree();

      return true;
    }
    else
    {
      printf("wrong vHLaneFeatures, can't get vHLaneFeatures: %m\n");
      return false;
    }
  }
  printf("wrong HLane layer, can't get layer: %m\n");
  return false;
}

bool Search::buildHLaneFromHLaneID(ILayer *pLayer)
{
  DataManager *pDataManager = DataManager::getInstance();

  printf("layer name: %s,feature count:%d\n",pLayer->GetName().c_str(),(int)pLayer->GetCount());

  for(vector<OID>::const_iterator iter = HLaneIDs.begin(); iter != HLaneIDs.end(); iter++)
  {
    IFeature *pFeature = pLayer->GetFeature(*iter);
    
    vHLaneFeatures.push_back(pFeature);
  }

  if (vHLaneFeatures.size())
  {
    return true;
  }

  return false;

}

bool Search::buildHLaneFromPathline(ILayer *pLayer)
{
  DataManager *pDataManager = DataManager::getInstance();
  DataBase *pDataBase = pDataManager->getDBHandle();

  char sql[100] = {'0'};
  IFeature* pFeature;

  for(vector<OID>::const_iterator iter = pathIDs.begin(); iter != pathIDs.end(); iter++)
  {
    memset(sql,'0',100);
    sprintf(sql,"select HLaneID from HLane where LINKID = %ld", (*iter));

    pDataBase->ExcuteQuery(sql);

    while(pDataBase->Read())
    {
      OID laneid = pDataBase->GetInt64Value(0);
      pFeature = pLayer->GetFeature(laneid);
      vHLaneFeatures.push_back(pFeature);
    }
  }

  if (vHLaneFeatures.size())
  {
    return true;
  }

  return false;
}

bool Search::drawWholeHLane(void)
{
  DataManager *pDataManager = DataManager::getInstance();

  ILayer *pLayer = pDataManager->getLayer("HLane");

  std::ofstream HLpoints("HLpoints.log");

  if(pLayer)
  {
    printf("layer name: %s,feature count:%d\n",pLayer->GetName().c_str(),(int)pLayer->GetCount());

    IFeature *pFeature = pLayer->GetFirst();

    while(pFeature)
    {
      OGRGeometry *pGeometry = pFeature->GetOGRGeometry();
      OGRFeature *pOGRFeature = pFeature->GetOGRFeatureRef();

      if(pGeometry)
      {
        OGRLineString *pLine = (OGRLineString *)pGeometry;
        // printf("ID: %ld\n",pOGRFeature->GetFieldAsInteger("LINKID"));

        for(int j = 0; j < pLine->getNumPoints(); j++)
        {
          // printf("%.7f %.7f\n", pLine->getX(j), pLine->getY(j));
          HLpoints << pLine->getX(j) << " " << pLine->getY(j) << std::endl;
        }
      }
      pFeature = pLayer->GetNext();
    }
  }

  HLpoints.close();
  return false;
}

bool Search::buildHLaneGeneralInfo(void)
{
  int count = vHLaneFeatures.size();

  for(int i = 0; i < count; i++)
  {
    OGRFeature *pOGRFeature = vHLaneFeatures.at(i)->GetOGRFeatureRef();
    if(pOGRFeature)
    {
      if(buildHLaneStruct(pOGRFeature))
      {
        return true;
      }
      else
      {
        printf("can not build HLane struct: %m\n");
        return false;
      }
    }
    else
    {
      printf("wrong OGRFeature, can't get OGRFeature: %m\n");
      return false;
    }
  }
}

bool Search::buildHLaneStruct(OGRFeature *pOGRFeature)
{
  hLane.HLaneID = pOGRFeature->GetFieldAsInteger64(0);
  hLane.SLNodeID = pOGRFeature->GetFieldAsInteger("SLNodeID");
  hLane.ELNodeID = pOGRFeature->GetFieldAsInteger("ELNodeID");
  hLane.PBGID = pOGRFeature->GetFieldAsInteger("PBGID");
  hLane.LWIDTH = pOGRFeature->GetFieldAsInteger("LWIDTH");
  hLane.LINKID = pOGRFeature->GetFieldAsInteger("LINKID");
  hLane.HGNodeID = pOGRFeature->GetFieldAsInteger("HGNodeID");
  hLane.LRBID = pOGRFeature->GetFieldAsInteger("LRBID");
  hLane.RRBID = pOGRFeature->GetFieldAsInteger("RRBID");
  hLane.HLPType = pOGRFeature->GetFieldAsInteger("HLPType");

  return true;
}

bool Search::buildNavPointsKDtree(void)
{
  int count = vHLaneFeatures.size();
  double pos[3];
  int index = 0;
  int totalNumber = 0;
  HLaneTree = kd_create(2);

  for(int i = 0; i < count; i++)
  {
    OGRGeometry *pGeometry = vHLaneFeatures.at(i)->GetOGRGeometry();
    if(pGeometry)
    {
      OGRLineString *pLine = (OGRLineString *)pGeometry;
      for(int j = 0; j < pLine->getNumPoints(); j++)
      {
        pos[0] = pLine->getX(j);
        pos[1] = pLine->getY(j);
        index = j + totalNumber;
        printf("%.7f %.7f\n", pos[0], pos[1]);
        kd_insert(HLaneTree, pos, index);
      }

      totalNumber += pLine->getNumPoints();
    }
    else
    {
      printf("wrong OGRGeometry, can't get OGRGeometry: %m\n");
      return false;
    }
  }

  return true;
}

bool Search::getNearest(const double *curPos)
{
  double pos[3];
  double dist;
  struct kdres *presults;
  presults = kd_nearest(HLaneTree, curPos);

  while( !kd_res_end( presults ) ) {
    kd_res_item( presults, pos );
    /* compute the distance of the current result from the pt */
    dist = sqrt( getDistBetXY( curPos, pos) );
    /* print out the retrieved data */
    printf( "node at (%.7f, %.7f, %.1f) is %.9f away, our curPOS: (%.7f, %.7f, %.1f)\n", pos[0], pos[1], pos[2],  dist, curPos[0], curPos[1], curPos[2]);
    memcpy(nearHDPoint,pos,3);
    /* go to the next entry */
    kd_res_next( presults );
  }
  kd_res_free( presults );

  if(dist < 150.0) 
  {
    printf("GOT the nearest point\n");
    return true;
  } 
  else 
  {
    return false;
  }
}

bool Search::getNPointsFromHere(const int number)
{
  OGRPoint point;

  for(int i = 0; i < number; i++)
  {
    point.setX(nearHDPoint[0]+i);
    point.setY(nearHDPoint[1]+i);
    vPoints.push_back(point);
  }

  return true;
}

std::vector<OGRPoint> Search::getHLanePoints(void)
{
  std::unique_lock<std::mutex> lockTmp(SEAmutex);
  return vPoints;
}





