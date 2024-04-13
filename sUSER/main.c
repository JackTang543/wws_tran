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
*/

#include "main.h"

//软件版本
#define SW_VERSION      "ver1.1"

#define DEBUG_EN


#define DEBUG_LED_GPIO         GPIOB 
#define DEBUG_LED_GPIO_PIN     GPIO_PIN_0

#define __SET_DEBUG_LED(__LV__)     do{HAL_GPIO_WritePin(DEBUG_LED_GPIO,DEBUG_LED_GPIO_PIN,__LV__);}while(0)


//Si24R1接收到数据中断
void recied_data(Si24R1_Data_t* data){
    sDRV_UART_Printf("recied data:\"%s\",len:%d,ppp:%d,rssi:%d\n",data->msg,data->len,data->ppp,data->rssi);
}

//串口接收到数据中断
void ReciCb(uint8_t* pReciData,uint16_t length){
    sBSP_UART1_SendBytes(pReciData,length);
    sBSP_UART1_ReadBytesBegin(ReciCb);
}

//数据结构体打包
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

//发送数据的缓冲区
uint8_t tx_buf[32];

void data_process(){
    //测量LED脉宽即可知道处理数据用了多久
    #ifdef DEBUG_EN
    __SET_DEBUG_LED(1);
    #endif

    //启动AHT10的测量
    sDRV_AHT10_BeginMeasure();
    //启动BMP280的测量
    sDRV_BMP280_GetMeasure();
    HAL_Delay(100);
    sDRV_AHT10_EndMeasure();

    //获取数据
    packet.aht10_humi = sDRV_AHT10_GetHumi();
    packet.aht10_temp = sDRV_AHT10_GetTemp();
    packet.bmp280_pres = (float)sDRV_BMP280_GetPress();
    packet.bmp280_temp = (float)sDRV_BMP280_GetTemp();
    packet.temt_mv = sDRV_TEMT_GetLight();
    packet.vbat = sAPP_Func_GetVbat();

    //把数据复制到发送缓冲区
    memcpy(tx_buf,&packet,sizeof(data_packet_t));

    //发送数据
    sDRV_Si24R1_TranDataNoAck(tx_buf,sizeof(tx_buf) / sizeof(tx_buf[0]));
    sDRV_UART_Printf("Si24R1 txed!\n");

    #ifdef DEBUG_EN
    sDRV_UART_Printf("AHT10 HUMI: %.1f %%RH,AHT10 TEMP: %.1f degC\n",packet.aht10_humi,packet.aht10_temp);
    sDRV_UART_Printf("BMP280 PRESS: %.3f HPa,BMP280 TEMP: %.2f degC\n",packet.bmp280_pres / 100,packet.bmp280_temp);
    sDRV_UART_Printf("LIGHT:%.2f mV,Vbat:%.2f mV\n",packet.temt_mv,packet.vbat);
    #endif

    #ifdef DEBUG_EN
    __SET_DEBUG_LED(0);
    #endif
}

void init_system(){
    
    //初始化时钟系统
    sBSP_SYS_InitRCC();
    //初始化串口
    sDRV_UART_Init(115200);

    sBSP_SYS_PWR_Init();

    //串口发送当前版本信息
    // sDRV_UART_Printf("Wireless Weather Station Tran bySightseer.\n");
    // sDRV_UART_Printf("%s Compile time: %s %s\n",SW_VERSION,__DATE__,__TIME__);
    //发送当前MCU频率
    uint32_t sys_freq = HAL_RCC_GetSysClockFreq();
    sDRV_UART_Printf("Freq: %d Hz\n",sys_freq);
    
    //初始化LED
    sFUNC_InitLED();
    __SET_DEBUG_LED(0);
    // sDRV_UART_Printf("LED init OK.\n");
    // //初始化AHT10
    // sDRV_AHT10_Init();
    // sDRV_UART_Printf("AHT10 init OK.\n");
    // //初始化BMP280
    // sDRV_BMP280_Init();
    // sDRV_UART_Printf("BMP280 init OK.\n");
    // //初始化TEMT6000(ADC)
    // sBSP_ADC_Init();
    // sDRV_UART_Printf("TEMT6000(ADC) init OK.\n");
    // //初始化Si24R1
    // sFUNC_Init2d4GHz();
    // sDRV_UART_Printf("Si24R1 init OK.\n");

    // sDRV_UART_Printf("all func init done.\n\n");

    // //Si24R1进入空闲发送模式
    // sDRV_Si24R1_SetIdleTX();
    // sDRV_UART_Printf("Si24R1 start idle tx!\n");
}

extern RTC_HandleTypeDef hrtc;

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Clear Wake Up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
}

int main(void){
    HAL_Init();
    sBSP_RTC_Init();
    init_system();

    sBSP_RTC_SetTime(10,20,00);
    sBSP_RTC_SetDate(2024,RTC_MONTH_APRIL,13,RTC_WEEKDAY_SATURDAY);
    
    // RTC_AlarmTypeDef sAlarm = {0};
    // sAlarm.Alarm = RTC_ALARM_A;
    // sAlarm.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
    // sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    // sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES; // 只在特定秒数时触发
    // sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    // sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
    // sAlarm.AlarmTime.Hours = 0;
    // sAlarm.AlarmTime.Minutes = 30;
    // sAlarm.AlarmTime.Seconds = 0;
    // sAlarm.AlarmTime.SubSeconds = 0;
    // HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);

    RTC_TimeTypeDef time = {0};
    RTC_DateTypeDef date = {0};



    while(1){
        init_system();

        sBSP_RTC_GetTime(&time);
        sBSP_RTC_GetDate(&date);
        
        sDRV_UART_Printf("20%02d.%02d.%02d  %02d:%02d:%02d week:%d\n",      \
        date.Year,date.Month,date.Date,time.Hours,time.Minutes,time.Seconds,date.WeekDay);

        RTC_AlarmTypeDef sAlarm = {0};
        sAlarm.Alarm = RTC_ALARM_A;
        sAlarm.AlarmDateWeekDay = date.WeekDay;
        sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
        sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES; // 只在特定秒数时触发
        sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
        sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
        sAlarm.AlarmTime.Hours = time.Hours;
        sAlarm.AlarmTime.Minutes = time.Minutes;
        sAlarm.AlarmTime.Seconds = (time.Seconds + 10) % 60;
        sDRV_UART_Printf("set seconds:%d",(time.Seconds + 10) % 60);
        sAlarm.AlarmTime.SubSeconds = 0;
        HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);


        //sDRV_UART_Printf("Wake up!\n");                                                        

        //HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
        //HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0xBE8C, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
        sBSP_SYS_PWR_Init();
        HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

        
        //HAL_Delay(1000);
    }
}


void sERROR_Handler(){
    while(1){
        __NOP();
    }
}


