#include "sBSP_ADC.h"

ADC_HandleTypeDef hadc;
ADC_ChannelConfTypeDef sConfig;

void sBSP_ADC_Init(){
    //ADC1
    hadc.Instance = ADC1;
    //过采样
    hadc.Init.OversamplingMode = DISABLE;
    //! 过采样能把12位的ADC变成16位的,代价是损失速度,转换时要注意位数变化了
    //hadc.Init.Oversample.Ratio = ADC_OVERSAMPLING_RATIO_256;
    //hadc.Init.Oversample.RightBitShift = ADC_RIGHTBITSHIFT_NONE;
    //hadc.Init.Oversample.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
    //选择ADC的时钟分频
    hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    //ADC解析度
    hadc.Init.Resolution = ADC_RESOLUTION_12B;
    //采样时间
    hadc.Init.SamplingTime = ADC_SAMPLETIME_160CYCLES_5;
    //扫描转换的方向
    hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
    //数据对齐
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //连续转换模式
    hadc.Init.ContinuousConvMode = DISABLE;
    //非连续转换模式
    hadc.Init.DiscontinuousConvMode = DISABLE;
    //外部触发边缘
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //是否外部触发转换
    hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    //DMA连续请求
    hadc.Init.DMAContinuousRequests = DISABLE;
    //转换完成选择
    hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    //设置溢出会怎么样
    hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc.Init.LowPowerAutoWait = DISABLE;
    hadc.Init.LowPowerFrequencyMode = DISABLE;
    hadc.Init.LowPowerAutoPowerOff = DISABLE;

    HAL_ADC_Init(&hadc);

    //ADC自校准,这个操作要在初始化ADC之后且还没有启动ADC转换时进行,否则硬错误伺候
    HAL_ADCEx_Calibration_Start(&hadc,ADC_SINGLE_ENDED);

    sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_TEMPSENSOR | ADC_CHANNEL_VREFINT;
    //清空rank
    sConfig.Rank = ADC_RANK_NONE;
    HAL_ADC_ConfigChannel(&hadc, &sConfig);
}

//uint32_t channel : ADC_CHANNEL_0
uint16_t sBSP_ADC_GetChannel(uint32_t channel){
    //设置通道
    sConfig.Channel = channel;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    HAL_ADC_ConfigChannel(&hadc, &sConfig);

    //启动转换,等待结果
    HAL_ADC_Start(&hadc);
    HAL_ADC_PollForConversion(&hadc, 100);

    uint16_t val = 0;
    val = HAL_ADC_GetValue(&hadc);

    //重置通道
    sConfig.Rank = ADC_RANK_NONE;
    HAL_ADC_ConfigChannel(&hadc, &sConfig);

    return val;
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc){
    if(hadc->Instance == ADC1){
        __GPIOA_CLK_ENABLE();
        GPIO_InitTypeDef gpio = {0};
        //注意这里模式为ANALOG才能模拟输入,INPUT输入的是经过施密特触发器的信号(为了低功耗)
        gpio.Mode = GPIO_MODE_ANALOG;
        gpio.Pull = GPIO_NOPULL;
        //PA0 <- TEMT6000
        gpio.Pin = GPIO_PIN_0;
        //这里的复用在ADC这里不需要
        gpio.Alternate = 0x0;

        HAL_GPIO_Init(GPIOA,&gpio);
        //PA0 <- Vbat
        gpio.Pin = GPIO_PIN_1;
        HAL_GPIO_Init(GPIOA,&gpio);
        
        __ADC1_CLK_ENABLE();
    }
    
}



