#include "sDRV_AHT10.h"
#include "sBSP_softI2C.h"

//这些是AHT10的命令
//初始化
uint8_t aht10_comm_init[]    = {0xE1,0x08,0x00};
//读数据
uint8_t aht10_comm_read[]    = {0x71};
//开始测量
uint8_t aht10_comm_measure[] = {0xAC,0x33,0x00};

//用于接收读到的数据
uint8_t reci_data[6];
//原始数据
uint32_t raw_humi,raw_temp;
//最终数据
float humi,temp;

//引用BSP的I2C句柄
extern I2C_HandleTypeDef hi2c1;

//便于修改接口
void I2C_SendData(uint16_t DevAddress, uint8_t *pData, uint16_t Size){
    sBSP_softI2C_SendData(DevAddress,pData,Size);
}

void I2C_ReciData(uint16_t DevAddress, uint8_t *pData, uint16_t Size){
    sBSP_softI2C_ReciData(DevAddress,pData,Size);
}




//初始化AHT10
void sDRV_AHT10_Init(void){
    //初始化I2C
    sBSP_softI2C_Init();
    HAL_Delay(10);
    //发送初始化命令
    I2C_SendData(AHT10_ADDR,&aht10_comm_init[0],3);
    //外部要等待40ms
}

//开始测量
void sDRV_AHT10_StartMeasure(){
    I2C_SendData(AHT10_ADDR,&aht10_comm_measure[0],3);
}

void sDRV_AHT10_EndMeasure(){
    //等待75ms测量完成之后才可获取温湿度值
    I2C_ReciData(AHT10_ADDR,&reci_data[0],6);
    
    raw_humi = (reci_data[1] << 12) | (reci_data[2] << 4) | (reci_data[3] >> 4);
    raw_temp = ((reci_data[3] & 0x0F) << 16) | (reci_data[4] << 8) | reci_data[5];
    
    humi = (raw_humi * 100.0) / (1 << 20);
    temp = ((raw_temp * 200.0) / (1 << 20)) - 50;
}

float sDRV_AHT10_GetHumi(){
    return humi;
}

float sDRV_AHT10_GetTemp(){
    return temp;
}



