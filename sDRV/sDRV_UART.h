#ifndef __SDRV_UART_H__
#define __SDRV_UART_H__


#include "stm32l0xx_hal.h"

#include "sBSP_UART.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>


void sDRV_UART_Init(uint32_t bandrate);
void sDRV_UART_Printf(char *p,...);


#endif

