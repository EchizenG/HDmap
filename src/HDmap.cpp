#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <errno.h>
#include <unistd.h>


#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include "HDmap.hpp"
#include "resolve.hpp"

using namespace boost::archive::iterators;

static bool Base64Encode( const std::string & input, std::string * output );
static bool Base64Decode( const std::string & input, std::string * output );

HDmap::HDmap()
{}

HDmap::~HDmap()
{}

bool HDmap::getMAP(struct msg_header head)
{
      // snprintf(mapFileName,20, "/home/gaoyang/HiRain_Project/eMapGo/%s_%s", 
    //                                   mapTree_files.get<std::string>("file_name").c_str(),
    //                                   mapTree_files.get<std::string>("version").c_str());
    // printf("before file outtttttttttttttttttttttttt\n");

  std::string mapdata(head.body_len-18,'1');
  char mapFileName[21];
  char *data;
  data = new char[head.body_len];

  Resolve parseMap;
  boost::property_tree::ptree::iterator ite;
  boost::property_tree::ptree mapTree;
  boost::property_tree::ptree mapTree_result;
  boost::property_tree::ptree mapTree_files;
  boost::property_tree::ptree mapTree_oneMap;

  std::string mapTree_name, mapTree_size, mapTree_stream;

  if(read(conInfo.sHandler, data, head.body_len))
  {
    parseMap.parseJSON(data, mapTree);
    mapTree_result = mapTree.get_child("result");
    mapTree_files = mapTree_result.get_child("map_files");

    for(ite = mapTree_files.begin(); ite != mapTree_files.end(); ite++)
    {
      mapTree_oneMap = ite->second;
      mapTree_name = mapTree_oneMap.get<std::string>("file_name");
      mapTree_size = mapTree_oneMap.get<std::string>("file_size");
      mapTree_stream = mapTree_oneMap.get<std::string>("file_stream");
    }

    Base64Decode(mapTree_stream, &mapdata);

    int diff = mapdata.length() - std::stoi(mapTree_size);

    for(int i = 0; i < diff; i++)
    {
      mapdata.pop_back();
    }

    std::ofstream mapfile(mapTree_name);

    mapfile << mapdata;
    mapfile.close();

    delete data;
    return true;
  }
  else
  {
    printf("can not read mapfile stream, errno: %d\n", errno);

    delete data;
    return false;
  }
}

bool HDmap::codeMAP(struct connectInfo &conInfo, char *data)
{
  return true;
}


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