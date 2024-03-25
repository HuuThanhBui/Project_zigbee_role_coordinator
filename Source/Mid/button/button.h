#ifndef SOURCE_MID_BUTTON_BUTTON_H_
#define SOURCE_MID_BUTTON_BUTTON_H_

typedef enum{
	release,
	press_1,
	press_2,
	press_3,
	press_4,
	press_5
}btnPressState_e;

typedef enum{
	hold_0s,
	hold_1s,
	hold_2s,
	hold_3s,
	hold_4s,
	hold_5s
}btnHoldState_e;

typedef void (*buttonHoldCallbackFunction)(uint8_t, btnHoldState_e);
typedef void (*buttonPresCallbackFunction)(uint8_t, btnPressState_e);

void buttonInit(buttonHoldCallbackFunction,buttonPresCallbackFunction);

#endif
