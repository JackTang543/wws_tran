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

#include "sDRV_UART.h"
#include "sDRV_LED.h"
#include "sDRV_BMP280.h"
#include "sDRV_AHT10.h"



#include "sBSP_TIM.h"


#include "sBSP_SYS.h"



void ReciCb(uint8_t* pReciData,uint16_t length){
    sBSP_UART1_SendBytes(pReciData,length);
    sBSP_UART1_ReadBytesBegin(ReciCb);
}


int main(void){
    HAL_Init();

    sBSP_SYS_InitRCC();
    sDRV_UART_Init(115200);

    // __GPIOB_CLK_ENABLE();
    // GPIO_InitTypeDef gpio = {0};
    // gpio.Mode = GPIO_MODE_OUTPUT_PP;
    // gpio.Pull = GPIO_NOPULL;
    // gpio.Speed = GPIO_SPEED_FREQ_LOW;
    // gpio.Pin = GPIO_PIN_0;
    // gpio.Alternate = 0x0;

    // HAL_GPIO_Init(GPIOB,&gpio);
    
    sDRV_UART_Printf("Hello,From STM32L031!\n");
    sBSP_UART1_ReadBytesBegin(ReciCb);

    uint32_t sys_freq = HAL_RCC_GetSysClockFreq();
    sDRV_UART_Printf("Freq: %d Hz\n",sys_freq);

    sDRV_LED_Init();
    sDRV_LED_SetMode(LED_MODE_BREATHE);
    //sDRV_LED_SetBrightness(20);

    //sBSP_TIM2_Init();
    //sBSP_TIM2_SetCH3PWMEN(1);
    sDRV_AHT10_Init();
    sDRV_BMP280_Init();

   //过滤掉前两个错误数据
   sDRV_BMP280_GetMeasure();
   sDRV_BMP280_GetTemp();
   sDRV_BMP280_GetPress();
   sDRV_BMP280_GetMeasure();
   sDRV_BMP280_GetTemp();
   sDRV_BMP280_GetPress();

    
    HAL_Delay(100);
    
    while (1){
        //HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
        //sDRV_LED_Handler();

        sDRV_BMP280_GetMeasure();
        
        sDRV_UART_Printf("TEMP: %.2f ℃,PRESS: %.3f Pa ",sDRV_BMP280_GetTemp(),sDRV_BMP280_GetPress());
        
        sDRV_AHT10_BeginMeasure();
        HAL_Delay(100);
        sDRV_AHT10_EndMeasure();

        sDRV_UART_Printf("   AHT10 TEMP: %.1f ℃,HUMI: %.1f %%RH",sDRV_AHT10_GetTemp(),sDRV_AHT10_GetHumi());
        
        sDRV_UART_Printf("\n");
        
        HAL_Delay(300);
    }
}


void sERROR_Handler(){
    while(1){
        __NOP();
    }
}


