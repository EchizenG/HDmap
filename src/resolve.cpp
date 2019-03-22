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
  printf("are you ok?errno: %d\n", errno);
  return true;
}

