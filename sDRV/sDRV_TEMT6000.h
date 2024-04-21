#include "stm32l0xx_hal.h"

#ifndef __SDRV_TEMT6000_H__
#define __SDRV_TEMT6000_H__

#ifdef __cplusplus
extern "C"{
#endif

void sDRV_TEMT_Init();
void sDRV_TEMT_Startup();
void sDRV_TEMT_Shutdown();
float sDRV_TEMT_GetLight();


#ifdef __cplusplus
}
#endif
#endif
