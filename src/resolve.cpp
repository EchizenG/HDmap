#include "resolve.hpp"
#include "errno.h"

Resolve::Resolve()
{}

Resolve::~Resolve()
{}

bool Resolve::parseJSON(const std::string json, boost::property_tree::ptree& messages)
{
  //std::basic_istream<typename boost::property_tree::ptree::key_type::value_type> stream(json);
  std::stringstream stream(json);
  boost::property_tree::json_parser::detail::read_json_internal(stream, messages, std::string());
  return true;
}

bool Resolve::stringifyJSON(std::stringstream &sendData, const boost::property_tree::ptree pt)
{
  //boost::property_tree::json_parser::detail::write_json_internal(sendData, pt, std::string(), true);
  return true;
}


