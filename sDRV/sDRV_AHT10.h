#include "stm32l0xx_hal.h"

#ifndef __SDRV_AHT10_H__
#define __SDRV_AHT10_H__

#ifdef __cplusplus
extern "C"{
#endif


#define AHT10_ADDR (0x38 << 1)


void sDRV_AHT10_Init(void);
void sDRV_AHT10_StartMeasure();
void sDRV_AHT10_EndMeasure();
float sDRV_AHT10_GetHumi();
float sDRV_AHT10_GetTemp();


#ifdef __cplusplus
}
#endif
#endif
