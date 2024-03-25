#include "app/framework/include/af.h"
#include "zigbee-framework/zigbee-device-library.h"
#include "Source/Mid/button/button.h"
#include "Source/App/Network/Network.h"
#include "Source/App/Receive/Receive.h"
#include "Source/Mid/Led/led.h"
#include "stdint.h"
#include "stdbool.h"
#include "Send.h"

#define MAX_DATA_COMMAND_SIZE 		500
#define SOURCE_ENDPOINT_PRIMARY 	1
#define DESTINATION_ENDPOINT 		1
#define HC_NETWORK_ADDRESS 			0x0000

#define ZDO_MESSAGE_OVERHEAD 		100

/*
 * func: SEND_ReportInfoToHc
 * brief: Ham send thong tin cua thiet bi len ZC de tao icon tren app lumi life
 * param: NONE
 * retval: NONE
 */

void SEND_ReportInfoToHc(void)					//Ham gui ban tin report khi moi khoi dong len
{
//	uint8_t modelID[17] = {16,'S','W','1','_','S','W','2','_','P','I','R','1','_','L','M','1'};
	uint8_t modelID[13] = {12,'S','W','2','_','P','I','R','1','_','L','M','1'};
//	uint8_t modelID[4] = {3,'S','W','1'};
	uint8_t manufactureID[5] = {4,'L','u','m','i'};
	uint8_t version = 1;

	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,(uint16_t*)modelID,13,ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,DESTINATION_ENDPOINT,HC_NETWORK_ADDRESS);

	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,ZCL_MANUFACTURER_NAME_ATTRIBUTE_ID,ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,(uint16_t*)manufactureID,5,ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,DESTINATION_ENDPOINT,HC_NETWORK_ADDRESS);

	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,ZCL_APPLICATION_VERSION_ATTRIBUTE_ID,ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,(uint16_t*)&version,1,ZCL_BOOLEAN_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,DESTINATION_ENDPOINT,HC_NETWORK_ADDRESS);
}

/*
 * func: SEND_FillBufferGlobalCommand
 * brief: Ham dien cac tham so nhu cluster id, data send, len data, ... vao goi data gui di
 * param:
 * 		clusterID: Cluster id muon dung
 * 		attributeId: Attribute id muon dung
 * 		globalCommand: Global command
 * 		*value: Data muon gui
 * 		length: Chieu dai data muon gui
 * 		dataType: loai du lieu cua data se gui
 * retval: NONE
 */

void SEND_FillBufferGlobalCommand(EmberAfClusterId clusterID, EmberAfAttributeId attributeId, uint8_t globalCommand, uint16_t *value, uint8_t length, uint8_t dataType)
{
	uint8_t data[MAX_DATA_COMMAND_SIZE];
	data[0] = (uint8_t)(attributeId & 0x00FF);
	data[1] = (uint8_t)((attributeId & 0x00FF) >> 8);
	data[2] = EMBER_SUCCESS;
	data[3] = (uint8_t)dataType;

	memcpy(&data[4],value,length);

	(void)emberAfFillExternalBuffer((ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),clusterID,globalCommand,"b",data,(length + 4));
}

/*
 * func: SEND_SendCommandUnicast
 * brief: Ham gui du lieu theo kieu Unicat trong mang
 * param:
 * 		source: Endpoint cua thiet bi gui
 * 		destination: Endpoint cua thiet bi dich
 * 		address: Dia chi cua thiet bi nhan
 * retval: NONE
 */

void SEND_SendCommandUnicast(uint8_t source, uint8_t destination, uint16_t address)
{
	emberAfSetCommandEndpoints(source,destination);
	(void)emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT,address);
}

/*
 * func: SEND_OnOffStateReport
 * brief: Ham gui gia tri on/off cho thiet bi dich
 * param:
 * 		endpoint Endpoint cua thiet bi gui
 * 		value: gia tri can gui
 * retval: NONE
 */

void SEND_OnOffStateReport(uint8_t endpoint_source, uint8_t endpoint_destination, uint8_t value)
{
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}
	SEND_FillBufferGlobalCommand(ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID,ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,(uint16_t*)&value,1,ZCL_BOOLEAN_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(endpoint_source,endpoint_destination,HC_NETWORK_ADDRESS);
}

/*
 * func: SEND_PIRStateReport
 * brief: Ham gui gia tri cua cam bien chuyen dong PIR cho thiet bi ZC
 * param:
 * 		endpoint_source: Endpoint cua thiet bi gui
 * 		endpoint_destination: Endpoint cua thiet bi dich
 * 		status: gia tri can gui
 * retval: NONE
 */

void SEND_PIRStateReport(uint8_t endpoint_source, uint8_t endpoint_destination, uint8_t value)
{
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}
	SEND_FillBufferGlobalCommand(ZCL_IAS_ZONE_CLUSTER_ID,ZCL_STACK_VERSION_ATTRIBUTE_ID,ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,(uint16_t*)&value,1,ZCL_BITMAP8_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(endpoint_source,endpoint_destination,HC_NETWORK_ADDRESS);
}

/*
 * func: SEND_LDRValue
 * brief: Ham gui gia tri cua cam bien do anh sang cho thiet bi ZC
 * param:
 * 		endpoint_source: Endpoint cua thiet bi gui
 * 		endpoint_destination: Endpoint cua thiet bi dich
 * 		status: gia tri can gui
 * retval: NONE
 */

void SEND_LDRValue(uint8_t endpoint_source, uint8_t endpoint_destination, uint16_t value)
{
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}
	SEND_FillBufferGlobalCommand(ZCL_ILLUM_MEASUREMENT_CLUSTER_ID,ZCL_VERSION_ATTRIBUTE_ID,ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,(uint16_t*)&value,2,ZCL_INT16U_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(endpoint_source,endpoint_destination,HC_NETWORK_ADDRESS);
}

void SEND_Data(uint16_t address, uint8_t endpoint_source, uint8_t endpoint_destination, uint8_t *value)
{
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}
//	SEND_FillBufferGlobalCommand(ZCL_ILLUM_MEASUREMENT_CLUSTER_ID,ZCL_VERSION_ATTRIBUTE_ID,ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,(uint16_t*)&value,2,ZCL_INT16U_ATTRIBUTE_TYPE);
//	SEND_SendCommandUnicast(endpoint_source,endpoint_destination,address);
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,(uint8_t*)value,10,ZCL_INT8U_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(endpoint_source,endpoint_destination,address);
}

