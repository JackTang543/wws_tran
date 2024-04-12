#ifndef __SBSP_SOFT_SPI_H__
#define __SBSP_SOFT_SPI_H__
//控制是否启用这个模块
#define __SBSP_SOFT_SPI_EN__
#ifdef  __SBSP_SOFT_SPI_EN__


#include "stm32l0xx_hal.h"

#ifdef __cplusplus
extern "C"{
#endif

void sBSP_softSPI_Init();
void sBSP_softSPI_SetCS(uint8_t lv);
void sBSP_softSPI_SendByte(uint8_t byte);
uint8_t sBSP_softSPI_ReciByte();
void sBSP_softSPI_SendBytes(uint8_t* pData,uint32_t num);
void sBSP_softSPI_ReciBytes(uint8_t* pData,uint32_t num);




#ifdef __cplusplus
}
#endif

#endif
#endif

