#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32l0xx_hal.h"

#include "sDRV_UART.h"
#include "sDRV_LED.h"
#include "sDRV_BMP280.h"
#include "sDRV_AHT10.h"

#include "sBSP_TIM.h"

#include "sBSP_SYS.h"

#include "sDRV_Si24R1.h"

#include "sAPP_Func.h"

#include "sBSP_ADC.h"

#include "sDRV_TEMT6000.h"
#include "sBSP_RTC.h"


#ifdef __cplusplus
extern "C" {
#endif



void recied_data(Si24R1_Data_t* data);


void sERROR_Handler();



#ifdef __cplusplus
}
#endif

#endif
