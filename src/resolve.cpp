#include "resolve.hpp"
#include "errno.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <errno.h>
#include <unistd.h>

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

		mapfile << mapdata;
		mapfile.close();
	}

	return true;
}

size_t Resolve::setMAPreq(char *data)
{
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
	printf("we build json: \n%s\n", reqInfo.c_str());

	strncpy(data, reqInfo.c_str(), reqInfo.size() + 1); 

	return reqInfo.size();
}

size_t Resolve::setLOTreq(char *data)
{
	boost::property_tree::ptree pt_singleItem;

	char vehicleID[5] = "A111";

	pt_singleItem.put("vehicleID", vehicleID);

	std::stringstream ss;
	stringifyJSON(ss, pt_singleItem);

	std::string reqInfo = ss.str();
	printf("we build json: \n%ssize: %d", reqInfo.c_str(), reqInfo.size());

	strncpy(data, reqInfo.c_str(), reqInfo.size() + 1); 

	return reqInfo.size();
}

size_t Resolve::setPATHreq(char *data)
{
	boost::property_tree::ptree pt_singleItem;
	boost::property_tree::ptree pt_reqInfo;

	char vehicleID[5] = "A111";
	char src_addressType[15] = "entrance";
	int src_address = 1100000004;
	char dst_addressType[15] = "parkingSpace";
	int dst_address = 1100000153;

	pt_reqInfo.put("vehicleID", vehicleID);

	pt_singleItem.put("addressType", src_addressType);
	pt_singleItem.put("address", src_address);
	pt_reqInfo.add_child("srcAddress", pt_singleItem);
	pt_singleItem.clear();

	pt_singleItem.put("addressType", dst_addressType);
	pt_singleItem.put("address", dst_address);
	pt_reqInfo.add_child("dstAddress", pt_singleItem);
	pt_singleItem.clear();

	std::stringstream ss;
	stringifyJSON(ss, pt_reqInfo);

	std::string reqInfo = ss.str();
	printf("we build json: \n%ssize: %d", reqInfo.c_str(), reqInfo.size());

	strncpy(data, reqInfo.c_str(), reqInfo.size() + 1); 

	return reqInfo.size();
}