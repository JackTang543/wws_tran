#include "sAPP_Func.h"

#include "sBSP_ADC.h"





void sFUNC_InitLED(){
    __GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef gpio = {0};
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Pin = GPIO_PIN_0;
    gpio.Alternate = 0x0;
    
    HAL_GPIO_Init(GPIOB,&gpio);
}


//��ʼ��2.4GHz
void sFUNC_Init2d4GHz(){
    Si24R1_Conf_t si24r1 = {0};
    //��ַ���
    si24r1.addr_width = ADDR_WIDTH_5;
    //���յ�ַP0
    si24r1.addr.rx_addr_p0 = 0x123456789a;
    //���յ�ַP1
    si24r1.addr.rx_addr_p1 = 0x1111111111;
    //P2
    si24r1.addr.rx_addr_p2 = 0x22;
    //P3
    si24r1.addr.rx_addr_p3 = 0x33;
    //P4
    si24r1.addr.rx_addr_p4 = 0x44;
    //P5
    si24r1.addr.rx_addr_p5 = 0x55;
    //���͵�ַ
    si24r1.addr.tx_addr = 0x123456789a;
    //�Զ�ȷ��ʹ��
    si24r1.aa_en = EN_AA_ENAA_P0 | EN_AA_ENAA_P1 | EN_AA_ENAA_P2 | EN_AA_ENAA_P3 | EN_AA_ENAA_P4 | EN_AA_ENAA_P5;
    //ACKģʽ
    si24r1.ack_mode = ACK_MODE_DIS;
    //�Զ��ط�����
    si24r1.arc_times = 10;
    //�Զ��ط��ӳ�
    si24r1.ard_us = 1000;
    //CRCʹ��
    si24r1.crc_en = CRC_DIS;
    //CRC����
    si24r1.crc_len = CRC_LEN_2;
    //��̬���س���
    si24r1.dynpd = DYNPD_DPL_P0 | DYNPD_DPL_P1 | DYNPD_DPL_P2 | DYNPD_DPL_P3 | DYNPD_DPL_P4 | DYNPD_DPL_P5;
    //ACK����
    si24r1.en_ack_payd = EN_ACK_PAYD_DIS;
    //ʹ�ܶ�̬����
    si24r1.en_dpl = EN_DPL_DIS;
    //���仹�ǽ���ģʽ
    si24r1.mode = MODE_TX;
    //��Ƶ�ŵ�
    si24r1.rf_ch = 100;
    //���书��
    si24r1.rf_pwr = RF_PWR_4DBM;
    //��Ƶ����
    si24r1.rf_spd = RF_SPD_250K;
    //���չܵ�����
    si24r1.rx_pw.rx_pw_p0 = 32;
    si24r1.rx_pw.rx_pw_p1 = 32;
    si24r1.rx_pw.rx_pw_p2 = 32;
    si24r1.rx_pw.rx_pw_p3 = 32;
    si24r1.rx_pw.rx_pw_p4 = 32;
    si24r1.rx_pw.rx_pw_p5 = 32;
    //���չܵ�ʹ��
    si24r1.rxaddr_en = EN_RXADDR_MSK_ERX_P5 | EN_RXADDR_MSK_ERX_P4 | EN_RXADDR_MSK_ERX_P3 | \
                       EN_RXADDR_MSK_ERX_P2 | EN_RXADDR_MSK_ERX_P0 | EN_RXADDR_MSK_ERX_P0;
    si24r1.irq_msk = 0;

    sDRV_UART_Printf("Si24R1 init begin!\n");
    
    //
    
    sDRV_Si24R1_Init(&si24r1,recied_data);
    
    HAL_Delay(1);
    
    sDRV_Si24R1_SetMode(MODE_TX);
    sDRV_Si24R1_SetRFSPD(RF_SPD_250K);
    
    
    sDRV_Si24R1_SetStandby();
    sDRV_UART_Printf("Si24R1 into standby mode!\n");

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

