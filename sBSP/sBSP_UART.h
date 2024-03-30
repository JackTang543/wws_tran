#ifndef __SBSP_UART_H__
#define __SBSP_UART_H__

#include "stm32l0xx_hal.h"

void sBSP_UART1_Init(uint32_t bandrate);
void sBSP_UART1_SendBytes(uint8_t* pData,uint16_t length);


#endif
