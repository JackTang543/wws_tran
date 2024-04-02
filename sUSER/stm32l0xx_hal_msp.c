#include "stm32l0xx_hal.h"
#include "main.h"

void HAL_MspInit(){
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
}


void HAL_UART_MspInit(UART_HandleTypeDef* huart){
    if(huart->Instance == LPUART1){
        //LPUART1.RX -> PA3
        //LPUART1.TX -> PA2
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitTypeDef gpio_init = {0};
        gpio_init.Mode = GPIO_MODE_AF_PP;
        gpio_init.Pin = GPIO_PIN_2 | GPIO_PIN_3;
        gpio_init.Pull = GPIO_NOPULL;
        gpio_init.Speed = GPIO_SPEED_HIGH;
        gpio_init.Alternate = GPIO_AF6_LPUART1;
        HAL_GPIO_Init(GPIOA,&gpio_init);

        __HAL_RCC_LPUART1_CLK_ENABLE();
        HAL_NVIC_SetPriority(LPUART1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(LPUART1_IRQn);
    }
}


//重写HAL库的MSP
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim){
    if(htim->Instance == TIM2){
        
        //TIM2.CH3->PB0
        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitTypeDef gpio_init = {0};
        gpio_init.Mode = GPIO_MODE_AF_PP;
        gpio_init.Pin = GPIO_PIN_0;
        gpio_init.Pull = GPIO_NOPULL;
        gpio_init.Speed = GPIO_SPEED_HIGH;
        gpio_init.Alternate = GPIO_AF5_TIM2;
        HAL_GPIO_Init(GPIOB,&gpio_init);

        __TIM2_CLK_ENABLE();
    }
}

//重写HAL的MSP(MCU Support Package)Init
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c){
    if(hi2c->Instance == I2C1){
        //! 先配置时钟,再配置外设!!!
        __GPIOA_CLK_ENABLE();
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
        HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
        __I2C1_CLK_ENABLE();
        HAL_NVIC_SetPriority(I2C1_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(I2C1_IRQn);
    }
}



