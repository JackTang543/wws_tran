#include "sBSP_GPIO.h"

#include "sDRV_Si24R1.h"


void sBSP_GPIO_LED_Init(){
    //LED -> PB0
    __GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pin = GPIO_PIN_0;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_LOW;
    gpio_init.Alternate = 0;
    HAL_GPIO_Init(GPIOB,&gpio_init);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,0);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    if(GPIO_Pin == GPIO_PIN_7){
        //PA7 -> EXTI 7
        sDRV_Si24R1_IrqHandler();
    }
}


