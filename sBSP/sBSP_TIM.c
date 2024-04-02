#include "sBSP_TIM.h"

#ifdef __cplusplus
extern "C" {
#endif

//本文件资源分配:
//TIM2.CH3 -> LED
//
//

//TIM2的句柄
TIM_HandleTypeDef hTIM2;

//TIM2 : ARR=999,PSC=15,FREQ=32MHZ计算得2KHZ
//自动重装载值
uint32_t TIM2_ARR_val = 999;
//预分频值
uint32_t TIM2_PSC_val = 15;



//初始化TIM2
void sBSP_TIM2_Init(){
    hTIM2.Instance = TIM2;
    //PSC
    hTIM2.Init.Prescaler = TIM2_PSC_val;
    //ARR
    hTIM2.Init.Period = TIM2_ARR_val;
    //向上计数模式
    hTIM2.Init.CounterMode = TIM_COUNTERMODE_UP;
    //初始化TIM的PWM模式
    HAL_TIM_PWM_Init(&hTIM2);

    //用于控制输出比较
    TIM_OC_InitTypeDef TIM2_OC_Init = {0};
    //PWM模式1
    TIM2_OC_Init.OCMode = TIM_OCMODE_PWM1;
    //初始化为占空比50%
    TIM2_OC_Init.Pulse = TIM2_ARR_val / 2;
    //输出极性LOW
    TIM2_OC_Init.OCPolarity = TIM_OCPOLARITY_HIGH;
    
    //配置CH1为PWM
    HAL_TIM_PWM_ConfigChannel(&hTIM2,&TIM2_OC_Init,TIM_CHANNEL_3);
    //先不启动输出
    HAL_TIM_PWM_Stop(&hTIM2,TIM_CHANNEL_3);
}

//设置PWM频率
void sBSP_TIM2_SetPWMFreq(int32_t freq){
    uint32_t arr = 0;
    //越界保护
    freq > 5000?freq = 5000:freq;
    freq < 200 ?freq = 200 :freq;
    arr = -(16 * freq - 72000000) / (16 * freq);
    //设置ARR的值
    __HAL_TIM_SET_AUTORELOAD(&hTIM2,arr);
}

//设置PWM占空比
inline void sBSP_TIM2_SetCH3PWMDuty(uint8_t percent){
    //设置比较值
    __HAL_TIM_SET_COMPARE(&hTIM2,TIM_CHANNEL_3,((float)TIM2_ARR_val * ((float)percent / 100.0)));
}

//选择TIM2的CH3是否输出
inline void sBSP_TIM2_SetCH3PWMEN(uint8_t en){
    if(en){
        HAL_TIM_PWM_Start(&hTIM2,TIM_CHANNEL_3);
    }else{
        HAL_TIM_PWM_Stop(&hTIM2,TIM_CHANNEL_3);
    }
}





#ifdef __cplusplus
}
#endif
