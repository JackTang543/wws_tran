#ifndef __SBSP_UART_H__
#define __SBSP_UART_H__

#include "stm32l0xx_hal.h"


typedef void(*reci_data_end_cb_t)(uint8_t* pReciData,uint16_t length);


void sBSP_UART1_Init(uint32_t bandrate);
void sBSP_UART1_SendBytes(uint8_t* pData,uint16_t length);
void sBSP_UART1_ReadBytesBegin(reci_data_end_cb_t end_cb);


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);

#endif
