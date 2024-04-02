#include "stm32l0xx_hal.h"

#ifndef __SBSP_TIM_H__
#define __SBSP_TIM_H__

void sBSP_TIM2_Init();
void sBSP_TIM2_SetPWMFreq(int32_t freq);
void sBSP_TIM2_SetCH3PWMDuty(uint8_t percent);
void sBSP_TIM2_SetCH3PWMEN(uint8_t en);





#endif
