#include "stm32l0xx_hal.h"

#ifndef __SBSP_I2C_H__
#define __SBSP_I2C_H__

#ifdef __cplusplus
extern "C"{
#endif



void sBSP_I2C1_Init();

HAL_StatusTypeDef sBSP_I2C1_SendData(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef sBSP_I2C1_ReciData(uint16_t DevAddress, uint8_t *pData, uint16_t Size);

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c);


#ifdef __cplusplus
}
#endif
#endif
