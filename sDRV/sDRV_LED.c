#include "sDRV_LED.h"

#include "sBSP_TIM.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sDRV_LED.c       用于控制WWS板载LED
 * 
 * 使用TIM3_CH1   引脚: LED->PB4
 *
 * 实现亮度控制,循环脉冲,单脉冲,闪烁,呼吸灯效果
 * 
 * 
 */

//使用LED_MODE_ADJ_BRIGHTNESS可调亮度模式
//设置模式sDRV_LED_SetMode(),设置亮度sDRV_LED_SetBrightness()

//使用LED_MODE_PULSE_CYCLE循环脉冲模式
//先设置模式sDRV_LED_SetMode(),亮度sDRV_LED_SetBrightness(),然后sDRV_LED_SetPulseTime_ms()设置亮的时间和整个周期

//使用LED_MODE_PULSE_SINGLE单脉冲模式
//先设置模式sDRV_LED_SetMode(),亮度sDRV_LED_SetBrightness(),然后sDRV_LED_SetPulseTime_ms()设置亮的时间,周期被忽略
//使用sDRV_LED_StartSinglePulse()启动一次单脉冲

//使用LED_MODE_BLINK闪烁模式
//设置模式sDRV_LED_SetMode(),亮度sDRV_LED_SetBrightness(),然后设置闪烁时间sDRV_LED_SetBlinkTime_ms()

//使用LED_MODE_BREATHE呼吸灯模式
//设置模式sDRV_LED_SetMode(),

//先设置时间然后设置模式
//定期调用sDRV_LED_Handler()处理函数,不需要调用设置亮度函数,呼吸周期由调用Handler处理函数的时间间隔决定
 
 
led_t led;


//获取滴答定时器
static inline uint32_t getTick(void){
    return HAL_GetTick();
}

//LED模块初始化
void sDRV_LED_Init(){
    sBSP_TIM2_Init();
    led.mode = LED_MODE_ADJ_BRIGHTNESS;
    led.brightness = 0;     //percent
    led.time_ms = 0;
    led.prev_chrg_ts = 0;
    led.on_ts = 0;
    led.off_ts = 0;
    led.single_pulse_trig = 0;
}

//设置LED模式
void sDRV_LED_SetMode(led_mode_t led_mode){
    //针对呼吸灯模式特别处理
    if(led_mode == LED_MODE_BREATHE){
        //设置输出启动
        sBSP_TIM2_SetCH3PWMEN(1);
        //设置led状态为亮
        led.status = 1;
        //设置模式
        led.mode = led_mode;
        return;
    }
    //处理其他模式
    sBSP_TIM2_SetCH3PWMEN(0);
    led.status = 0;
    led.mode = led_mode;
}

//设置亮度,呼吸灯模式不要设置
void sDRV_LED_SetBrightness(uint8_t led_brightness_percent){
    //限幅
    led_brightness_percent > 100 ? led_brightness_percent = 100 : (void)0;
    led.brightness = led_brightness_percent;
}

//设置闪烁时间
void sDRV_LED_SetBlinkTime_ms(uint16_t time){
    led.time_ms = time;
}

//设置脉冲模式的时间:亮起时长,一个循环的时长
void sDRV_LED_SetPulseTime_ms(uint32_t on_time, uint32_t period_time){
    //参数检查
    if((on_time == 0) || (period_time < on_time)) return;
    led.on_ts = on_time;
    //一个循环的总时长-亮起时间就是熄灭时间
    led.off_ts = period_time - on_time;
}

//启动一次脉冲,只有单脉冲模式生效
void sDRV_LED_StartSinglePulse(){
    if(led.mode == LED_MODE_PULSE_SINGLE){
        //首先设置不输出
        sBSP_TIM2_SetCH3PWMEN(0);
        led.status = 0;
        //清空单次脉冲触发标志位
        led.single_pulse_trig = 0;
    }
}

//伽马校正
static float gamma_correction(float percent_brightness) {
    //将输入的亮度百分比转换为0到1之间的范围
    float normalized_brightness = percent_brightness / 100.0;
    //应用伽马校正公式
    float corrected_brightness = powf(normalized_brightness, GAMMA_VAL);
    //将校正后的亮度映射回PWM占空比范围
    float pwm_duty_cycle = corrected_brightness * 100.0;
    return pwm_duty_cycle;
}

//LED模块处理函数,需要定期调用
void sDRV_LED_Handler(){
    //调光亮度模式
    if(led.mode == LED_MODE_ADJ_BRIGHTNESS){
        //如果亮度为0,就关闭输出
        if(led.brightness == 0){sBSP_TIM2_SetCH3PWMEN(0);return;}
        //启用PWM输出
        sBSP_TIM2_SetCH3PWMEN(1);
        //设置亮度,先伽马矫正之后再给占空比
        sBSP_TIM2_SetCH3PWMDuty(gamma_correction(led.brightness));
    }
    //闪烁模式
    else if(led.mode == LED_MODE_BLINK){
        //如果闪烁时间为0,说明配置错误
        if(led.time_ms == 0) return;
        //如果当前时间-上一次变化的时间 >= 闪烁时间 就说明超时了,该翻转LED状态了
        if((getTick() - led.prev_chrg_ts) >= led.time_ms){ 
            //更新上一次变化时间
            led.prev_chrg_ts = getTick();
            //状态取反
            led.status = !led.status;
            //设置输出
            sBSP_TIM2_SetCH3PWMEN(led.status);
        }
    }
    //循环脉冲模式
    else if(led.mode == LED_MODE_PULSE_CYCLE){
        //配置错误处理
        if(led.on_ts == 0 || led.on_ts == 0) return;
        //和闪烁模式大体一样原理,只是多了根据led的状态选择比较哪个时间
        if ((getTick() - led.prev_chrg_ts) >= (led.status ? led.on_ts : led.off_ts)) {
            led.prev_chrg_ts = getTick();
            led.status = !led.status;
            sBSP_TIM2_SetCH3PWMEN(led.status);
        }

    }
    //单次脉冲模式
    else if(led.mode == LED_MODE_PULSE_SINGLE){
        //这个标志位使用来保证进入两次单脉冲模式的,进入两次才是一个完整脉冲
        if(led.single_pulse_trig >= 2) return;
        //配置错误处理
        if(led.on_ts == 0 || led.on_ts == 0) return;
        //同上,只是只会触发2次
        if ((getTick() - led.prev_chrg_ts) >= (led.status ? led.on_ts : led.off_ts)) {
            led.prev_chrg_ts = getTick();
            led.status = !led.status;
            sBSP_TIM2_SetCH3PWMEN(led.status);
            //标志位++
            led.single_pulse_trig++;
        }
    }
    //呼吸灯模式,周期由用户调用处理函数的时间差决定
    else if(led.mode == LED_MODE_BREATHE){
        //这里,如果brightness为正,说明led正在逐渐亮起,反之,正在逐渐熄灭
        if(led.brightness > 100){
            //进入后半程
            led.brightness = -100;
        }
        if(led.brightness > 0){
            sBSP_TIM2_SetCH3PWMDuty(gamma_correction(led.brightness));
        }else{
            //注意这里的-号
            sBSP_TIM2_SetCH3PWMDuty(gamma_correction(-led.brightness));
        }
        led.brightness++;
    }
}




#ifdef __cplusplus
}
#endif
