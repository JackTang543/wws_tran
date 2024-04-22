#include "sBSP_RTC.h"

#ifdef __SBSP_RTC_EN__


RTC_HandleTypeDef hrtc;

RTC_TimeTypeDef time = {0};
RTC_DateTypeDef date = {0};

#define RTC_ASYNCH_PREDIV    127
#define RTC_SYNCH_PREDIV     288

void sBSP_RTC_Init(){
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    hrtc.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if(HAL_RTC_Init(&hrtc) != HAL_OK)
    {
    }
}

void sBSP_RTC_SetTime(uint8_t hour,uint8_t min,uint8_t sec){
    time.Hours = hour;
    time.Minutes = min;
    time.Seconds = sec;
    HAL_RTC_SetTime(&hrtc,&time,RTC_FORMAT_BIN);
}

//year(eg:2024),RTC_Month_Date_Definitions,day,RTC_WeekDay_Definitions
void sBSP_RTC_SetDate(uint16_t year,uint8_t month,uint8_t day,uint8_t weekday){
    date.Year = year - 2000;
    date.Month = month;
    date.Date = day;
    date.WeekDay = weekday;
    HAL_RTC_SetDate(&hrtc,&date,RTC_FORMAT_BIN);
}

void sBSP_RTC_GetTime(RTC_TimeTypeDef* param_time){
    HAL_RTC_GetTime(&hrtc,&time,RTC_FORMAT_BIN);
    param_time->Hours = time.Hours;
    param_time->Minutes = time.Minutes;
    param_time->Seconds = time.Seconds;
}

void sBSP_RTC_GetDate(RTC_DateTypeDef* param_date){
    HAL_RTC_GetDate(&hrtc,&date,RTC_FORMAT_BIN);
    param_date->Year = date.Year;
    param_date->Month = date.Month;
    param_date->Date = date.Date;
    param_date->WeekDay = date.WeekDay;
}


void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle){
    if(rtcHandle->Instance==RTC)
    {
          RCC_OscInitTypeDef RCC_OscInitStruct;
          RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
          
          /*##-1- Configure the RTC clock source ######################################*/
          /* -a- Enable LSI Oscillator */
          RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI;
          RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
          RCC_OscInitStruct.LSIState = RCC_LSI_ON;
          if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
          {
              while(1);
          }
          
          /* -b- Select LSI as RTC clock source */
          PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
          PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
          if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
          { 
              while(1);
          }
          
          /*##-2- Enable the RTC peripheral Clock ####################################*/
          /* Enable RTC Clock */
          __HAL_RCC_RTC_ENABLE();
          
          /*##-3- Configure the NVIC for RTC Alarm ###################################*/
          HAL_NVIC_SetPriority(RTC_IRQn, 0x0, 0);
          HAL_NVIC_EnableIRQ(RTC_IRQn);

    }
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc){
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
}



#endif
