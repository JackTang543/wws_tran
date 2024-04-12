/* 
 * Wireless Weather Station Tran
 * 无线气象站发送端
 * By Sightseer. inHNIP9607Lab
 * 
 * TIME:2024.04.11
 * 描述:所有外设的驱动完工+Demo完工
 * v1.0
 * 
 * TIME:2024.04.12
 * 描述:低功耗优化完工
 * v1.1
 *
*/

#include "main.h"

//软件版本
#define SW_VERSION      "ver1.1"



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    if(GPIO_Pin == GPIO_PIN_7){
        sDRV_Si24R1_IrqHandler();
    }
}

void recied_data(Si24R1_Data_t* data){
    sDRV_UART_Printf("recied data:\"%s\",len:%d,ppp:%d,rssi:%d\n",data->msg,data->len,data->ppp,data->rssi);
}


void ReciCb(uint8_t* pReciData,uint16_t length){
    sBSP_UART1_SendBytes(pReciData,length);
    sBSP_UART1_ReadBytesBegin(ReciCb);
}

typedef struct{
    float aht10_humi;
    float aht10_temp;
    float bmp280_pres;
    float bmp280_temp;
    float temt_mv;
    float vbat;
    uint16_t status;
}data_packet_t;

data_packet_t packet;



int main(void){
    HAL_Init();
    //初始化时钟系统
    sBSP_SYS_InitRCC();
    //初始化串口
    sDRV_UART_Init(115200);
    

    //串口发送当前版本信息
    sDRV_UART_Printf("Wireless Weather Station Tran bySightseer.\n");
    sDRV_UART_Printf("%s Compile time: %s %s\n",SW_VERSION,__DATE__,__TIME__);

    //发送当前MCU频率
    uint32_t sys_freq = HAL_RCC_GetSysClockFreq();
    sDRV_UART_Printf("Freq: %d Hz\n",sys_freq);
    
    //初始化LED
    sFUNC_InitLED();
    sDRV_UART_Printf("LED init OK.\n");
    //初始化AHT10
    sDRV_AHT10_Init();
    sDRV_UART_Printf("AHT10 init OK.\n");
    //初始化BMP280
    sDRV_BMP280_Init();
    sDRV_UART_Printf("BMP280 init OK.\n");
    //初始化TEMT6000(ADC)
    sBSP_ADC_Init();
    sDRV_UART_Printf("TEMT6000(ADC) init OK.\n");
    //初始化Si24R1
    sFUNC_Init2d4GHz();
    sDRV_UART_Printf("Si24R1 init OK.\n");

    sDRV_UART_Printf("all func init done.\n");
    

    
    sDRV_UART_Printf("\n");
    
    sDRV_Si24R1_SetIdleTX();
    sDRV_UART_Printf("Si24R1 start idle tx!\n");
    uint8_t tx_buf[32] = "";


    float aht10_humi = 0.0f;
    float aht10_temp = 0.0f;

    float bmp280_pres = 0.0f;
    float bmp280_temp = 0.0f;

    float temt_mv = 0.0f;
    float vbat = 0.0f;



    while(1){

        sDRV_AHT10_BeginMeasure();
        sDRV_BMP280_GetMeasure();
        HAL_Delay(100);
        sDRV_AHT10_EndMeasure();

        packet.aht10_humi = sDRV_AHT10_GetHumi();
        packet.aht10_temp = sDRV_AHT10_GetTemp();
        packet.bmp280_pres = (float)sDRV_BMP280_GetPress();
        packet.bmp280_temp = (float)sDRV_BMP280_GetTemp();
        packet.temt_mv = sDRV_TEMT_GetLight();
        packet.vbat = sAPP_Func_GetVbat();
        
        memcpy(tx_buf,&packet,sizeof(data_packet_t));

        // sDRV_UART_Printf("AHT10 HUMI: %.1f %%RH,AHT10 TEMP: %.1f degC\n",aht10_humi,aht10_temp);
        // sDRV_UART_Printf("BMP280 PRESS: %.3f HPa,BMP280 TEMP: %.2f degC\n",bmp280_pres / 100,bmp280_temp);
        // sDRV_UART_Printf("LIGHT:%.2f mV,Vbat:%.2f mV\n",sDRV_TEMT_GetLight(),sAPP_Func_GetVbat());

        //sDRV_Si24R1_TranDataNoAck(tx_buf,sizeof(tx_buf) / sizeof(tx_buf[0]));
        sDRV_Si24R1_TranDataNoAck(tx_buf,sizeof(tx_buf) / sizeof(tx_buf[0]));
        
        sDRV_UART_Printf("Si24R1 txed!\n");

        HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
        HAL_Delay(500);
    }
}


// int main(void){
//     HAL_Init();

//     sBSP_SYS_InitRCC();
//     sDRV_UART_Init(115200);

//     // __GPIOB_CLK_ENABLE();
//     // GPIO_InitTypeDef gpio = {0};
//     // gpio.Mode = GPIO_MODE_OUTPUT_PP;
//     // gpio.Pull = GPIO_NOPULL;
//     // gpio.Speed = GPIO_SPEED_FREQ_LOW;
//     // gpio.Pin = GPIO_PIN_0;
//     // gpio.Alternate = 0x0;

//     // HAL_GPIO_Init(GPIOB,&gpio);
    
//     sDRV_UART_Printf("Hello,From STM32L031!\n");
//     sBSP_UART1_ReadBytesBegin(ReciCb);

//     uint32_t sys_freq = HAL_RCC_GetSysClockFreq();
//     sDRV_UART_Printf("Freq: %d Hz\n",sys_freq);

//     sDRV_LED_Init();
//     sDRV_LED_SetMode(LED_MODE_BREATHE);
//     //sDRV_LED_SetBrightness(20);

//     //sBSP_TIM2_Init();
//     //sBSP_TIM2_SetCH3PWMEN(1);
//     sDRV_AHT10_Init();
//     sDRV_BMP280_Init();

//    //过滤掉前两个错误数据
//    sDRV_BMP280_GetMeasure();
//    sDRV_BMP280_GetTemp();
//    sDRV_BMP280_GetPress();
//    sDRV_BMP280_GetMeasure();
//    sDRV_BMP280_GetTemp();
//    sDRV_BMP280_GetPress();

    
//     HAL_Delay(100);
    
//     while (1){
//         //HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
//         //sDRV_LED_Handler();

//         sDRV_BMP280_GetMeasure();
        
//         sDRV_UART_Printf("TEMP: %.2f ℃,PRESS: %.3f Pa ",sDRV_BMP280_GetTemp(),sDRV_BMP280_GetPress());
        
//         sDRV_AHT10_BeginMeasure();
//         HAL_Delay(100);
//         sDRV_AHT10_EndMeasure();

//         sDRV_UART_Printf("   AHT10 TEMP: %.1f ℃,HUMI: %.1f %%RH",sDRV_AHT10_GetTemp(),sDRV_AHT10_GetHumi());
        
//         sDRV_UART_Printf("\n");
        
//         HAL_Delay(300);
//     }
// }


void sERROR_Handler(){
    while(1){
        __NOP();
    }
}


