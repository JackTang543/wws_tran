#include "sAPP_Func.h"

extern RTC_HandleTypeDef hrtc;

//检查是否是闰年
static uint8_t isLeapYear(uint16_t year) {
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
        return 1;
    }
    return 0;
}

//获取月份的天数
static uint8_t getDaysInMonth(uint8_t month, uint8_t year) {
    static const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return daysInMonth[month - 1];
}

//HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
//HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0xBE8C, RTC_WAKEUPCLOCK_RTCCLK_DIV16);


void sFUNC_SetRTCAlarmA(uint32_t seconds){
    RTC_AlarmTypeDef sAlarm = {0};
    //获取当前时间
    RTC_TimeTypeDef time = {0}; RTC_DateTypeDef date = {0};
    sBSP_RTC_GetTime(&time); sBSP_RTC_GetDate(&date);

    //时间计算
    time.Seconds += seconds;
    time.Minutes += time.Seconds / 60;
    time.Seconds %= 60;
    time.Hours += time.Minutes / 60;
    time.Minutes %= 60;

    //日期计算
    uint16_t daysToAdd = time.Hours / 24;
    time.Hours %= 24;
    date.Date += daysToAdd;

    while (date.Date > getDaysInMonth(date.Month, date.Year)) {
        date.Date -= getDaysInMonth(date.Month, date.Year);
        date.Month++;
        if (date.Month > 12) {
            date.Month = 1;
            date.Year++;
        }
    }

    //设置闹钟
    sAlarm.AlarmTime.Hours = time.Hours;
    sAlarm.AlarmTime.Minutes = time.Minutes;
    sAlarm.AlarmTime.Seconds = time.Seconds;
    sAlarm.AlarmTime.SubSeconds = 0;

    sAlarm.AlarmTime.TimeFormat = time.TimeFormat;
    sAlarm.AlarmDateWeekDay = date.Date;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
    sAlarm.Alarm = RTC_ALARM_A;
    HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);

    //sDRV_UART_Printf("Alarm A set for %d-%d-%d %02d:%02d:%02d\n",   \
    date.Year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds);
}

void sFUNC_2D4GHZ_PWR_Init(){
    __GPIOB_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = 0x0;
    //初始化Si24R1的VCC
    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
}

void sFUNC_Set2D4GHZ_PWR(uint8_t is_en){
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,!!is_en);
}

void sFUNC_InitBMP280(){
    sDRV_BMP280_Conf_t bmp280 = {0};
    bmp280.mode = BMP280_MODE_FORCED;
    bmp280.filter = BMP280_FILTER_OFF;
    bmp280.osrs_p = BMP280_OSRS_X1;
    bmp280.osrs_t = BMP280_OSRS_X1;
    bmp280.tsb = BMP280_TSB_62D5MS;
    sDRV_BMP280_Init(&bmp280);
}

 //Si24R1接收到数据中断
 void recied_data(Si24R1_Data_t* data){
     //sDRV_UART_Printf("recied data:\"%s\",len:%d,ppp:%d,rssi:%d\n",data->msg,data->len,data->ppp,data->rssi);
 }

//初始化2.4GHz
void sFUNC_Init2d4GHz(){
    Si24R1_Conf_t si24r1 = {0};
    //地址宽度
    si24r1.addr_width = ADDR_WIDTH_5;
    //接收地址P0
    si24r1.addr.rx_addr_p0 = 0x123456789a;
    //接收地址P1
    si24r1.addr.rx_addr_p1 = 0x1111111111;
    //P2
    si24r1.addr.rx_addr_p2 = 0x22;
    //P3
    si24r1.addr.rx_addr_p3 = 0x33;
    //P4
    si24r1.addr.rx_addr_p4 = 0x44;
    //P5
    si24r1.addr.rx_addr_p5 = 0x55;
    //发送地址
    si24r1.addr.tx_addr = 0x123456789a;
    //自动确认使能
    si24r1.aa_en = EN_AA_ENAA_P0 | EN_AA_ENAA_P1 | EN_AA_ENAA_P2 | EN_AA_ENAA_P3 | EN_AA_ENAA_P4 | EN_AA_ENAA_P5;
    //ACK模式
    si24r1.ack_mode = ACK_MODE_DIS;
    //自动重发次数
    si24r1.arc_times = 10;
    //自动重发延迟
    si24r1.ard_us = 1000;
    //CRC使能
    si24r1.crc_en = CRC_DIS;
    //CRC长度
    si24r1.crc_len = CRC_LEN_2;
    //动态负载长度
    si24r1.dynpd = DYNPD_DPL_P0 | DYNPD_DPL_P1 | DYNPD_DPL_P2 | DYNPD_DPL_P3 | DYNPD_DPL_P4 | DYNPD_DPL_P5;
    //ACK负载
    si24r1.en_ack_payd = EN_ACK_PAYD_DIS;
    //使能动态负载
    si24r1.en_dpl = EN_DPL_DIS;
    //发射还是接收模式
    si24r1.mode = MODE_TX;
    //射频信道
    si24r1.rf_ch = 100;
    //发射功率
    si24r1.rf_pwr = RF_PWR_7DBM;
    //射频速率
    si24r1.rf_spd = RF_SPD_250K;
    //接收管道长度
    si24r1.rx_pw.rx_pw_p0 = 32;
    si24r1.rx_pw.rx_pw_p1 = 32;
    si24r1.rx_pw.rx_pw_p2 = 32;
    si24r1.rx_pw.rx_pw_p3 = 32;
    si24r1.rx_pw.rx_pw_p4 = 32;
    si24r1.rx_pw.rx_pw_p5 = 32;
    //接收管道使能
    si24r1.rxaddr_en = EN_RXADDR_MSK_ERX_P5 | EN_RXADDR_MSK_ERX_P4 | EN_RXADDR_MSK_ERX_P3 | \
                       EN_RXADDR_MSK_ERX_P2 | EN_RXADDR_MSK_ERX_P0 | EN_RXADDR_MSK_ERX_P0;
    si24r1.irq_msk = 0;

    //sDRV_UART_Printf("Si24R1 init begin!\n");
    
    //
    
    sDRV_Si24R1_Init(&si24r1,recied_data);
    
    HAL_Delay(1);
    
    sDRV_Si24R1_SetMode(MODE_TX);
    sDRV_Si24R1_SetRFSPD(RF_SPD_250K);
    
    
    sDRV_Si24R1_SetStandby();
    //sDRV_UART_Printf("Si24R1 into standby mode!\n");

}

#define VBAT_CH     ADC_CHANNEL_0

#define AVERAGE_NUM     (8)



float sAPP_Func_GetVbat(){
    uint16_t raw = 0;
    for(uint8_t i = 0;i < AVERAGE_NUM;i++){
        raw += sBSP_ADC_GetChannel(VBAT_CH);
    }


    float raw_val = ((float)raw / AVERAGE_NUM) / 4095.0f * 3300.0f;


    return raw_val;
}

