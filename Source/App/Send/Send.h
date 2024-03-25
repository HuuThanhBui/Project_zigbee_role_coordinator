#ifndef _SEND_H_
#define _SEND_H_

#include "stdint.h"
#include "stdbool.h"

void SEND_ReportInfoToHc(void);
void SEND_FillBufferGlobalCommand(EmberAfClusterId clusterID, EmberAfAttributeId attributeId, uint8_t globalCommand, uint16_t *value, uint8_t length, uint8_t dataType);
void SEND_SendCommandUnicast(uint8_t source, uint8_t destination, uint16_t address);
void SEND_OnOffStateReport(uint8_t endpoint_source, uint8_t endpoint_destination, uint8_t value);
void SEND_Data(uint16_t address, uint8_t endpoint_source, uint8_t endpoint_destination, uint8_t *value);

#endif
