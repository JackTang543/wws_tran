#include "sAPP_Task.h"



#define __SET_DEBUG_LED(__LV__)     do{HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,__LV__);}while(0)
#define DEBUG_EN

void sAPP_Task_DataGet(sAPP_Task_DataPacket_t* pData){
    //__SET_DEBUG_LED(1);                 //测量LED脉宽即可知道处理数据用了多久

    sDRV_AHT10_StartMeasure();          //启动AHT10的测量
    sDRV_BMP280_StartMeasure();         //启动BMP280的测量
    sDRV_TEMT_Startup();                //启动TEMT6000的电源
    HAL_Delay(75);                      //等待器件测量完成和TEMT的电流稳定
    sDRV_AHT10_EndMeasure();            //结束AHT10的测量(获取测量值)
    sDRV_BMP280_EndMeasure();           //结束BMP280的测量

    //写入数据
    pData->aht10_humi  = sDRV_AHT10_GetHumi();
    pData->aht10_temp  = sDRV_AHT10_GetTemp();
    pData->bmp280_temp = (float)sDRV_BMP280_GetTemp();  //注意这里要先获取温度
    pData->bmp280_pres = (float)sDRV_BMP280_GetPress() / 100; //获取温度得到t_fine,这个值要给气压用
    pData->temt_mv     = (sDRV_TEMT_GetLight() / 3300) * 100;
    pData->vbat_mv     = sAPP_Func_GetVbat() * 2;       //x2是分压系数

    sDRV_TEMT_Shutdown();               //关闭TEMT6000电源

    #ifdef DEBUG_EN
    sDRV_UART_Printf("AHT10 HUMI: %.1f %%RH,AHT10 TEMP: %.1f degC\n",pData->aht10_humi,pData->aht10_temp);
    sDRV_UART_Printf("BMP280 PRESS: %.3f HPa,BMP280 TEMP: %.2f degC\n",pData->bmp280_pres,pData->bmp280_temp);
    sDRV_UART_Printf("LIGHT:%.2f mV,Vbat:%.2f mV\n",pData->temt_mv,pData->vbat_mv);
    #endif

    
    //__SET_DEBUG_LED(0);
}

//发送数据的缓冲区
uint8_t tx_buf[32];

#include "string.h"

void sAPP_Task_DataSend(sAPP_Task_DataPacket_t* pData){
    //sDRV_Si24R1_SetStandby();
    //sDRV_Si24R1_SetShutdown();

    sDRV_Si24R1_SetStandby();
    HAL_Delay(5);
    sDRV_Si24R1_SetIdleTX();
    HAL_Delay(5);
    //把数据复制到发送缓冲区
    memcpy(tx_buf,pData,sizeof(sAPP_Task_DataPacket_t));
    //发送数据
    sDRV_Si24R1_TranDataNoAck(tx_buf,sizeof(tx_buf) / sizeof(tx_buf[0]));
    sDRV_Si24R1_SetStandby();
    HAL_Delay(5);
    sDRV_Si24R1_SetShutdown();
    HAL_Delay(5);

    //sDRV_UART_Printf("Si24R1 txed!\n");
}



