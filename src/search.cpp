#include <search.hpp>
#include <errno.h>
#include <sys/types.h>

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

bool Search::SetPathID(const OID curID)
{
  pathIDs.push_back(curID);

  return true;
}


bool Search::loadMap(char *mapDirectory)
{
  DataManager *pDataManager = DataManager::getInstance();

  bool breval = pDataManager->useDB(mapDirectory);
  if(true == breval){
  	pDataManager->loadData();
  	return true;
  }

  return false;
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
        printf("wrong OGRFeature, can't get OGRFeature, errno: %d\n", errno);
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
        printf("wrong OGRGeometry, can't get OGRGeometry, errno: %d\n", errno);
        return false;
      }
    }
    printf("wrong parkingID, can't get feature, errno: %d\n", errno);
    return false;
  }
  printf("wrong parkings layer, can't get layer, errno: %d\n", errno);
  return false;
}

bool Search::findPathLine(void)
{
  // get layer by layer name
  ILayer *pLayer = pDataManager->getLayer("pathline");
  if(pLayer)
  {
    printf("layer name: %s,feature count:%d\n",pLayer->GetName().c_str(),(int)pLayer->GetCount());

    //get feature
    IFeature *pFeature = pLayer->GetFeature(1100000046);
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
        printf("TYPE: %d\n",pathLine.TYPE);
        printf("DT: %d\n",pathLine.DT);
        printf("LENGTH: %d\n",pathLine.LENGTH);
        printf("FJCID: %d\n",pathLine.FJCID);
        printf("TJCID: %d\n",pathLine.TJCID);
        printf("WIDTH: %d\n",pathLine.WIDTH);
        printf("PBGID: %d\n",pathLine.PBGID);
       
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
            printf("%.6f,%.6f,%.2f\n",pLine->getX(i),pLine->getY(i),pLine->getZ(i));
          }
        }
      pFeature = 0;//pLayer->GetNext();
      return true;
    }
  }
  return false;
}

bool Search::findObstacle(void)
{
  std::vector<IFeature *> vFeatures;

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
        printf("wrong OGRGeometry, can't get OGRGeometry, errno: %d\n", errno);
        return false;
      }
    }
    printf("wrong Position or radius, can't get obstacle, errno: %d\n", errno);
    return false;
  }
  printf("wrong obstacle layer, can't get layer, errno: %d\n", errno);
  return false;
}

bool Search::findHLane(void)
{
  DataManager *pDataManager = DataManager::getInstance();

  ILayer *pLayer = pDataManager->getLayer("HLane");

  if(buildHLane(pLayer))
  {
    getNearest(Position);
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

    buildHLaneFromPathline(pLayer);

    if (vHLaneFeatures.size())
    {
      buildHLaneGeneralInfo();
      buildNavPointsKDtree();

      return true;
    }
    else
    {
      printf("wrong vHLaneFeatures, can't get vHLaneFeatures, errno: %d\n", errno);
      return false;
    }
  }
  printf("wrong HLane layer, can't get layer, errno: %d\n", errno);
  return false;
}

bool Search::buildHLaneFromPathline(ILayer *pLayer)
{
  DataManager *pDataManager = DataManager::getInstance();

  DataBase *pDataBase = pDataManager->getDBHandle();

  char sql[100] = {'0'};
  IFeature* pFeature;

  for(vector<OID>::const_iterator iter = pathIDs.cbegin(); iter != pathIDs.cend(); iter++)
  {
    memset(sql,'0',100);
    sprintf(sql,"select HLaneID from HLane where LINKID = %d", (unsigned int)(*iter));
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
        printf("can not build HLane struct, errno: %d\n", errno);
        return false;
      }
    }
    else
    {
      printf("wrong OGRFeature, can't get OGRFeature, errno: %d\n", errno);
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
        kd_insert(HLaneTree, pos, index);
      }

      totalNumber += pLine->getNumPoints();
    }
    else
    {
      printf("wrong OGRGeometry, can't get OGRGeometry, errno: %d\n", errno);
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
    printf( "node at (%.7f, %.7f, %.1f) is %.9f away\n", pos[0], pos[1], pos[2],  dist);
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

  for(int i; i < number; i++)
  {
    point.setX(nearHDPoint[2]+i);
    point.setY(nearHDPoint[2]+i);
    vPoints.push_back(point);
  }

  return true;
}





