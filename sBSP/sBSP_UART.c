#include "sBSP_UART.h"

#ifdef __cplusplus
extern "C" {
#endif

UART_InitTypeDef huart1;

void sBSP_UART1_Init(uint32_t bandrate){
    huart1.BaudRate = bandrate;
    huart1.HwFlowCtl = 
    huart1.Mode = 
    huart1.OneBitSampling = 
    huart1.OverSampling = 
    huart1.Parity = 
    huart1.StopBits = 
    huart1.WordLength =
    HAL_UART_Init(&huart1);
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart){
    if(huart->Instance == LPUART1){

    }
}

void sBSP_UART1_SendBytes(uint8_t* pData,uint16_t length){

}



#ifdef __cplusplus
}
#endif
