#ifndef _RESOLVE_HPP
#define _RESOLVE_HPP

#include <iostream>
#include <string>
#include "boost/property_tree/ptree.hpp"



class Resolve
{
public:
	Resolve();
	~Resolve();

	bool parseJSON(const std::string json, boost::property_tree::ptree& messages);
	bool stringifyJSON(std::stringstream &sendData, const boost::property_tree::ptree pt);
	bool getMAP(char *bodybuffer);
	bool getStartParkingInfo(char *data);
	bool getParkingLotInfo(char *data);

	size_t setMAPreq(char *data);
	size_t setLOTreq(char *data);
	size_t setPATHreq(char *data);

private:
	char vehicleID[5];
	int parkLot_length, parkLot_width, parkLot_height;
	
};
#endif //define _RESOLVE_HPP