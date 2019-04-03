#ifndef _RESOLVE_HPP
#define _RESOLVE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "boost/property_tree/ptree.hpp"



class Resolve
{
public:

	Resolve();
	~Resolve();

	bool parseJSON(const std::string json, boost::property_tree::ptree& messages);
	bool stringifyJSON(std::stringstream &sendData, const boost::property_tree::ptree pt);
	bool getMAP(char *bodybuffer);
	bool parseStartParkingInfo(char *data);
	bool parseParkingLotInfo(char *data);
	bool parsePath(char *data);
	bool parsePos(char *data);
	bool parseObjects(char *data);
	bool parseAPAStatus(char *data);


	size_t setMAPreq(char *data);
	size_t setLOTreq(char *data);
	size_t setPATHreq(char *data);

	std::vector<int64_t> getPathIDs(void);

private:
	
	std::vector<int64_t> pathIDs;

	char vehicleID[5];
	int parkLot_length, parkLot_width, parkLot_height;
	double pos_X, pos_Y, pos_Z, pos_heading, pos_speed;
	
};
#endif //define _RESOLVE_HPP