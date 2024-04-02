#ifndef __SBSP_SOFT_I2C_H__
#define __SBSP_SOFT_I2C_H__

#include "stm32l0xx_hal.h"

#ifdef __cplusplus
extern "C"{
#endif

void sBSP_softI2C_Init();
void sBSP_softI2C_Start();
void sBSP_softI2C_Stop();
void sBSP_softI2C_SendByte(uint8_t byte);
uint8_t sBSP_softI2C_ReciByte();
void sBSP_softI2C_SendACK(uint8_t ACKBIT);
uint8_t sBSP_softI2C_ReciACK();


void sBSP_softI2C_SendData(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
void sBSP_softI2C_ReciData(uint16_t DevAddress, uint8_t *pData, uint16_t Size);




#ifdef __cplusplus
}
#endif

#endif

