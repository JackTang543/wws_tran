#include "stm32l0xx_hal.h"

#ifndef __SDRV_BMP280_H__
#define __SDRV_BMP280_H__

#ifdef __cplusplus
extern "C"{
#endif

//使用超高解析度
#define sDRV_BMP280_USE_MAX_RESOLUTION

//使用定点计算,如果不强调,默认浮点运算
#define sDRV_BMP280_USE_FIXED_POINT_COMPE

//这里顺序有要求
typedef struct{
    uint8_t press_msb;
    uint8_t press_lsb;
    uint8_t press_xlsb;
    uint8_t temp_msb;
    uint8_t temp_lsb;
    uint8_t temp_xlsb;
    uint32_t temp;
    uint32_t press;
    uint8_t cal_val[26];
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
}sDrv_BMP280_t;


HAL_StatusTypeDef sDRV_BMP280_Init();
HAL_StatusTypeDef sDRV_BMP280_GetMeasure();
double sDRV_BMP280_GetPress();
double sDRV_BMP280_GetTemp();


#ifdef __cplusplus
}
#endif
#endif
