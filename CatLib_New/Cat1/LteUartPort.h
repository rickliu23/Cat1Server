#ifndef _LTEUARTPORT_H_
#define _LTEUARTPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "usart.h"
#include "stdbool.h"

bool HW_UART_Transmit(uint8_t *pData, uint16_t len);

void HW_UART_Start_Recv(void);



void HW_LTE_Disable(void);
void HW_LTE_Enable(void);



bool HW_DEBUG_Transmit(uint8_t *pData, uint16_t len);


void HW_Printf(char *info);

#ifdef __cplusplus
}
#endif

#endif
