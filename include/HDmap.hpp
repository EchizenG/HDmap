#ifndef _HDMAP_HPP_
#define _HDMAP_HPP_

#include <iostream>
#include <communication.hpp>

class HDmap : public Communication
{
public:
 	HDmap();
 	~HDmap();

	bool getMAP(struct connectInfo &conInfo, char *data);

};
#endif //define _HDMAP_HPP_