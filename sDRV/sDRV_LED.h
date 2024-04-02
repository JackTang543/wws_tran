#include "stm32l0xx_hal.h"
#include "math.h"


#ifndef __SDRV_LED_H__
#define __SDRV_LED_H__


//伽马矫正值
#define GAMMA_VAL (2.2f)

typedef enum{
    LED_MODE_ADJ_BRIGHTNESS = 0,
    LED_MODE_PULSE_CYCLE,
    LED_MODE_PULSE_SINGLE,
    LED_MODE_BLINK,
    LED_MODE_BREATHE
}led_mode_t;

typedef enum{
    LED_STATUS_DARK = 0,
    LED_STATUS_LIGHT = 1
}led_status;

typedef struct{
    led_mode_t mode;
    int8_t brightness;
    uint16_t time_ms;
    uint32_t prev_chrg_ts;
    uint32_t on_ts;
    uint32_t off_ts;
    led_status status;
    uint8_t single_pulse_trig;
}led_t;

void sDRV_LED_Init();
void sDRV_LED_SetMode(led_mode_t led_mode);
void sDRV_LED_SetBrightness(uint8_t led_brightness_percent);
void sDRV_LED_StartSinglePulse();
void sDRV_LED_SetPulseTime_ms(uint32_t on_time, uint32_t off_time);
void sDRV_LED_SetBlinkTime_ms(uint16_t time);
void sDRV_LED_Handler();


#endif
