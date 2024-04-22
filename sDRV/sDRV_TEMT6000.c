#include "sDRV_TEMT6000.h"
#include "sBSP_ADC.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEMT_CH     ADC_CHANNEL_1

#define AVERAGE_NUM     (8)

void sDRV_TEMT_Init(){
    __GPIOA_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = 0x0;
    //初始化TEMT6000的VCC
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);

}

void sDRV_TEMT_Startup(){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,1);
}

void sDRV_TEMT_Shutdown(){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,0);
}

float sDRV_TEMT_GetLight(){
    uint16_t raw = 0;
    for(uint8_t i = 0;i < AVERAGE_NUM;i++){
        raw += sBSP_ADC_GetChannel(TEMT_CH);
    }


    float raw_val = ((float)raw / AVERAGE_NUM) / 4095.0f * 3300.0f;


    return raw_val;
}



#ifdef __cplusplus
}
#endif
