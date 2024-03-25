#ifndef _RECEIVE_H_
#define _RECEIVE_H_

#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "app/framework/include/af.h"
#include "zigbee-framework/zigbee-device-library.h"

typedef void (*callback_data_recive_over_zigbee)(EmberAfIncomingMessage* incomingMessage);
void SendZclCommandViaBinding(uint8_t source_endpoint, uint8_t destination_endpoint, bool value);
void init_function_callback_recive_data_over_zigbee(callback_data_recive_over_zigbee callback);

#endif
