#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"
#include "main.h"
#include "app/framework/include/af.h"
#include "zigbee-framework/zigbee-device-common.h"
#include "app/framework/plugin/find-and-bind-target/find-and-bind-target.h"
#include "app/framework/plugin/find-and-bind-initiator/find-and-bind-initiator.h"
#include "plugin/network-creator-security/network-creator-security.h"
#include "app/framework/plugin/network-creator/network-creator.h"
#include "app/framework/util/af-main.h"
#include "stack/include/message.h"
#include "stack/include/event.h"
#include "Source/Mid/button/button.h"
#include "Source/App/Network/Network.h"
#include "Source/App/Receive/Receive.h"
#include "Source/App/Send/Send.h"
#include "Source/Mid/Led/led.h"
#include "Source/Mid/LCD_20_4/LCD_I2C.h"
#include "Source/Driver/uart.h"
#include "Ringbuffer.h"

#define SIZE_OF_QUEUE   	300			//ThanhBH Add
#define FRAME_SOF 			0xB1
#define FRAME_ACK 			0x06
#define FRAME_NACK 			0x15
#define CXOR_INIT_VAL 		0xFF

typedef struct data_struct_zone_1 data_struct_zone_1_t;
struct data_struct_zone_1{
	uint8_t data_led[8];
	uint16_t data_lm75;
	uint8_t byte_1_lm75;
	uint8_t byte_2_lm75;
	uint16_t data_bh1750;
	uint8_t byte_1_bh1750;
	uint8_t byte_2_bh1750;
};

typedef struct data_struct_zone_2 data_struct_zone_2_t;
struct data_struct_zone_2{
	uint8_t data_led[8];
	uint8_t byte_1_pir;
	uint8_t byte_2_pir;
	uint16_t data_temp_dht11;
	uint8_t byte_1_temp_dht11;
	uint8_t byte_2_temp_dht11;
	uint16_t data_humi_dht11;
	uint8_t byte_1_humi_dht11;
	uint8_t byte_2_humi_dht11;
};

typedef struct data_struct_zone_3 data_struct_zone_3_t;
struct data_struct_zone_3{
	uint8_t data_led[8];
	uint16_t data_mq7;
	uint8_t byte_1_mq7;
	uint8_t byte_2_mq7;
};

typedef enum{
	UART_STATE_IDLE,
	RX_STATE_START_BYTE,
	RX_STATE_DATA_BYTES,
	UART_STATE_ACK_RECEIVED,
	UART_STATE_NACK_RECEIVED,
	UART_STATE_ERROR,
	RX_STATE_CXOR_BYTE,
	UART_STATE_DATA_RECEIVED,
	UART_STATE_RX_TIMEOUT
}status_revice_data;

typedef struct information_node_address information_node_address_t;
struct information_node_address{
	uint8_t status;
	int16_t num_zone;
	EmberNodeId address;
};

typedef struct {
	uint8_t cmdid;
	uint8_t type;
}frm_common_t;

uint8_t arr_mem_data[SIZE_OF_QUEUE] = {0};
static uint8_t byRxBuffer2[SIZE_OF_QUEUE] = {0};
static RingBuff ringbuff;
var_proc_t var_proc;
static uint8_t byRxBufState = RX_STATE_START_BYTE;
static uint8_t byIndexRxBuf = 0;
static uint8_t byCheckXorRxBuf = 0;

EmberEventControl TimerPrintTableAddressControl;
EmberEventControl TimerProcessDataControl;
EmberEventControl TimerSendStatusOverUARTControl;
EmberEventControl TimerSendStatusOverLCDControl;

information_node_address_t node_address[EMBER_ADDRESS_TABLE_SIZE];
static data_struct_zone_1_t data_zone_1;
static data_struct_zone_2_t data_zone_2;
static data_struct_zone_3_t data_zone_3;

void MAIN_ButtonHoldCallbackHandle(uint8_t button, uint8_t holdingEvent);		//Ham xu ly trang thai an giu cua nut nhan
void MAIN_ButtonPressCallbackHandle(uint8_t button, uint8_t pressAndHoldingEvent);		//Ham xu ly trang thai an nha cua nut nhan
void function_Recive_Data_Over_Zigbee(EmberAfIncomingMessage* incomingMessage);

void calcu_data_to_send(type_device_t device, action_t action, uint8_t command_id, uint8_t option, uint8_t sequen,uint8_t data_to_send[], uint8_t leng_of_data, uint8_t data[])
{
	uint8_t result_xor = 0;
	if(device == LED)
	{
		if(action == SET)
		{
			result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
			for(uint32_t i = 0; i < leng_of_data; i++)
			{
				result_xor = result_xor ^ data_to_send[i];
			}
			data[0] = FRAME_SOF; data[1] = LENG_BUFF_LED; data[2] = option; data[3] = command_id; data[4] = 0x01;
			data[5] = data_to_send[0]; data[6] = sequen; data[7] = result_xor;
		}
		else if(action == GET)
		{
			result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x02 ^ sequen);
			for(uint32_t i = 0; i < leng_of_data; i++)
			{
				result_xor = result_xor ^ data_to_send[i];
			}
			data[0] = FRAME_SOF; data[1] = LENG_BUFF_LED; data[2] = option; data[3] = command_id; data[4] = 0x02;
			data[5] = data_to_send[0]; data[6] = sequen; data[7] = result_xor;
		}
	}
	else if(device == LM75)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
	else if(device == BH1750)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
	else if(device == TEMP_DHT11)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
	else if(device == HUMI_DHT11)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
	else if(device == LM35)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
	else if(device == MQ7)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
	else if(device == PIR)
	{
		result_xor = (CXOR_INIT_VAL ^ option ^ command_id ^ 0x01 ^ sequen);
		for(uint32_t i = 0; i < leng_of_data; i++)
		{
			result_xor = result_xor ^ data_to_send[i];
		}
		data[0] = FRAME_SOF; data[1] = LENG_BUFF_SENSOR; data[2] = option; data[3] = command_id; data[4] = 0x01;
		data[5] = data_to_send[0]; data[6] = data_to_send[1]; data[7] = sequen; data[8] = result_xor;
	}
}

uint8_t table_convert_to_command_id(uint8_t number_led, uint8_t number_zone)
{
	name_led_t name_led = LED_NUM_1;
	name_zone_t name_zone = ZONE_NUM_1;
	if(number_led == 1){name_led = LED_NUM_1;}
	else if(number_led == 2){name_led = LED_NUM_2;}
	else if(number_led == 3){name_led = LED_NUM_3;}
	else if(number_led == 4){name_led = LED_NUM_4;}
	else if(number_led == 5){name_led = LED_NUM_5;}
	else if(number_led == 6){name_led = LED_NUM_6;}
	else if(number_led == 7){name_led = LED_NUM_7;}
	else if(number_led == 8){name_led = LED_NUM_8;}

	if(number_zone == 1){name_zone = ZONE_NUM_1;}
	else if(number_zone == 2){name_zone = ZONE_NUM_2;}
	else if(number_zone == 3){name_zone = ZONE_NUM_3;}

	switch(name_zone)
	{
		case ZONE_NUM_1:
		{
			if(name_led == LED_NUM_1) {return 0x02;}
			else if(name_led == LED_NUM_2) {return 0x03;}
			else if(name_led == LED_NUM_3) {return 0x04;}
			else if(name_led == LED_NUM_4) {return 0x05;}
			else if(name_led == LED_NUM_5) {return 0x06;}
			else if(name_led == LED_NUM_6) {return 0x07;}
			else if(name_led == LED_NUM_7) {return 0x08;}
			else if(name_led == LED_NUM_8) {return 0x09;}
			break;
		}
		case ZONE_NUM_2:
		{
			if(name_led == LED_NUM_1) {return 0x0D;}
			else if(name_led == LED_NUM_2) {return 0x0E;}
			else if(name_led == LED_NUM_3) {return 0x0F;}
			else if(name_led == LED_NUM_4) {return 0x10;}
			else if(name_led == LED_NUM_5) {return 0x11;}
			else if(name_led == LED_NUM_6) {return 0x12;}
			else if(name_led == LED_NUM_7) {return 0x13;}
			else if(name_led == LED_NUM_8) {return 0x14;}
			break;
		}
		case ZONE_NUM_3:
		{
			if(name_led == LED_NUM_1) {return 0x17;}
			else if(name_led == LED_NUM_2) {return 0x18;}
			else if(name_led == LED_NUM_3) {return 0x19;}
			else if(name_led == LED_NUM_4) {return 0x1A;}
			else if(name_led == LED_NUM_5) {return 0x1B;}
			else if(name_led == LED_NUM_6) {return 0x1C;}
			else if(name_led == LED_NUM_7) {return 0x1D;}
			else if(name_led == LED_NUM_8) {return 0x1E;}
			break;
		}
	}
	return 0xFF;
}

uint16_t calculator_hex_to_decimal(uint8_t value_1, uint8_t value_2)
{
	uint16_t sum_value = 0;
	char arr_temp[10] = {0};
	if(value_2 != 0)
	{
		sprintf(arr_temp,"%x%x",value_1,value_2);
	}
	else if(value_2 == 0)
	{
		sprintf(arr_temp,"%x",value_1);
	}
	sscanf(arr_temp,"%hx",&sum_value);
	return sum_value;
}

void reset_node_address(void)
{
	for(uint32_t i = 0; i < EMBER_ADDRESS_TABLE_SIZE; i++)
	{
		node_address[i].address = 0;
		node_address[i].status = 0;
		node_address[i].num_zone = -1;
	}
}

void add_node_address(EmberNodeId address_need_add)
{
	for(uint32_t i = 0; i < EMBER_ADDRESS_TABLE_SIZE; i++)
	{
		if((node_address[i].status == 1 && node_address[i].address == address_need_add) || address_need_add == 0xFFFF || address_need_add == 0x0000)
		{
			return;
		}
	}
	for(uint32_t i = 0; i < EMBER_ADDRESS_TABLE_SIZE; i++)
	{
		if(node_address[i].status == 0 && node_address[i].address == 0)
		{
			node_address[i].status = 1;
			node_address[i].address = address_need_add;
			return;
		}
	}
}

void remove_node_address(EmberNodeId address_need_remove)
{
	for(uint32_t i = 0; i < EMBER_ADDRESS_TABLE_SIZE; i++)
	{
		if(node_address[i].status == 1 && node_address[i].address == address_need_remove)
		{
			node_address[i].status = 0;
			node_address[i].address = 0;
		}
	}
}

void SendDataToRouterHandler(uint8_t num_led, uint8_t num_zone, uint8_t data_led)
{
	uint8_t data_send[10] = {0}, arr_data_led[2] = {0};
	for(uint8_t i = 0; i < EMBER_ADDRESS_TABLE_SIZE; i++)
	{
		if(node_address[i].status == 1)
		{
			arr_data_led[0] = data_led;
			calcu_data_to_send(LED, SET, table_convert_to_command_id(num_led, num_zone), num_zone, 0, arr_data_led, 1, data_send);
			SEND_Data(node_address[i].address, num_led, num_led, data_send);
		}
	}
}

void function_callback_data_usart_recive(char data[], uint32_t leng)
{
	int data_arr[4] = {0};
	emberAfCorePrint("Data func callback: %s  --  leng: %d\n",data,leng);
	sscanf(data,"%d %d %d",&data_arr[0], &data_arr[1], &data_arr[2]);
	emberAfCorePrint("Data send to router: %d %d %d\n",data_arr[0], data_arr[1], data_arr[2]);
	SendDataToRouterHandler(data_arr[0], data_arr[1], data_arr[2]);
}

/*
 * func: emberAfMainInitCallback
 * brief: Ham main se duoc stack zigbee goi dau tien khi he thong khoi dong len
 * param: NONE
 * retval: NONE
 */

void emberAfMainInitCallback(void)
{
	emberAfCorePrintln("emberAfMainInitCallback");
	reset_node_address();
	/******************** Create Network and Open *************************/
	emberAfCorePrintln("\nOpen network !!!\n");
	emberAfPluginNetworkCreatorNetworkForm(1, 0xABCD, 10, 11);
	emberAfPluginNetworkCreatorSecurityOpenNetwork();
	/**********************************************************************/
	ring_buff_init(&ringbuff, arr_mem_data, SIZE_OF_QUEUE);
	/************************** Cai dat phan cung *********************/
	buttonInit(MAIN_ButtonHoldCallbackHandle,MAIN_ButtonPressCallbackHandle);		//Khoi tao cac ham xu ly nut nhan
	init_function_callback_recive_data_over_zigbee(function_Recive_Data_Over_Zigbee);


	Init_LCD_I2C();
	lcd_i2c_init();
	lcd_i2c_clear();
	initUSART1(porta, 0, porta, 3, 9600, function_callback_data_usart_recive);
	/******************************************************************/

	/******************** Cai dat mang khoi tao trang thai *****************/

	/**********************************************************************/

	emberEventControlSetInactive(TimerPrintTableAddressControl);
	emberEventControlSetInactive(TimerProcessDataControl);
	emberEventControlSetInactive(TimerSendStatusOverUARTControl);
	emberEventControlSetInactive(TimerSendStatusOverLCDControl);
	emberEventControlSetDelayMS(TimerPrintTableAddressControl,5000);
	emberEventControlSetDelayMS(TimerSendStatusOverUARTControl,2000);
	emberEventControlSetDelayMS(TimerSendStatusOverLCDControl,2000);
	emberEventControlSetActive(TimerProcessDataControl);
}

void TimerSendStatusOverUART_Device_Zone_1(void)
{
	char data_colection[500] = {0}, buff_temp[50] = {0};
	uint16_t j = 0;
	uint8_t data_send[10] = {0}, k = 0;
	uint8_t data_led[] = {0x00, 0x00};
	uint8_t data_sensor[] = {0x00, 0x00};

	data_colection[j] = '0';		//Devie type: 0x01: Zigbee, 0x02: BLE
	j++;
	data_colection[j] = '1';		//Devie type: 0x01: Zigbee, 0x02: BLE
	j++;
	data_colection[j] = ' ';		//Devie type: 0x01: Zigbee, 0x02: BLE
	j++;

	data_led[0] = data_zone_1.data_led[0];
	calcu_data_to_send(LED, SET, CMD_ID_LED_1_ZONE_1, 1, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_1.data_led[1];
	calcu_data_to_send(LED, SET, CMD_ID_LED_2_ZONE_1, 1, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_1.data_led[2];
	calcu_data_to_send(LED, SET, CMD_ID_LED_3_ZONE_1, 1, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_1.data_led[3];
	calcu_data_to_send(LED, SET, CMD_ID_LED_4_ZONE_1, 1, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_1.data_led[4];
	calcu_data_to_send(LED, SET, CMD_ID_LED_5_ZONE_1, 1, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_1.data_led[5];
	calcu_data_to_send(LED, SET, CMD_ID_LED_6_ZONE_1, 1, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_1.data_led[6];
	calcu_data_to_send(LED, SET, CMD_ID_LED_7_ZONE_1, 1, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_1.data_led[7];
	calcu_data_to_send(LED, SET, CMD_ID_LED_8_ZONE_1, 1, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_sensor[0] = data_zone_1.byte_1_bh1750; data_sensor[1] = data_zone_1.byte_2_bh1750;
	calcu_data_to_send(BH1750, SET, CMD_ID_BH1750, 1, 0,data_sensor, 2, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_SENSOR; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_sensor[0] = data_zone_1.byte_1_lm75; data_sensor[1] = data_zone_1.byte_2_lm75;
	calcu_data_to_send(LM75, SET, CMD_ID_LM75, 1, 0,data_sensor, 2, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_SENSOR; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}
	data_colection[j] = '\0';
	USART1_send(USART1, data_colection, strlen(data_colection));
	USART1_send(USART1, "\n", 1);
}

void TimerSendStatusOverUART_Device_Zone_2(void)
{
	char data_colection[500] = {0}, buff_temp[50] = {0};
	uint16_t j = 0;
	uint8_t data_send[10] = {0}, k = 0;
	uint8_t data_led[] = {0x00, 0x00};
	uint8_t data_sensor[] = {0x00, 0x00};

	data_colection[j] = '0';		//Devie type: 0x01: Zigbee, 0x02: BLE
	j++;
	data_colection[j] = '1';		//Devie type: 0x01: Zigbee, 0x02: BLE
	j++;
	data_colection[j] = ' ';		//Devie type: 0x01: Zigbee, 0x02: BLE
	j++;

	data_led[0] = data_zone_2.data_led[0];
	calcu_data_to_send(LED, SET, CMD_ID_LED_1_ZONE_2, 2, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_2.data_led[1];
	calcu_data_to_send(LED, SET, CMD_ID_LED_2_ZONE_2, 2, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_2.data_led[2];
	calcu_data_to_send(LED, SET, CMD_ID_LED_3_ZONE_2, 2, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_2.data_led[3];
	calcu_data_to_send(LED, SET, CMD_ID_LED_4_ZONE_2, 2, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_2.data_led[4];
	calcu_data_to_send(LED, SET, CMD_ID_LED_5_ZONE_2, 2, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_2.data_led[5];
	calcu_data_to_send(LED, SET, CMD_ID_LED_6_ZONE_2, 2, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_2.data_led[6];
	calcu_data_to_send(LED, SET, CMD_ID_LED_7_ZONE_2, 2, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_2.data_led[7];
	calcu_data_to_send(LED, SET, CMD_ID_LED_8_ZONE_2, 2, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_led[0] = data_zone_2.byte_1_pir;
	calcu_data_to_send(PIR, SET, CMD_ID_PIR, 2, 0,data_led, 1, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_LED; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_sensor[0] = data_zone_2.byte_1_temp_dht11; data_sensor[1] = data_zone_2.byte_2_temp_dht11;
	calcu_data_to_send(TEMP_DHT11, SET, CMD_ID_TEMP_DHT11, 2, 0,data_sensor, 2, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_SENSOR; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}

	data_sensor[0] = data_zone_2.byte_1_humi_dht11; data_sensor[1] = data_zone_2.byte_2_humi_dht11;
	calcu_data_to_send(HUMI_DHT11, SET, CMD_ID_HUMI_DHT11, 2, 0,data_sensor, 2, data_send);
	for(uint8_t i = 0; i < LENG_PAYLOAD_SENSOR; i++)
	{
		k = sprintf(buff_temp,"%x ",data_send[i]);
		if(k <= 0){continue;}
		for(uint8_t a = 0; a < k; a++)
		{
			if(buff_temp[a] == '\0'){continue;}
			data_colection[j] = buff_temp[a];
			j++;
		}
	}
	data_colection[j] = '\0';
	USART1_send(USART1, data_colection, strlen(data_colection));
	USART1_send(USART1, "\n", 1);
}

void TimerSendStatusOverUARTHandler(void)
{
	typedef enum{
		Zone_1,
		Zone_2
	}send_timer_handler;
	static send_timer_handler send_handler = Zone_1;
	emberEventControlSetInactive(TimerSendStatusOverUARTControl);
	if(send_handler == Zone_1)
	{
		TimerSendStatusOverUART_Device_Zone_1();
		send_handler = Zone_2;
	}
	else if(send_handler == Zone_2)
	{
		TimerSendStatusOverUART_Device_Zone_2();
		send_handler = Zone_1;
	}
	emberEventControlSetDelayMS(TimerSendStatusOverUARTControl,3000);
}

void TimerSendStatusOverLCDHandler(void)
{
	char arr_send[30] = {0};
	typedef enum{
		ZONE_1,
		ZONE_2,
	}state_machine_zone;
	static state_machine_zone state_zone = ZONE_1;
	emberEventControlSetInactive(TimerSendStatusOverLCDControl);

	switch(state_zone)
	{
		case ZONE_1:
		{
			lcd_i2c_clear();
			lcd_i2c_put_cur(0,0);
			sprintf(arr_send,"%s:%d %s:%d TEMP:","LED1",data_zone_1.data_led[0],"LED2",data_zone_1.data_led[1]);
			lcd_i2c_send_string(arr_send);
			memset(arr_send,0,sizeof(arr_send));

			lcd_i2c_put_cur(1,0);
			sprintf(arr_send,"%s:%d %s:%d %d  ","LED3",data_zone_1.data_led[2],"LED4",data_zone_1.data_led[3],data_zone_1.data_lm75);
			lcd_i2c_send_string(arr_send);
			memset(arr_send,0,sizeof(arr_send));

			lcd_i2c_put_cur(2,0);
			sprintf(arr_send,"%s:%d %s:%d LUX:","LED5",data_zone_1.data_led[4],"LED6",data_zone_1.data_led[5]);
			lcd_i2c_send_string(arr_send);
			memset(arr_send,0,sizeof(arr_send));

			lcd_i2c_put_cur(3,0);
			sprintf(arr_send,"%s:%d %s:%d %d  ","LED7",data_zone_1.data_led[6],"LED8",data_zone_1.data_led[7],data_zone_1.data_bh1750);
			lcd_i2c_send_string(arr_send);
			memset(arr_send,0,sizeof(arr_send));
			state_zone = ZONE_2;
			break;
		}
		case ZONE_2:
		{
			lcd_i2c_clear();
			lcd_i2c_put_cur(0,0);
			sprintf(arr_send,"%s:%d %s:%d TEMP:","LED1",data_zone_2.data_led[0],"LED2",data_zone_2.data_led[1]);
			lcd_i2c_send_string(arr_send);
			memset(arr_send,0,sizeof(arr_send));

			lcd_i2c_put_cur(1,0);
			sprintf(arr_send,"%s:%d %s:%d %d  ","LED3",data_zone_2.data_led[2],"LED4",data_zone_2.data_led[3],data_zone_2.data_temp_dht11);
			lcd_i2c_send_string(arr_send);
			memset(arr_send,0,sizeof(arr_send));

			lcd_i2c_put_cur(2,0);
			sprintf(arr_send,"%s:%d %s:%d HUMI:","LED5",data_zone_2.data_led[4],"LED6",data_zone_2.data_led[5]);
			lcd_i2c_send_string(arr_send);
			memset(arr_send,0,sizeof(arr_send));

			lcd_i2c_put_cur(3,0);
			sprintf(arr_send,"%s:%d %s:%d %d  ","LED7",data_zone_2.data_led[6],"LED8",data_zone_2.data_led[7],data_zone_2.data_humi_dht11);
			lcd_i2c_send_string(arr_send);
			memset(arr_send,0,sizeof(arr_send));
			state_zone = ZONE_1;
			break;
		}
	}
	emberEventControlSetDelayMS(TimerSendStatusOverLCDControl,1000);
}

/*
 * func: MAIN_ButtonHoldCallbackHandle
 * brief: Ham callback xu ly su kien an giu
 * param:
 * 		button: ten nut nhan dang duoc nhan
 * 		holdingEvent: thoi gian an giu
 * retval: NONE
 */

void MAIN_ButtonHoldCallbackHandle(uint8_t button, uint8_t holdingEvent)
{
	switch(holdingEvent)
	{
		case hold_1s:
		{

			break;
		}
		case hold_2s:
		{

			break;
		}
		case hold_3s:
		{

			break;
		}
		case hold_4s:
		{

			break;
		}
		case hold_5s:
		{

			break;
		}
		default:
			break;
	}
}

/*
 * func: MAIN_ButtonPressCallbackHandle
 * brief: Ham callback xu ly su kien an nha
 * param:
 * 		button: ten nut nhan dang duoc nhan
 * 		pressAndHoldingEvent: so lan an nha
 * retval: NONE
 */

void MAIN_ButtonPressCallbackHandle(uint8_t button, uint8_t pressAndHoldingEvent)
{
	switch(pressAndHoldingEvent)
	{
		case press_1:
		{
			if(button == BUTTON0)
			{

			}
			else if(button == BUTTON1)
			{

			}
			break;
		}
		case press_2:
		{
			if(button == BUTTON0)
			{

			}
			else if(button == BUTTON1)
			{

			}
			break;
		}
		case press_3:
		{
			if(button == BUTTON0)
			{

			}
			else if(button == BUTTON1)
			{

			}
			break;
		}
		case press_4:
		{
			if(button == BUTTON0)
			{

			}
			else if(button == BUTTON1)
			{

			}
			break;
		}
		case press_5:
		{
			if(button == BUTTON0)
			{

			}
			else if(button == BUTTON1)
			{

			}
			break;
		}
		default:
			break;
	}
}

void TimerPrintTableAddressHandler(void)
{
	uint8_t i = 0;
	emberEventControlSetInactive(TimerPrintTableAddressControl);
	for (i = 0; i < emberAfGetAddressTableSize(); i++)
	{
		EmberNodeId nodeId = emberGetAddressTableRemoteNodeId(i);
		if (nodeId != EMBER_TABLE_ENTRY_UNUSED_NODE_ID)
		{
			EmberEUI64 eui64;
			add_node_address(nodeId);
			emberGetAddressTableRemoteEui64(i, eui64);
			emberAfAppDebugExec(emberAfPrintBigEndianEui64(eui64));
			emberAfAppFlush();
		}
	}
	emberEventControlSetDelayMS(TimerPrintTableAddressControl,2000);
}

uint8_t PollRxBuff(void)
{
	uint8_t byRxData;
	uint8_t byUartState = (uint8_t)UART_STATE_IDLE;
	byRxBufState = RX_STATE_START_BYTE;
	while((ring_buff_available(&ringbuff) != 0) && (byUartState == UART_STATE_IDLE))
	{
		ring_buff_pop(&ringbuff, &byRxData);
//		emberAfCorePrint("%d ",byRxData);
		switch(byRxBufState)
		{
			case RX_STATE_START_BYTE:
			{
				if(byRxData == FRAME_SOF)
				{
					byIndexRxBuf = 0;
					byCheckXorRxBuf = CXOR_INIT_VAL;
					byRxBufState = RX_STATE_DATA_BYTES;
				}
				else if(byRxData == FRAME_ACK)
				{
					byUartState = UART_STATE_ACK_RECEIVED;
				}
				else if(byRxData == FRAME_NACK)
				{
					byUartState = UART_STATE_NACK_RECEIVED;
				}
				else
				{
					byUartState = UART_STATE_ERROR;
				}
				break;
			}
			case RX_STATE_DATA_BYTES:
			{
				if(byIndexRxBuf < 254)
				{
					byRxBuffer2[byIndexRxBuf] = byRxData;
					if(byIndexRxBuf > 0)
					{
						byCheckXorRxBuf ^= byRxData;
					}
					if(++byIndexRxBuf == *byRxBuffer2)
					{
						byRxBufState = RX_STATE_CXOR_BYTE;
					}
				}
				else
				{
					byRxBufState = RX_STATE_START_BYTE;
					byUartState = UART_STATE_ERROR;
				}
				break;
			}
			case RX_STATE_CXOR_BYTE:
			{
				if(byRxData == byCheckXorRxBuf)
				{
					byUartState = UART_STATE_DATA_RECEIVED;
				}
				else
				{
					byUartState = UART_STATE_ERROR;
				}
				break;
			}
			default:
				byRxBufState = RX_STATE_START_BYTE;
				break;
		}
	}

	return byUartState;
}

void UartCommandProcess(void *arg)
{
	frm_common_t *pCmd = (frm_common_t *)arg;
	switch(pCmd->cmdid)
	{
		case CMD_ID_LM75:
		{
			data_zone_1.byte_1_lm75 = byRxBuffer2[4];
			data_zone_1.byte_2_lm75 = byRxBuffer2[5];

			emberAfCorePrintln("Data LM75: %d",calculator_hex_to_decimal(data_zone_1.byte_1_lm75, data_zone_1.byte_2_lm75));
			data_zone_1.data_lm75 = calculator_hex_to_decimal(data_zone_1.byte_1_lm75, data_zone_1.byte_2_lm75);
			break;
		}
		case CMD_ID_BH1750:
		{
			data_zone_1.byte_1_bh1750 = byRxBuffer2[4];
			data_zone_1.byte_2_bh1750 = byRxBuffer2[5];

			emberAfCorePrintln("Data BH1750: %d",calculator_hex_to_decimal(data_zone_1.byte_1_bh1750, data_zone_1.byte_2_bh1750));
			data_zone_1.data_bh1750 = calculator_hex_to_decimal(data_zone_1.byte_1_bh1750, data_zone_1.byte_2_bh1750);
			break;
		}
		case CMD_ID_LED_1_ZONE_1:
		{
			emberAfCorePrintln("Raw Data LED_1_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_1.data_led[0] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_2_ZONE_1:
		{
			emberAfCorePrintln("Raw Data LED_2_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_1.data_led[1] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_3_ZONE_1:
		{
			emberAfCorePrintln("Raw Data LED_3_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_1.data_led[2] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_4_ZONE_1:
		{
			emberAfCorePrintln("Raw Data LED_4_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_1.data_led[3] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_5_ZONE_1:
		{
			emberAfCorePrintln("Raw Data LED_5_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_1.data_led[4] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_6_ZONE_1:
		{
			emberAfCorePrintln("Raw Data LED_6_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_1.data_led[5] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_7_ZONE_1:
		{
			emberAfCorePrintln("Raw Data LED_7_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_1.data_led[6] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_8_ZONE_1:
		{
			emberAfCorePrintln("Raw Data LED_8_ZONE_1: %d",byRxBuffer2[4]);
			data_zone_1.data_led[7] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_TEMP_DHT11:
		{
			data_zone_2.byte_1_temp_dht11 = byRxBuffer2[4];
			data_zone_2.byte_2_temp_dht11 = byRxBuffer2[5];

			emberAfCorePrintln("Data TEMP_DHT11: %d",calculator_hex_to_decimal(data_zone_2.byte_1_temp_dht11, data_zone_2.byte_2_temp_dht11));
			data_zone_2.data_temp_dht11 = calculator_hex_to_decimal(data_zone_2.byte_1_temp_dht11, data_zone_2.byte_2_temp_dht11);
			break;
		}
		case CMD_ID_HUMI_DHT11:
		{
			data_zone_2.byte_1_humi_dht11 = byRxBuffer2[4];
			data_zone_2.byte_2_humi_dht11 = byRxBuffer2[5];

			emberAfCorePrintln("Data HUMI_DHT11: %d",calculator_hex_to_decimal(data_zone_2.byte_1_humi_dht11, data_zone_2.byte_2_humi_dht11));
			data_zone_2.data_humi_dht11 = calculator_hex_to_decimal(data_zone_2.byte_1_humi_dht11, data_zone_2.byte_2_humi_dht11);
			break;
		}
		case CMD_ID_LED_1_ZONE_2:
		{
			emberAfCorePrintln("Raw Data LED_1_ZONE_2: %d",byRxBuffer2[4]);
			data_zone_2.data_led[0] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_2_ZONE_2:
		{
			emberAfCorePrintln("Raw Data LED_2_ZONE_2: %d",byRxBuffer2[4]);
			data_zone_2.data_led[1] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_3_ZONE_2:
		{
			emberAfCorePrintln("Raw Data LED_3_ZONE_2: %d",byRxBuffer2[4]);
			data_zone_2.data_led[2] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_4_ZONE_2:
		{
			emberAfCorePrintln("Raw Data LED_4_ZONE_2: %d",byRxBuffer2[4]);
			data_zone_2.data_led[3] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_5_ZONE_2:
		{
			emberAfCorePrintln("Raw Data LED_5_ZONE_2: %d",byRxBuffer2[4]);
			data_zone_2.data_led[4] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_6_ZONE_2:
		{
			emberAfCorePrintln("Raw Data LED_6_ZONE_2: %d",byRxBuffer2[4]);
			data_zone_2.data_led[5] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_7_ZONE_2:
		{
			emberAfCorePrintln("Raw Data LED_7_ZONE_2: %d",byRxBuffer2[4]);
			data_zone_2.data_led[6] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_8_ZONE_2:
		{
			emberAfCorePrintln("Raw Data LED_8_ZONE_2: %d",byRxBuffer2[4]);
			data_zone_2.data_led[7] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_PIR:
		{
			data_zone_2.byte_1_pir = byRxBuffer2[4];
			emberAfCorePrintln("Raw Data PIR: %d",byRxBuffer2[4]);
			break;
		}
		case CMD_ID_MQ8:
		{
			data_zone_3.byte_1_mq7 = byRxBuffer2[4];
			data_zone_3.byte_2_mq7 = byRxBuffer2[5];
			emberAfCorePrintln("Raw Data MQ8: %d  --  %d",byRxBuffer2[4], byRxBuffer2[5]);
			data_zone_3.data_mq7 = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_1_ZONE_3:
		{
		emberAfCorePrintln("Raw Data LED_1_ZONE_3: %d",byRxBuffer2[4]);
			data_zone_3.data_led[0] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_2_ZONE_3:
		{
			emberAfCorePrintln("Raw Data LED_2_ZONE_3: %d",byRxBuffer2[4]);
			data_zone_3.data_led[1] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_3_ZONE_3:
		{
			emberAfCorePrintln("Raw Data LED_3_ZONE_3: %d",byRxBuffer2[4]);
			data_zone_3.data_led[2] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_4_ZONE_3:
		{
			emberAfCorePrintln("Raw Data LED_4_ZONE_3: %d",byRxBuffer2[4]);
			data_zone_3.data_led[3] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_5_ZONE_3:
		{
			emberAfCorePrintln("Raw Data LED_5_ZONE_3: %d",byRxBuffer2[4]);
			data_zone_3.data_led[4] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_6_ZONE_3:
		{
			emberAfCorePrintln("Raw Data LED_6_ZONE_3: %d",byRxBuffer2[4]);
			data_zone_3.data_led[5] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_7_ZONE_3:
		{
			emberAfCorePrintln("Raw Data LED_7_ZONE_3: %d",byRxBuffer2[4]);
			data_zone_3.data_led[6] = byRxBuffer2[4];
			break;
		}
		case CMD_ID_LED_8_ZONE_3:
		{
			emberAfCorePrintln("Raw Data LED_8_ZONE_3: %d",byRxBuffer2[4]);
			data_zone_3.data_led[7] = byRxBuffer2[4];
			break;
		}
		default:
			emberAfCorePrintln("Data Error !!!");
			break;
	}
}

void processDataReceiver(void)
{
	uint8_t stateRx;
	stateRx = PollRxBuff();
	if(stateRx != UART_STATE_IDLE)
	{
		switch(stateRx)
		{
			case UART_STATE_ACK_RECEIVED:
			{
				break;
			}
			case UART_STATE_NACK_RECEIVED:
			{
				break;
			}
			case UART_STATE_DATA_RECEIVED:
			{
				UartCommandProcess(&byRxBuffer2[2]);
				break;
			}
			case UART_STATE_ERROR:
			case UART_STATE_RX_TIMEOUT:
			{
				break;
			}
			default:
				break;
		}
	}
}

void TimerProcessDataHandler(void)
{
	emberEventControlSetInactive(TimerProcessDataControl);
	processDataReceiver();
	emberEventControlSetActive(TimerProcessDataControl);
}

void function_Recive_Data_Over_Zigbee(EmberAfIncomingMessage* incomingMessage)
{
//	emberAfCorePrintln("\n------------ Data: %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x form address: 0x%2x   ------\n\n",incomingMessage->message[0],
//		incomingMessage->message[1],incomingMessage->message[2],incomingMessage->message[3],incomingMessage->message[4],
//		incomingMessage->message[5],incomingMessage->message[6],incomingMessage->message[7],incomingMessage->message[8],
//		incomingMessage->message[9],incomingMessage->message[10],incomingMessage->message[11],incomingMessage->message[12],
//		incomingMessage->message[13],incomingMessage->message[14],incomingMessage->message[15],incomingMessage->message[16],
//		incomingMessage->message[17],incomingMessage->message[18],incomingMessage->message[19],incomingMessage->message[20],
//		incomingMessage->message[21],incomingMessage->message[22],incomingMessage->message[23],incomingMessage->message[24],
//		incomingMessage->message[25],incomingMessage->message[26],incomingMessage->message[27],incomingMessage->message[28],
//		incomingMessage->message[29],incomingMessage->message[30],incomingMessage->message[31],incomingMessage->message[32],
//		incomingMessage->message[33],incomingMessage->message[34],incomingMessage->message[35],incomingMessage->message[36],
//		incomingMessage->message[37],incomingMessage->message[38],incomingMessage->message[39],incomingMessage->message[40],
//		incomingMessage->source);		//Data se duoc thay tu byte thu 7
//	emberAfCorePrintln("\nData_Length: %u\n",incomingMessage->msgLen);
	for (uint32_t i = 0; i < incomingMessage->msgLen; i++)
	{
		ring_buff_push(&ringbuff, incomingMessage->message[i]);
	}
}
