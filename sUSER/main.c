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
 * 描述:低功耗优化Demo完工
 * v1.1
 *
 * TIME:2024.04.13
 * 描述:RTC定时唤醒+数据测量发送
 * v1.2
 *
 * TIME:2024.04.21
 * 描述:更新BMP280驱动,引入低功耗模式控制,把顺序操作改为Mem操作,优化工程结构
 * v1.3
 * 
 * 
*/

#include "main.h"

//软件版本
#define SW_VERSION      "ver1.3"

#define DEBUG_LED_GPIO         GPIOB 
#define DEBUG_LED_GPIO_PIN     GPIO_PIN_0



// //串口接收到数据中断
// void ReciCb(uint8_t* pReciData,uint16_t length){
//     sBSP_UART1_SendBytes(pReciData,length);
//     sBSP_UART1_ReadBytesBegin(ReciCb);
// }

sAPP_Task_DataPacket_t data_packet;






int main(void){
    //**************************初始化系统**************************
    HAL_Init();
    //初始化时钟系统
    sBSP_SYS_InitRCC();
    //初始化串口
    sDRV_UART_Init(115200);
    //初始化RTC
    sBSP_RTC_Init();
    //初始化电源
    sBSP_SYS_PWR_Init();
    //初始化LED
    sBSP_GPIO_LED_Init();
    //初始化ADC
    sBSP_ADC_Init();

    //串口发送当前版本信息
    sDRV_UART_Printf("Wireless Weather Station Tran bySightseer.\n");
    sDRV_UART_Printf("%s Compile time: %s %s\n",SW_VERSION,__DATE__,__TIME__);
    //发送当前MCU频率
    uint32_t sys_freq = HAL_RCC_GetSysClockFreq();
    sDRV_UART_Printf("Freq: %d Hz\n",sys_freq);

    //**************************初始化外设**************************
    //初始化AHT10
    sDRV_AHT10_Init();
    //初始化BMP280
    sFUNC_InitBMP280();
    //初始化TEMT6000
    sDRV_TEMT_Init();
    //初始化Si24R1
    sFUNC_2D4GHZ_PWR_Init();
    sFUNC_Init2d4GHz();
    sDRV_UART_Printf("all func init done.\n\n");
    //初始化RTC时间
    sBSP_RTC_SetTime(16,00,00);
    sBSP_RTC_SetDate(2024,RTC_MONTH_APRIL,21,RTC_WEEKDAY_SUNDAY);
    

    sDRV_Si24R1_SetStandby();
    sDRV_Si24R1_SetShutdown();

    sDRV_Si24R1_SetStandby();
    sDRV_Si24R1_SetIdleTX();
    sDRV_UART_Printf("Si24R1 start idle tx!\n");

    RTC_TimeTypeDef time = {0};
    RTC_DateTypeDef date = {0};

    sFUNC_Set2D4GHZ_PWR(0);
    

    while(1){
        sBSP_RTC_GetTime(&time);
        sBSP_RTC_GetDate(&date);
        
        sDRV_UART_Printf("20%02d.%02d.%02d  %02d:%02d:%02d week:%d\n",      \
        date.Year,date.Month,date.Date,time.Hours,time.Minutes,time.Seconds,date.WeekDay);

        //sFUNC_Set2D4GHZ_PWR(0);
        

        sAPP_Task_DataGet(&data_packet);
        sAPP_Task_DataSend(&data_packet);
        //sFUNC_Set2D4GHZ_PWR(1);

        //sFUNC_SetRTCAlarmA(5);
        //sBSP_SYS_PWR_Init();
        //HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

        HAL_Delay(5000);
    }
}


void sERROR_Handler(){
    while(1){
        __NOP();
    }
}


