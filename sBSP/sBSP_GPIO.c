#include "sBSP_GPIO.h"

#include "sDRV_Si24R1.h"



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    if(GPIO_Pin == GPIO_PIN_7){
        //PA7 -> EXTI 7
        sDRV_Si24R1_IrqHandler();
    }
}


