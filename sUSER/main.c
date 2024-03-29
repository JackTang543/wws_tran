/* 
 * Wireless Weather Station Tran
 * 无线气象站发送端
 * By Sightseer.
 * TIME:2024.03.29
 * 
 * v1.0
 *
*/



#include "main.h"

#include "stm32l0xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif


int main(void){
    HAL_Init();
    __GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef gpio = {0};
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Pin = GPIO_PIN_0;
    gpio.Alternate = 0x0;

    HAL_GPIO_Init(GPIOB,&gpio);
    

    
    while (1){
        HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
        HAL_Delay(500);
    }
}


#ifdef __cplusplus
}
#endif
