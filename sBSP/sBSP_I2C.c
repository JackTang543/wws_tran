#include "sBSP_I2C.h"

#include "main.h"


I2C_HandleTypeDef hi2c1;

void sBSP_I2C1_Init(){
    hi2c1.Instance = I2C1;
    //
    hi2c1.Init.Timing = 0x00B0384E;
    //从模式地址,主模式不需要
    hi2c1.Init.OwnAddress1 = 0;
    //地址长度,从模式的设置
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    //双地址模式,从模式的设置
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    //第二个地址
    hi2c1.Init.OwnAddress2 = 0;
    //
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    //广播模式,如果地址为0x00,那么所有设备都会响应(若可用)
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    //时钟延展,从模式功能,作用:发送:你太快啦,等我一下
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    //初始化I2C
    if(HAL_I2C_Init(&hi2c1) != HAL_OK){
        sERROR_Handler();
    }
    
    /** Configure Analogue filter
  */
//   if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
//   {
//     sERROR_Handler();
//   }

  /** Configure Digital filter
  */
//   if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0xF) != HAL_OK)
//   {
//     sERROR_Handler();
//   }


}

void sBSP_I2C1_DeInit(){

}




HAL_StatusTypeDef sBSP_I2C1_SendData(uint16_t DevAddress, uint8_t *pData, uint16_t Size){
    if(HAL_I2C_Master_Transmit(&hi2c1, DevAddress, pData, Size, 10000) != HAL_OK) {
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef sBSP_I2C1_ReciData(uint16_t DevAddress, uint8_t *pData, uint16_t Size){
    if(HAL_I2C_Master_Receive(&hi2c1, DevAddress, pData, Size, 10000) != HAL_OK) {
        return HAL_ERROR;
    }
    return HAL_OK;
}




