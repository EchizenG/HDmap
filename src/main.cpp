#include <errno.h>
#include <stdio.h>
#include "communication.hpp"
#include "search.hpp"
#include "resolve.hpp"
#include "HDmap.hpp"

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

/*THIS IS FOR JSON
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
*/
enum msg_type
{
  OBU_MSG_START_PARK_NOTIFY =0x0,	// 0x0启动泊车通知
	OBU_MSG_MAP_REQ,		     		    // 0x1地图文件请求
	OBU_MSG_MAP_RSP,     		  		  // 0x2地图文件响应
	OBU_MSG_PARKING_SPACE_REQ,	    // 0x3申请停车位请求
	OBU_MSG_PARKING_SPACE_RSP,      // 0x4申请停车位响应
	OBU_MSG_GLOBAL_PATH_REQ, 		    // 0x5 全局路径规划请求
  OBU_MSG_GLOBAL_PATH_RSP, 		    // 0x6 全局路径规划响应
  OBU_MSG_CAR_POS_REQ,	    	  	// 0x7 车辆位置请求
  OBU_MSG_CAR_POS_RSP,	    		  // 0x8 车辆位置响应
  OBU_MSG_CAR_POS_NOTIFY,	    	  // 0x9 车辆位置通知
  OBU_MSG_BARRIERS_NOTIFY,			  // 0xA 障碍物列表通知
  OBU_MSG_APA_STATUS_NOTIFY,		  // 0xB APA状态通知
  OBU_MSG_APA_COMMAND			        // 0xC APA命令下发
};

int main(int argc, char **argv)
{
	
	HDmap map_handler;

	map_handler.setIP((char *)"192.168.3.191");
	map_handler.setPORT(5050);

	map_handler.connectTCP();
	struct Communication::msg_header head = map_handler.getHEAD();
	printf("head: %d\n", head.msg_type);
	printf("body_len: %d\n", head.body_len);


	switch(head.msg_type)
	{
		case OBU_MSG_START_PARK_NOTIFY:

		break;
		case OBU_MSG_MAP_REQ:

		break;
		case OBU_MSG_MAP_RSP:
		map_handler.getMAP(head);

		break;
		case OBU_MSG_PARKING_SPACE_REQ:
		break;
		case OBU_MSG_PARKING_SPACE_RSP:

		break;
		case OBU_MSG_GLOBAL_PATH_REQ:

    break;
    case OBU_MSG_GLOBAL_PATH_RSP:

    break;
    case OBU_MSG_CAR_POS_REQ:

    break;
    case OBU_MSG_CAR_POS_RSP:

    break;
    case OBU_MSG_CAR_POS_NOTIFY:

    break;
    case OBU_MSG_BARRIERS_NOTIFY:

    break;
    case OBU_MSG_APA_STATUS_NOTIFY:

    break;
    case OBU_MSG_APA_COMMAND:

    break;
	}

	return 0;
}
