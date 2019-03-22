#include <errno.h>
#include <stdio.h>
#include "communication.hpp"
#include "search.hpp"
#include "resolve.hpp"

/*THIS IS FOR CAN
int main(int argc, char **argv)
{
	struct Communication::can_hdl *hdl;
	struct Communication::can_cfg cfg;
	struct can_frame rcvd_frame;
	struct can_frame sent_frame;
	int ret = 0;
	char interface[5];

	sprintf(interface,"can%d",argv[1]);
	cfg.ifname = interface;

	ret = Communication::can_open(&hdl, &cfg);

	if(0 != ret){
		printf("can not open can, errno: %d\n", errno);
	}

	Communication::can_read(hdl, &rcvd_frame);

	printf("rcvd_frame: %x:%s\n",rcvd_frame.can_id, rcvd_frame.data);

	sent_frame.can_id = 0x777;
	sent_frame.can_dlc = 8;
	sent_frame.data[0] = 0x00;
	sent_frame.data[1] = 0x01;
	sent_frame.data[2] = 0x02;
	sent_frame.data[3] = 0x03;
	sent_frame.data[4] = 0x04;
	sent_frame.data[5] = 0x05;
	sent_frame.data[6] = 0x06;
	sent_frame.data[7] = 0x07;
	Communication::can_write(hdl, &sent_frame);


	ret = Communication::can_close(&hdl);
	if(0 != ret){
		printf("can not close can, errno: %d\n", errno);
	}
}
*/
/*THIS IS FOR SEARCHING
int main(int argc, char **argv)
{
	char mapDirectory[30] = "./bin/oushangPark.sqlite";

	//Search::findParkingLot(parkingID,pDataManager);
	//Search::findPathLine(pDataManager);
	//Search::findObstacle(pDataManager);
	double curPos[2];
	curPos[0] = 84.0;
	curPos[1] = 92.0;

	double raidus = 5.0;
	// ID = 1100000005;

	Search searchTEST;
	searchTEST.loadMap(mapDirectory);
	searchTEST.SetPosition(curPos);
	searchTEST.SetRadius(raidus);
	searchTEST.SetPathID(1100000073);
	searchTEST.SetPathID(1100000074);
	searchTEST.SetPathID(1100000016);


	searchTEST.findHLane();

	return 0;
}
*/

int main(int argc, char **argv)
{
	
	Communication tcpCON;

	tcpCON.setIP((char *)"192.168.3.191");
	tcpCON.setPORT(5050);

	char jsondata[1000] = {'0'};

	tcpCON.connectTCP();
	int head = tcpCON.getHEAD();
	printf("head: %d\n", head);

	tcpCON.getBODY(jsondata);
	printf("jsondata: %s\n",jsondata);


	std::string inputJSON;
	inputJSON = jsondata;

	Resolve parser;
	boost::property_tree::ptree resolvedJSON;

	parser.parseJSON(inputJSON, resolvedJSON);
		
	boost::property_tree::ptree start_address;
	start_address = resolvedJSON.get_child("start_address");
	string addtype = start_address.get<string>("addressType");
	
	printf("addtype: %s\n", addtype.c_str());

	return 0;
}
