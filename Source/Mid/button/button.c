#include "stdint.h"
#include "stdbool.h"
#include "app/framework/include/af.h"
#include "button.h"
#include "em_chip.h"
#include "em_device.h"
#include "em_cmu.h"

#define Delay_Hold 200		//Thoi gian delay cua su kien an giu, khi time vuot qua gia tri nay times nhan giu se ++ them 1 don vi
#define Delay_Pres 300		//Thoi gian delay cua su kien an nha, khi time nho hon qua gia tri nay so lan nhan nha se ++ them 1 don vi

buttonHoldCallbackFunction hold_callback_handler = NULL;	//Con tro ham toan cuc voi kieu buttonHoldCallbackFunction
buttonPresCallbackFunction press_callback_handler = NULL;	//Con tro ham toan cuc voi kieu buttonPresCallbackFunction
EmberEventControl TimerCheckPressControl;		//Bien dieu khien event timer dem so lan nhan nha
EmberEventControl TimerCheckHoldControl;		//Bien dieu khien event timer dem so thoi gian an giu

typedef struct vari_init vari_init_t;		//Struct data luu thong tin ket qua cua nut nhan
struct vari_init{
	uint8_t hold_time;
	bool is_hold;
	uint8_t press_count;
	uint8_t name_button;
};
vari_init_t var;

typedef struct								//Struct data luu gia tri chong nhieu nut nha
{
	uint8_t btn_filter_1;
	uint8_t btn_filter_2;
	uint8_t is_debouncing;
	uint32_t time_deboune;
}Button_Typdef;

void TimerCheckPressHandler(void);
void TimerCheckHoldHandler(void);

/*
 * func: buttonInit
 * brief: Ham khoi tao cac funtion call de xu ly cac event cua nut nhan
 * param:
 * 		hold_func: Ham xu ly nhan giu
 * 		press_func: Ham xu ly nhan nha
 * retval: NONE
 */

void buttonInit(buttonHoldCallbackFunction hold_func,buttonPresCallbackFunction press_func)
{
	if(hold_func != NULL && press_func != NULL)
	{
		hold_callback_handler = hold_func;
		press_callback_handler = press_func;
	}
}

/*
 * func: button_handler
 * brief: Ham xu ly su kien nhan nut de tinh thoi gian an nha, an giu nut nhan
 * param:
 * 		pin: Chan pin cua nut nhan dang duoc nhan
 * 		state: trang thai cua nut nhan dang duoc nhan
 * retval: NONE
 */

void button_handler(uint8_t pin, uint8_t state)
{
	if(pin == BUTTON0)		//SW1
	{
		var.name_button = BUTTON0;
		switch(state)
		{
			case release:
			{
//				emberAfCorePrintln("SW1 is Released");
				emberEventControlSetDelayMS(TimerCheckPressControl,Delay_Pres);		//Set delay cua su kien nhan nha Delay_Pres
				emberEventControlSetInactive(TimerCheckHoldControl);				//Inactive kien nhan giu
				break;
			}
			case press_1:
			{
//				emberAfCorePrintln("SW1 is Pressed");
				var.press_count++;
				var.hold_time = 0;
				var.is_hold = false;
				emberEventControlSetDelayMS(TimerCheckHoldControl,Delay_Hold);		//Set delay cua su kien nhan giu Delay_Hold
				emberEventControlSetInactive(TimerCheckPressControl);				//Inactive kien nhan nha
				break;
			}
		}
	}
	else if(pin == BUTTON1)		//SW2
	{
		var.name_button = BUTTON1;
		switch(state)
		{
			case release:
			{
//				emberAfCorePrintln("SW2 is Released");
				emberEventControlSetDelayMS(TimerCheckPressControl,Delay_Pres);
				emberEventControlSetInactive(TimerCheckHoldControl);
				break;
			}
			case press_1:
			{
//				emberAfCorePrintln("SW2 is Pressed");
				var.press_count++;
				var.hold_time = 0;
				var.is_hold = false;
				emberEventControlSetDelayMS(TimerCheckHoldControl,Delay_Hold);
				emberEventControlSetInactive(TimerCheckPressControl);
				break;
			}
		}
	}
}

/*
 * func: emberAfHalButtonIsrCallback
 * brief: Ham se duoc goi khi co su kien nhan nut, ham nay se duoc goi boi Stack Zigbee.
 * param:
 * 		button: Chan pin cua nut nhan dang duoc nhan
 * 		state: trang thai cua nut nhan dang duoc nhan
 * retval: NONE
 */

void emberAfHalButtonIsrCallback(uint8_t button, uint8_t state)
{
//	Button_Typdef Button_t;
//	Button_t.btn_filter_1 = 0;
//	Button_t.btn_filter_2 = 0;
//	Button_t.is_debouncing = 0;
//	Button_t.time_deboune = 0;
//	while(1)
//	{
//		uint32_t sta_1 = GPIO_PinInGet(gpioPortD, 4);
//		uint32_t sta_2 = GPIO_PinInGet(gpioPortD, 3);
//		if(sta_1 != Button_t.btn_filter_1 || sta_2 != Button_t.btn_filter_2)
//		{
//			Button_t.btn_filter_1 = sta_1;
//			Button_t.btn_filter_2 = sta_2;
//			Button_t.is_debouncing = 1;
//			Button_t.time_deboune = halCommonGetInt32uMillisecondTick();
//		}
//		//------------------ Tin hieu da xac lap------------------------
//		if(Button_t.is_debouncing && (halCommonGetInt32uMillisecondTick() - Button_t.time_deboune >= 100))
//		{
//			Button_t.is_debouncing =0;
//			break;
//		}
//	}

	uint32_t time_deboune = halCommonGetInt32uMillisecondTick();
	while(1)
	{
		if(halCommonGetInt32uMillisecondTick() - time_deboune >= 200)
		{
			break;
		}
	}
	button_handler(button,state);								//Ham xu ly trang thai nut bam dua vao ten nut bam, va trang thai nut dang duoc bam
}

/*
 * func: TimerCheckPressHandler
 * brief: Ham xu ly event cua nut nhan, in ra so lan bam nha, va dem thoi gian an giu
 * param: NONE
 * retval: NONE
 */

void TimerCheckPressHandler(void)
{
	emberEventControlSetInactive(TimerCheckPressControl);
	if(var.is_hold == true)
	{
		emberAfCorePrintln("So giay nhan giu la: %d",(var.hold_time / 5));	//Cu 200 ms thi var.hold_time++ mot don vi, vay can var.hold_time ++ len 5 don vi thi tao ra 1s
		hold_callback_handler(var.name_button,(var.hold_time / 5));
	}
	else
	{
		emberAfCorePrintln("So lan bam la: %d",var.press_count);
		if(var.press_count == 1)
		{
			press_callback_handler(var.name_button, press_1);
		}
		else if(var.press_count == 2)
		{
			press_callback_handler(var.name_button, press_2);
		}
		else if(var.press_count == 3)
		{
			press_callback_handler(var.name_button, press_3);
		}
		else if(var.press_count == 4)
		{
			press_callback_handler(var.name_button, press_4);
		}
		else if(var.press_count == 5)
		{
			press_callback_handler(var.name_button, press_5);
		}
	}
	var.is_hold = false;
	var.press_count = 0;
	emberEventControlSetDelayMS(TimerCheckPressControl,Delay_Pres);
}

/*
 * func: TimerCheckHoldHandler
 * brief: Ham xu ly event cua nut nhan, dem thoi gian an giu
 * param: NONE
 * retval: NONE
 */

void TimerCheckHoldHandler(void)
{
	emberEventControlSetInactive(TimerCheckHoldControl);
	var.hold_time++;
	if(var.hold_time >= 5)			//Khi time lon hon 5*Delay_Hold(200) = 1000ms = 1s thi bat dau kich hoat dem so lan nhan giu
	{
		var.is_hold = true;
	}
	emberEventControlSetDelayMS(TimerCheckHoldControl,Delay_Hold);
}


