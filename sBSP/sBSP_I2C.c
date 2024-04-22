#include "sBSP_I2C.h"


#include "sDRV_UART.h"

/** 
 * sBSP_I2C.c
 * 平台:STM32L031
 * Sightseer's BSP I2C interface
 * 
 * 更新日志:
 * 
 * v1.0  2024.04.16inHNIP9607
 * 说明: 初版
 * 
 * v1.1  2024.04.19inHNIP9607
 * 增加了Mem操作,所有操作改成内联函数
 * 
 * v1.2  2024.04.21inHNIP9607
 * 从F103移植到L031平台
 * 
 */


//I2C1是否检查发送/接收错误
#define I2C1_CHECK_ERROR
//I2C1的轮询超时时间
#define I2C1_POT_MS    (100)
//I2C是否使用中断处理,默认使用轮询处理,此宏不影响Seq操作
//#define I2C1_USE_IT


uint8_t I2C1M_RxCpltFlag;
uint8_t I2C1M_TxCpltFlag;

I2C_HandleTypeDef hi2c1;

int8_t sBSP_I2C1_Init(){
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
        return -1;
    }
    return 0;
}

void sBSP_I2C1_DeInit(){

}

//MSP在stm32l0xx_hal_msp.c文件里
//这是一个参考定义:
//
//重写HAL的MSP(MCU Support Package)Init
// void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c){
//     if(hi2c->Instance == I2C1){
//         //! 先配置时钟,再配置外设!!!
//         __GPIOA_CLK_ENABLE();
//         GPIO_InitTypeDef GPIO_InitStruct = {0};
//         GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
//         GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
//         GPIO_InitStruct.Pull = GPIO_PULLUP;
//         GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//         GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
//         HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
//         __I2C1_CLK_ENABLE();
//         HAL_NVIC_SetPriority(I2C1_IRQn, 1, 0);
//         HAL_NVIC_EnableIRQ(I2C1_IRQn);
//     }
// }

/**
  * @brief  I2C1主机发送一个字节
  *
  * @param  DevAddr 设备地址
  * @param  data 要发送的数据
  *
  * @return 如果正常返回0,其他为异常值
  */
inline int8_t sBSP_I2C1M_SendByte(uint16_t DevAddr,uint8_t data){
    I2C1M_TxCpltFlag = 0;

    #ifdef I2C1_CHECK_ERROR
        #ifdef I2C1_USE_IT
            if(HAL_I2C_Master_Transmit_IT(&hi2c1,DevAddr,&data,1,I2C1_POT_MS) != HAL_OK) {
                return -1;
            }
        #else
            if(HAL_I2C_Master_Transmit(&hi2c1,DevAddr,&data,1,I2C1_POT_MS) != HAL_OK) {
                return -1;
            }
        #endif
    #else
        #ifdef I2C1_USE_IT
            HAL_I2C_Master_Transmit_IT(&hi2c1,DevAddr,&data,1,I2C1_POT_MS);
        #else
            HAL_I2C_Master_Transmit(&hi2c1,DevAddr,&data,1,I2C1_POT_MS);
        #endif
    #endif

    return 0;
}

/**
  * @brief  I2C1主机发送多个字节
  *
  * @param  DevAddr 设备地址
  * @param  pData 数据指针
  * @param  length 数据长度
  *
  * @return 如果正常返回0,其他为异常值
  */
inline int8_t sBSP_I2C1M_SendBytes(uint16_t DevAddr,uint8_t* pData,uint16_t length){
    I2C1M_TxCpltFlag = 0;
    
    #ifdef I2C1_CHECK_ERROR
        #ifdef I2C1_USE_IT
            if(HAL_I2C_Master_Transmit_IT(&hi2c1,DevAddr,pData,length,I2C1_POT_MS) != HAL_OK) {
                return -1;
            }
        #else
            if(HAL_I2C_Master_Transmit(&hi2c1,DevAddr,pData,length,I2C1_POT_MS) != HAL_OK) {
                return -1;
            }
        #endif
    #else
        #ifdef I2C1_USE_IT
            HAL_I2C_Master_Transmit_IT(&hi2c1,DevAddr,pData,length,I2C1_POT_MS);
        #else
            HAL_I2C_Master_Transmit(&hi2c1,DevAddr,pData,length,I2C1_POT_MS);
        #endif
    #endif

    return 0;
}

/**
  * @brief  I2C1主机接收一个字节
  *
  * @param  DevAddr 设备地址
  *
  * @return 返回的数据
  */
inline uint8_t sBSP_I2C1M_ReadByte(uint16_t DevAddr){
    I2C1M_RxCpltFlag = 0;

    uint8_t ret = 0;

    #ifdef I2C1_USE_IT
            HAL_I2C_Master_Receive_IT(&hi2c1,DevAddr,&ret,1,I2C1_POT_MS);
    #else
        HAL_I2C_Master_Receive(&hi2c1,DevAddr,&ret,1,I2C1_POT_MS);
    #endif

    return ret;
}

/**
  * @brief  I2C1主机接收多个字节
  *
  * @param  DevAddr 设备地址
  * @param  pData   数据指针
  * @param  length  数据长度
  *
  * @return 如果正常返回0,其他为异常值
  */
inline int8_t sBSP_I2C1M_ReadBytes(uint16_t DevAddr,uint8_t* pData,uint16_t length){
    I2C1M_RxCpltFlag = 0;

    #ifdef I2C1_CHECK_ERROR
        #ifdef I2C1_USE_IT
            if(HAL_I2C_Master_Receive_IT(&hi2c1,DevAddr,pData,length,I2C1_POT_MS) != HAL_OK) {
                return -1;
            }
        #else
            if(HAL_I2C_Master_Receive(&hi2c1,DevAddr,pData,length,I2C1_POT_MS) != HAL_OK) {
                return -1;
            }
        #endif
    #else
        #ifdef I2C1_USE_IT
            HAL_I2C_Master_Receive_IT(&hi2c1,DevAddr,pData,length,I2C1_POT_MS);
        #else
            HAL_I2C_Master_Receive(&hi2c1,DevAddr,pData,length,I2C1_POT_MS);
        #endif
    #endif

    return 0;
}

/**
  * @brief  I2C1主机顺序操作接收一个字节
  *
  * @param  DevAddr 设备地址
  * @param  XferOptions 操作,参考HAL库I2C @ref I2C_XferOptions_definition
  * @param  pData 接收到的数据
  *
  * @return 如果正常返回0,其他为异常值
  */
inline int8_t sBSP_I2C1M_SeqReadByte(uint16_t DevAddr,uint8_t* pData,uint32_t XferOptions){
    I2C1M_RxCpltFlag = 0;

    #ifdef I2C1_CHECK_ERROR
        if(HAL_I2C_Master_Seq_Receive_IT(&hi2c1,DevAddr,pData,1,XferOptions) != HAL_OK) {
            return -1;
        }
    #else
        HAL_I2C_Master_Seq_Receive_IT(&hi2c1,DevAddr,pData,1,XferOptions);
    #endif

    return 0;
}

/**
  * @brief  I2C1主机顺序操作接收多个字节
  *
  * @param  DevAddr 设备地址
  * @param  pData   数据指针
  * @param  length  数据长度
  * @param  XferOptions 操作,参考HAL库I2C @ref I2C_XferOptions_definition
  *
  * @return 如果正常返回0,其他为异常值
  */
inline int8_t sBSP_I2C1M_SeqReadBytes(uint16_t DevAddr,uint8_t* pData,uint16_t length,uint32_t XferOptions){
    I2C1M_RxCpltFlag = 0;

    #ifdef I2C1_CHECK_ERROR
        if(HAL_I2C_Master_Seq_Receive_IT(&hi2c1,DevAddr,pData,length,XferOptions) != HAL_OK) {
            return -1;
        }
    #else
        HAL_I2C_Master_Seq_Receive_IT(&hi2c1,DevAddr,pData,length,XferOptions);
    #endif

    return 0;
}

/**
  * @brief  I2C1主机顺序操作发送一个字节
  *
  * @param  DevAddr 设备地址
  * @param  XferOptions 操作,参考HAL库I2C @ref I2C_XferOptions_definition
  *
  * @return 如果正常返回0,其他为异常值
  */
inline int8_t sBSP_I2C1M_SeqSendByte(uint16_t DevAddr,uint8_t data,uint32_t XferOptions){
    I2C1M_TxCpltFlag = 0;

    #ifdef I2C1_CHECK_ERROR
        if(HAL_I2C_Master_Seq_Transmit_IT(&hi2c1,DevAddr,&data,1,XferOptions) != HAL_OK) {
            return -1;
        }
    #else
        HAL_I2C_Master_Seq_Transmit_IT(&hi2c1,DevAddr,&data,1,XferOptions);
    #endif

    return 0;
}

/**
  * @brief  I2C1主机顺序操作发送多个字节(中断式)
  *
  * @param  DevAddr 设备地址
  * @param  pData   数据指针
  * @param  length  数据长度
  * @param  XferOptions 操作,参考HAL库I2C @ref I2C_XferOptions_definition
  *
  * @return 如果正常返回0,其他为异常值
  */
inline int8_t sBSP_I2C1M_SeqSendBytes(uint16_t DevAddr,uint8_t* pData,uint16_t length,uint32_t XferOptions){
    I2C1M_TxCpltFlag = 0;

    #ifdef I2C1_CHECK_ERROR
        if(HAL_I2C_Master_Seq_Transmit_IT(&hi2c1,DevAddr,pData,length,XferOptions) != HAL_OK) {
            return -1;
        }
    #else
        HAL_I2C_Master_Seq_Transmit_IT(&hi2c1,DevAddr,pData,length,XferOptions);
    #endif

    return 0;
}

/**
  * @brief  I2C1主机对从机内存操作发送一个字节
  *
  * @param  dev_addr  设备地址
  * @param  mem_addr  内存地址
  * @param  addr_size 设备内存地址的长度,参数:I2C_MEMADD_SIZE_8BIT/I2C_MEMADD_SIZE_16BIT
  * @param  data      数据
  *
  * @return 如果正常返回0,其他为异常值
  */
inline int8_t sBSP_I2C1M_MemSendByte(uint16_t dev_addr,uint16_t mem_addr,uint16_t addr_size,uint8_t data){
    I2C1M_TxCpltFlag = 0;

    #ifdef I2C1_CHECK_ERROR
        #ifdef I2C1_USE_IT
        if(HAL_I2C_Mem_Write_IT(&hi2c1,dev_addr,mem_addr,addr_size,&data,1) != HAL_OK) {
            return -1;
        }
        #else
        if(HAL_I2C_Mem_Write(&hi2c1,dev_addr,mem_addr,addr_size,&data,1,I2C1_POT_MS) != HAL_OK) {
            return -1;
        }
        #endif
    #else
        #ifdef I2C1_USE_IT
        HAL_I2C_Mem_Write_IT(&hi2c1,dev_addr,mem_addr,addr_size,&data,1);
        #else
        HAL_I2C_Mem_Write(&hi2c1,dev_addr,mem_addr,addr_size,&data,1,I2C1_POT_MS);
        #endif
    #endif

    return 0;
}

/**
  * @brief  I2C1主机对从机内存操作发送多个字节
  *
  * @param  dev_addr  设备地址
  * @param  mem_addr  内存地址
  * @param  addr_size 设备内存地址的长度,参数:I2C_MEMADD_SIZE_8BIT/I2C_MEMADD_SIZE_16BIT
  * @param  pData     数据指针
  * @param  length    长度
  * @return 如果正常返回0,其他为异常值
  */
inline int8_t sBSP_I2C1M_MemSendBytes(uint16_t dev_addr,uint16_t mem_addr,uint16_t addr_size,uint8_t* pData,uint16_t length){
    I2C1M_TxCpltFlag = 0;
    
    #ifdef I2C1_CHECK_ERROR
        #ifdef I2C1_USE_IT
            if(HAL_I2C_Mem_Write_IT(&hi2c1,dev_addr,mem_addr,addr_size,data,length) != HAL_OK) {
                return -1;
            }
        #else
            if(HAL_I2C_Mem_Write(&hi2c1,dev_addr,mem_addr,addr_size,pData,length,I2C1_POT_MS) != HAL_OK) {
                return -1;
            }
        #endif
    #else
        #ifdef I2C1_USE_IT
            HAL_I2C_Mem_Write_IT(&hi2c1,dev_addr,mem_addr,addr_size,data,length);
        #else
            HAL_I2C_Mem_Write(&hi2c1,dev_addr,mem_addr,addr_size,pData,length,I2C1_POT_MS);
        #endif
    #endif

    return 0;
}

/**
  * @brief  I2C1主机对从机内存操作接收一个字节
  *
  * @param  dev_addr  设备地址
  * @param  mem_addr  内存地址
  * @param  addr_size 设备内存地址的长度,参数:I2C_MEMADD_SIZE_8BIT/I2C_MEMADD_SIZE_16BIT
  *
  * @return 返回的数据
  */
inline uint8_t sBSP_I2C1M_MemReadByte(uint16_t dev_addr,uint16_t mem_addr,uint16_t addr_size){
    I2C1M_RxCpltFlag = 0;
    uint8_t ret = 0;

    #ifdef I2C1_USE_IT
        HAL_I2C_Mem_Read_IT(&hi2c1,dev_addr,mem_addr,addr_size,&ret,1)
    #else
        HAL_I2C_Mem_Read(&hi2c1,dev_addr,mem_addr,addr_size,&ret,1,I2C1_POT_MS);
    #endif

    return ret;
}

/**
  * @brief  I2C1主机对从机内存操作接收多个字节
  *
  * @param  DevAddr 设备地址
  * @param  pData   数据指针
  * @param  length  数据长度
  *
  * @return 如果正常返回0,其他为异常值
  */
inline int8_t sBSP_I2C1M_MemReadBytes(uint16_t dev_addr,uint16_t mem_addr,uint16_t addr_size,uint8_t* pData,uint16_t length){
    I2C1M_RxCpltFlag = 0;

    #ifdef I2C1_CHECK_ERROR
        #ifdef I2C1_USE_IT
            if(HAL_I2C_Mem_Read_IT(&hi2c1,dev_addr,mem_addr,addr_size,pData,length) != HAL_OK) {
                return -1;
            }
        #else
            if(HAL_I2C_Mem_Read(&hi2c1,dev_addr,mem_addr,addr_size,pData,length,I2C1_POT_MS) != HAL_OK) {
                return -1;
            }
        #endif
    #else
        #ifdef I2C1_USE_IT
            HAL_I2C_Mem_Read_IT(&hi2c1,dev_addr,mem_addr,addr_size,pData,length);
        #else
            HAL_I2C_Mem_Read(&hi2c1,dev_addr,mem_addr,addr_size,pData,length,I2C1_POT_MS);
        #endif
    #endif

    return 0;
}



inline int8_t sBSP_I2C1M_IsTxCplt(){
    return I2C1M_TxCpltFlag;
}

inline int8_t sBSP_I2C1M_IsRxCplt(){
    return I2C1M_RxCpltFlag;
}

inline int8_t sBSP_I2C1M_IsCplt(){
    return I2C1M_TxCpltFlag | I2C1M_RxCpltFlag;
}


void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c){
    if(hi2c->Instance == I2C1){
        I2C1M_TxCpltFlag = 1;
    }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c){
    if(hi2c->Instance == I2C1){
        I2C1M_RxCpltFlag = 1;
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c){
    if(hi2c->Instance == I2C1){
        sDRV_UART_Printf("sBSP_I2C:ERROR\n");
    }
}











