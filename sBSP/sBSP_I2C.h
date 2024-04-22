#ifndef __SBSP_I2C1_H__
#define __SBSP_I2C1_H__

#include "stm32l0xx_hal.h"


//sBSP_I2C.h
//平台:STM32L031
//Sightseer's BSP I2C interface

#ifdef __cplusplus
extern "C"{
#endif

/** @defgroup I2C_XferOptions_definition I2C XferOptions definition
  * @{
  */
// #define  I2C_FIRST_FRAME                0x00000001U
// #define  I2C_FIRST_AND_NEXT_FRAME       0x00000002U
// #define  I2C_NEXT_FRAME                 0x00000004U
// #define  I2C_FIRST_AND_LAST_FRAME       0x00000008U
// #define  I2C_LAST_FRAME_NO_STOP         0x00000010U
// #define  I2C_LAST_FRAME                 0x00000020U

/** @defgroup I2C_Memory_Address_Size I2C Memory Address Size
  * @{
  */
// #define I2C_MEMADD_SIZE_8BIT            0x00000001U
// #define I2C_MEMADD_SIZE_16BIT           0x00000010U


int8_t sBSP_I2C1_Init();
void sBSP_I2C1_DeInit();

int8_t  sBSP_I2C1M_SendByte     (uint16_t DevAddr,uint8_t data);
int8_t  sBSP_I2C1M_SendBytes    (uint16_t DevAddr,uint8_t* pData,uint16_t length);
uint8_t sBSP_I2C1M_ReadByte     (uint16_t DevAddr);
int8_t  sBSP_I2C1M_ReadBytes    (uint16_t DevAddr,uint8_t* pData,uint16_t length);
//! 这个顺序操作在L031好像不能用
int8_t  sBSP_I2C1M_SeqReadByte  (uint16_t DevAddr,uint8_t* pData,uint32_t XferOptions);
int8_t  sBSP_I2C1M_SeqReadBytes (uint16_t DevAddr,uint8_t* pData,uint16_t length,uint32_t XferOptions);
int8_t  sBSP_I2C1M_SeqSendByte  (uint16_t DevAddr,uint8_t data,uint32_t XferOptions);
int8_t  sBSP_I2C1M_SeqSendBytes (uint16_t DevAddr,uint8_t* pData,uint16_t length,uint32_t XferOptions);

int8_t  sBSP_I2C1M_MemSendByte  (uint16_t dev_addr,uint16_t mem_addr,uint16_t addr_size,uint8_t data);
int8_t  sBSP_I2C1M_MemSendBytes (uint16_t dev_addr,uint16_t mem_addr,uint16_t addr_size,uint8_t* pData,uint16_t length);
uint8_t sBSP_I2C1M_MemReadByte  (uint16_t dev_addr,uint16_t mem_addr,uint16_t addr_size);
int8_t  sBSP_I2C1M_MemReadBytes (uint16_t dev_addr,uint16_t mem_addr,uint16_t addr_size,uint8_t* pData,uint16_t length);

//! 这个好像是发送完一帧的完成回调
int8_t sBSP_I2C1M_IsTxCplt();
int8_t sBSP_I2C1M_IsRxCplt();
int8_t sBSP_I2C1M_IsCplt();

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c);

    
#ifdef __cplusplus
}
#endif

#endif
