#ifndef _RESOLVE_HPP
#define _RESOLVE_HPP

#include <iostream>
#include <string>
#include "boost/property_tree/json_parser/detail/read.hpp"
#include "boost/property_tree/ptree.hpp"


class Resolve
 {
 public:
 	Resolve();

 	bool parseJSON(const std::string json, boost::property_tree::ptree& messages);
	bool stringifyJSON(std::stringstream &sendData, const boost::property_tree::ptree pt);
	
 	~Resolve();
 	
 };
#endif //define _RESOLVE_HPP