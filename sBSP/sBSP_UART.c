#include "sBSP_UART.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

UART_HandleTypeDef huart1;

uint8_t UART1_RxBuf[128];

//保存用户传入的接收完成回调
reci_data_end_cb_t uart1_reci_data_end_cb;



void sBSP_UART1_Init(uint32_t bandrate){
    huart1.Instance = LPUART1;
    huart1.Init.BaudRate = bandrate;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_ENABLE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.WordLength =UART_WORDLENGTH_8B;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if(HAL_UART_Init(&huart1) != HAL_OK){
        sERROR_Handler();
    }
    //__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
}


void sBSP_UART1_SendBytes(uint8_t* pData,uint16_t length){
    HAL_UART_Transmit(&huart1,pData,length,50);
}

void sBSP_UART1_ReadBytesBegin(reci_data_end_cb_t end_cb){
    if(end_cb != NULL){
        uart1_reci_data_end_cb = end_cb;
    }else{
        return;
    }
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, UART1_RxBuf, sizeof(UART1_RxBuf));
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
    if (huart->Instance == LPUART1){
        if(uart1_reci_data_end_cb != NULL){
            //这里不清空缓冲区,因为有length在
            uart1_reci_data_end_cb(UART1_RxBuf,Size);
        }
    }
}


#ifdef __cplusplus
}
#endif
