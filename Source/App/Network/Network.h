#ifndef _NETWORK_HEADER_H_
#define _NETWORK_HEADER_H_

#include <stdint.h>
#include <stdbool.h>
#include "app/framework/include/af.h"

enum state_of_network{
	NETWORK_HAS_PARENT = EMBER_NETWORK_DOWN,
	NETWORK_JOIN_FAIL,
	NETWORK_JOIN_SUCCESS = EMBER_NETWORK_UP,
	NETWORK_LOST_PARENT = EMBER_JOINED_NETWORK_NO_PARENT,
	NETWORK_OUT_NETWORK = EMBER_NO_NETWORK
};

typedef struct var_of_program {
	int systemState;
	uint16_t timeFindAndJoin;
	bool networkReady;
}var_proc_t;

extern var_proc_t var_proc;

typedef void (*network_init)(uint8_t networkResult);
void NETWORK_Init(network_init nwkInitCallbackFunc);
void networkSetChannelMask(uint32_t mask_default);
void NETWORK_FindAndJoin(void);
void joinNetworkEventHandler(void);

#endif
