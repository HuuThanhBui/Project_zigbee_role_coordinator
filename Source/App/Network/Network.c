#include "Network.h"
#include "stdint.h"
#include "stdbool.h"
#include "em_chip.h"
#include "app/framework/include/af.h"
#include "zigbee-framework/zigbee-device-library.h"
#include "Source/App/Receive/Receive.h"
#include "Source/App/Network/Network.h"
#include "Source/App/Send/Send.h"
#include "Source/Mid/Led/led.h"

#define CHANNEL_MASK_DEFAULT 0x0318C800

EmberEventControl joinNetworkEventControl;
EmberEventControl MTORRsEventControl;

network_init func_init_network = NULL;
var_proc_t var_proc;

/*
 * func: NETWORK_Init
 * brief: Ham khoi tao ham callback de xu ly su kien mang zigbee
 * param:
 * 		nwkInitCallbackFunc: Ham callback xu ly su kien network zigbee
 * retval: NONE
 */

void NETWORK_Init(network_init nwkInitCallbackFunc)
{
	if(nwkInitCallbackFunc != NULL)
	{
		func_init_network = nwkInitCallbackFunc;
	}
	networkSetChannelMask(CHANNEL_MASK_DEFAULT);
}

/*
 * func: networkSetChannelMask
 * brief: Ham khoi tao tham so steering
 * param:
 * 		mask_default: gia tri mat na khi thuc hien steering
 * retval: NONE
 */

void networkSetChannelMask(uint32_t mask_default)
{
	emAfPluginNetworkSteeringSetChannelMask(mask_default,false);	//0x0318C800
	emberSetTxPowerMode(EMBER_TX_POWER_MODE_BOOST);
//	emberAfPluginNetworkSteeringStart();			//Tim mang
}

//boolean emberAfPreMessageReceivedCallback(EmberAfClusterCommand* cmd)
//{
//
//	return true;
//}

/*
 * func: emberAfStackStatusCallback
 * brief: Ham duoc goi khi trang thai mang zigbee bi thay doi
 * param:
 * 		status: trang thai mang zigbee duoc truyen vao boi stack zigbee
 * retval: status = 0
 */

bool emberAfStackStatusCallback(EmberStatus status)				//Callback thong bao trang thai mang cua he thong
{
	emberAfCorePrintln("Main_NetworkCallbackHandler");
	if (status == EMBER_NETWORK_UP)					//Khi he thong da co mang
	{
		if(var_proc.timeFindAndJoin > 0)
		{
//			NETWORK_StopFindAndJoin();
//			NETWORK_FindAndJoin();
			if(func_init_network != NULL)
			{
				func_init_network(NETWORK_JOIN_SUCCESS);			//Khi he thong da vao mang thanh cong
			}
		}
		else
		{
			if(func_init_network != NULL)
			{
				func_init_network(NETWORK_HAS_PARENT);
			}
		}
	}
	else		//status = EMBER_NETWORK_DOWN
	{
		EmberNetworkStatus nwkStatusCurrent = emberAfNetworkState();
		if(nwkStatusCurrent == EMBER_NO_NETWORK)				//Khi he thong chua co mang
		{
			if(func_init_network != NULL)
			{
				func_init_network(NETWORK_OUT_NETWORK);
			}
		}
		else if(nwkStatusCurrent == EMBER_JOINED_NETWORK_NO_PARENT)
		{
			func_init_network(NETWORK_LOST_PARENT);
		}
	}
	return false;
}

/*
 * func: joinNetworkEventHandler
 * brief: Ham thuc hien join mang zigbee duoc tao tu ZC.
 * param: NONE
 * retval: NONE
 */

void joinNetworkEventHandler(void)				//Ham xu ly event check khong co network
{
	emberEventControlSetInactive(joinNetworkEventControl);
	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberAfPluginNetworkSteeringStart();
		var_proc.timeFindAndJoin++;
	}
}

/*
 * func: NETWORK_FindAndJoin
 * brief: Ham active event delay join mang zigbee
 * param: NONE
 * retval: NONE
 */

void NETWORK_FindAndJoin(void)					//Ham tim va ket noi voi mang da mo san
{
	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberEventControlSetDelayMS(joinNetworkEventControl,2000);
	}
}

//void emberIncomingManyToOneRouteRequestHandler(EmberNodeId source,EmberEUI64 longId,uint8_t cost)
//{
//	emberAfCorePrintln("Received MTORRs");
//	emberEventControlSetInactive(MTORRsEventControl);
//	emberEventControlSetDelayMS(MTORRsEventControl,(2*((uint8_t)halCommonGetRandom())));			//1 -> 2 minute
//}

void MTORRsEventHandler(void)
{
	uint8_t data;
	emberEventControlSetInactive(MTORRsEventControl);
	EmberAfStatus status = emberAfReadServerAttribute(1,ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID,&data,1);
	if(status == EMBER_ZCL_STATUS_SUCCESS)
	{
//		SEND_OnOffStateReport(1,data);
	}
}
