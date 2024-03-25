#include "stdint.h"
#include "stdbool.h"
#include "em_chip.h"
#include "app/framework/include/af.h"
#include "zigbee-framework/zigbee-device-library.h"
#include "Receive.h"
#include "Source/App/Network/Network.h"
#include "Source/App/Send/Send.h"
#include "Source/Mid/Led/led.h"

#define EMBER_APPLICATION_HAS_INCOMING_MANY_TO_ONE_ROUTE_REQUEST_HANDLER

callback_data_recive_over_zigbee callback_data_zigbee = NULL;
void RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* cmd);

/*
 * func: emberAfPreCommandReceivedCallback
 * brief: Ham precomand se duoc goi boi stack zigbee se duoc goi moi khi co cac ban tin cmd toi
 * param:
 * 		cmd: con tro luu tru du lieu cua goi tin cmd toi
 * retval:
 * 		Status = 0
 */

bool emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd)
{
	emberAfCorePrintln("\nEntry Handler Recive command !!!\n");
	if(cmd->clusterSpecific)
	{
		switch(cmd->apsFrame->clusterId)
		{
			case ZCL_ON_OFF_CLUSTER_ID:
				RECEIVE_HandleOnOffCluster(cmd);
				break;
			default:
				break;
		}
	}
	return 0;
}

/*
 * func: Handler_LED
 * brief: Ham xu ly cac trang thai LED dua vao endpoint dich cua ban tin den
 * param:
 * 		endPoint: Endpoint dich can su dung
 * 		commandID: Command ID su dung
 * retval: NONE
 */

void Handler_LED(uint8_t endPoint, uint8_t commandID)
{
	switch(commandID)
	{
		case ZCL_OFF_COMMAND_ID:
		{
			if(endPoint == 1)
			{
				emberAfCorePrintln("\nEntry Handler Turn Off LED_1!!!\n");
//				turnOffLed(LED_1);
				SEND_OnOffStateReport(1, 1, ZCL_OFF_COMMAND_ID);		//Gui trang thai LED_1 cho thiet bi ZC
			}
			else
			{
				emberAfCorePrintln("\nEntry Handler Turn Off LED_2!!!\n");
//				turnOffLed(LED_2);
				SEND_OnOffStateReport(2, 2, ZCL_OFF_COMMAND_ID);		//Gui trang thai LED_2 cho thiet bi ZC
			}
			break;
		}
		case ZCL_ON_COMMAND_ID:
		{
			if(endPoint == 1)
			{
				emberAfCorePrintln("\nEntry Handler Turn On LED_1!!!\n");
//				turnOnLed(LED_1,ledGreen);
				SEND_OnOffStateReport(1, 1, ZCL_ON_COMMAND_ID);		//Gui trang thai LED_1 cho thiet bi ZC
			}
			else
			{
				emberAfCorePrintln("\nEntry Handler Turn On LED_2!!!\n");
//				turnOnLed(LED_2,ledGreen);
				SEND_OnOffStateReport(2, 2, ZCL_ON_COMMAND_ID);		//Gui trang thai LED_2 cho thiet bi ZC
			}
			break;
		}
		default:
			break;
	}
}

/*
 * func: RECEIVE_HandleOnOffCluster
 * brief: Ham xu ly du lieu dieu khien tu ZC
 * param:
 * 		cmd: con tro luu tru du lieu cua goi tin cmd toi
 * retval: NONE
 */

void RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* cmd)
{
	uint8_t commandID = cmd->commandId;
	uint8_t endPoint = cmd->apsFrame->destinationEndpoint;

	if(cmd->type == EMBER_INCOMING_MULTICAST)		//Neu ban tin tu ZC gui toi la multicast thi goi ham Handler_LED_Multicast thuc hien xu ly trang thai LED dua vao endpoint, va Cluster ID
	{
		emberAfCorePrintln("\nType of data: MULTICAST\n");
		Handler_LED(endPoint,commandID);
	}
	if(cmd->type == EMBER_INCOMING_UNICAST)		//Neu ban tin tu ZC gui toi la Unicast thi thuc hien kiem tra xem trong khoan TIMER truoc do dia chi nay da duoc gui toi chua, neu co thi khong thuc hien nua, neu chua thi goi ham Handler_LED_Unicast thuc hien xu ly trang thai LED dua vao endpoint, va Cluster ID
	{
		emberAfCorePrintln("\nType of data: UNICAST  --  endpoint: %d\n",endPoint);
		Handler_LED(endPoint,commandID);
	}
}

void init_function_callback_recive_data_over_zigbee(callback_data_recive_over_zigbee callback)
{
	if(callback != NULL)
	{
		callback_data_zigbee = callback;
	}
}

bool emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incomingMessage)
{
	callback_data_zigbee(incomingMessage);
	return 0;
}

