#ifndef __SDRV_BMP280_H__
#define __SDRV_BMP280_H__

//sDRV_BMP280.h
//Sightseer's BMP280驱动
//v2.0 2024.04.16 inHNIP9607

#include "stm32l0xx_hal.h"

#ifdef __cplusplus
extern "C"{
#endif

//BMP280的模式
typedef enum{
    BMP280_MODE_SLEEP  = 0b00,
    BMP280_MODE_FORCED = 0b01,
    BMP280_MODE_NORMAL = 0b11
}BMP280_Mode_t;

//Standby时间
typedef enum{
    BMP280_TSB_0D5MS   = 0b000,
    BMP280_TSB_62D5MS  = 0b001,
    BMP280_TSB_125MS   = 0b010,
    BMP280_TSB_250MS   = 0b011,
    BMP280_TSB_500MS   = 0b100,
    BMP280_TSB_1000MS  = 0b101,
    BMP280_TSB_2000MS  = 0b110,
    BMP280_TSB_4000MS  = 0b111
}BMP280_TSB_t;

//过采样
typedef enum{
    BMP280_OSRS_SKIP   = 0b000,
    BMP280_OSRS_X1     = 0b001,
    BMP280_OSRS_X2     = 0b010,
    BMP280_OSRS_X4     = 0b011,
    BMP280_OSRS_X8     = 0b100,
    BMP280_OSRS_X16    = 0b101
}BMP280_OSRS_t;

//滤波器系数
typedef enum{
    BMP280_FILTER_OFF  = 0b000,
    BMP280_FILTER_2    = 0b001,
    BMP280_FILTER_4    = 0b010,
    BMP280_FILTER_8    = 0b011,
    BMP280_FILTER_16   = 0b100,
}BMP280_FILTER_t;

//配置结构体
typedef struct{
    BMP280_Mode_t mode    : 2;
    BMP280_TSB_t  tsb     : 3;
    BMP280_OSRS_t osrs_t  : 3;
    BMP280_OSRS_t osrs_p  : 3;
    BMP280_FILTER_t filter: 3;
}sDRV_BMP280_Conf_t;

//BMP280数据和校准值,这里顺序有要求
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
}sDRV_BMP280_t;


int8_t sDRV_BMP280_Init(sDRV_BMP280_Conf_t* _conf);
int8_t sDRV_BMP280_StartMeasure();
int8_t sDRV_BMP280_EndMeasure();
double sDRV_BMP280_GetPress();
double sDRV_BMP280_GetTemp();

#ifdef __cplusplus
}
#endif

#endif

