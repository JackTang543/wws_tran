#include "stm32l0xx_hal.h"

#include "sDRV_Si24R1.h"
#include "main.h"

#ifndef __SAPP_FUNC_H__
#define __SAPP_FUNC_H__

#ifdef __cplusplus
extern "C"{
#endif

void sFUNC_2D4GHZ_PWR_Init();
void sFUNC_Set2D4GHZ_PWR(uint8_t is_en);

void sFUNC_SetRTCAlarmA(uint32_t seconds);
void sFUNC_InitBMP280();
void sFUNC_Init2d4GHz();
float sAPP_Func_GetVbat();


#ifdef __cplusplus
}
#endif
#endif
