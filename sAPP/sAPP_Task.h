#ifndef __SAPP_TASK_H__
#define __SAPP_TASK_H__

#include "stm32l0xx_hal.h"

#include "main.h"

#ifdef __cplusplus
extern "C"{
#endif


//数据结构体
typedef struct{
    float aht10_humi;
    float aht10_temp;
    float bmp280_pres;
    float bmp280_temp;
    float temt_mv;
    float vbat_mv;
    uint16_t status;
}sAPP_Task_DataPacket_t;



void sAPP_Task_DataGet(sAPP_Task_DataPacket_t* pData);
void sAPP_Task_DataSend(sAPP_Task_DataPacket_t* pData);


#ifdef __cplusplus
}
#endif
#endif
