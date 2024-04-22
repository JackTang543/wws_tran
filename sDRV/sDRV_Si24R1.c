#include "sDRV_Si24R1.h"
#include "sBSP_softSPI.h"


reci_data_cb_t reci_data_cb;
Si24R1_Data_t data;
uint8_t tran_cplt_flag;


//******************************************接口BEGIN****************************************
//SPI发送一个字节
static inline void sendByte(uint8_t byte){
    sBSP_softSPI_SendByte(byte);
    //sBSP_SPI1_SendByte(byte);
}
//SPI接收一个字节
static uint8_t reciByte(){
    uint8_t temp = sBSP_softSPI_ReciByte();  
    //uint8_t temp = sBSP_SPI1_ReciByte();
    return temp;
}
//SPI发送多个字节
static inline void sendBytes(uint8_t *pData,uint16_t length){
    sBSP_softSPI_SendBytes(pData,length);
    //sBSP_SPI1_SendBytes(pData,length);
}
//SPI接收多个字节
static inline void reciBytes(uint8_t *pData,uint16_t length){
    sBSP_softSPI_ReciBytes(pData,length);
    //sBSP_SPI1_ReciBytes(pData,length);
}
//设置CE电平
static inline void setCE(uint8_t lv){
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,lv);
}
//设置CS电平
static inline void setCS(uint8_t cs){
    sBSP_softSPI_SetCS(cs);
    //sBSP_SPI1_SetCS(cs);
}
//******************************************接口END****************************************

//初始化GPIO
static void init_gpio(){
    //PB0 -> CE   输出,模块控制
    //PB7 -> IRQ  
    //PA4 -> CSN  输出,片选
    //PA5 -> SCK  输出,SPI
    //PA6 -> MISO 
    //PA7 -> MOSI 
    __GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef gpio_init;
    gpio_init.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pin   = GPIO_PIN_6;
    gpio_init.Pull  = GPIO_PULLDOWN;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    //配置PB0->CE,下拉,高电平有效
    HAL_GPIO_Init(GPIOB,&gpio_init);

    __GPIOB_CLK_ENABLE();
    gpio_init.Mode  = GPIO_MODE_IT_FALLING;
    gpio_init.Pin   = GPIO_PIN_0;
    gpio_init.Pull  = GPIO_PULLUP;
    //PB0<-IRQ,上拉,低电平有效
    HAL_GPIO_Init(GPIOB,&gpio_init);

    //
    HAL_NVIC_SetPriority(EXTI4_15_IRQn,0,0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}
/*@brief  对寄存器进行操作
*
* @param  uint8_t reg_addr   : 要操作的寄存器地址
* @param  uint8_t reg_bit    : 要操作的位掩码
* @param  Si24R1_OPER_t oper : 要执行的操作
* 
* @return uint8_t            : 如果进行读操作,返回读的值,否则返回值无效
*/
static uint8_t oper_reg(uint8_t reg_addr,uint8_t reg_bit,Si24R1_OPER_t oper){
    uint8_t reg = 0;
    //首先把要操作的寄存器读过来
    setCS(0);
    sendByte(__COMM_R_REG(reg_addr));
    reg = reciByte();
    setCS(1);

    //正式操作
    setCS(0);
    //操作寄存器置位
    if(oper == OPER_SET_REG){
        //置位
        __SET_REG_BIT(reg,reg_bit);
        //发送指令写寄存器地址
        sendByte(__COMM_W_REG(reg_addr));
        //写回
        sendByte(reg);
    }
    //操作寄存器清位
    else if(oper == OPER_CLR_REG){
        //清空
        __CLR_REG_BIT(reg,reg_bit);
        //发送指令写寄存器地址
        sendByte(__COMM_W_REG(reg_addr));
        //写回
        sendByte(reg);
    }
    //获取寄存器位
    else if(oper == OPER_GET_REG){
        //清空读位的其他位
        __CLR_REG_BIT(reg,~reg_bit);
    }
    setCS(1);
    return reg;
}
/*@brief  把传入的结构体送入模块进行初始化
*
* @param  Si24R1_Conf_t* conf : 要初始化的结构体指针
* 
* @return 无
*/
static void init_conf(Si24R1_Conf_t* conf){
    //配置地址宽度
    //清空地址寄存器
    oper_reg(ADDR_SETUP_AW,SETUP_AW_MSK_AW_0 | SETUP_AW_MSK_AW_1,OPER_CLR_REG);
    //设置地址寄存器
    oper_reg(ADDR_SETUP_AW,conf->addr_width,OPER_SET_REG);

    //配置发射地址
    uint8_t addrTXseq[] = {__COMM_W_REG(ADDR_TX_ADDR),
                           (uint8_t)((conf->addr.tx_addr    >> 0 ) & 0xFF),
                           (uint8_t)((conf->addr.tx_addr    >> 8 ) & 0xFF),
                           (uint8_t)((conf->addr.tx_addr    >> 16) & 0xFF),
                           (uint8_t)((conf->addr.tx_addr    >> 24) & 0xFF),
                           (uint8_t)((conf->addr.tx_addr    >> 32) & 0xFF),
                          };
    setCS(0);
    sendBytes(addrTXseq,sizeof(addrTXseq) / sizeof(addrTXseq[0]));
    setCS(1);

    //配置P0接收地址
    uint8_t addrP0seq[] = {__COMM_W_REG(ADDR_RX_ADDR_P0),
                           (uint8_t)((conf->addr.rx_addr_p0 >> 0 ) & 0xFF),
                           (uint8_t)((conf->addr.rx_addr_p0 >> 8 ) & 0xFF),
                           (uint8_t)((conf->addr.rx_addr_p0 >> 16) & 0xFF),
                           (uint8_t)((conf->addr.rx_addr_p0 >> 24) & 0xFF),
                           (uint8_t)((conf->addr.rx_addr_p0 >> 32) & 0xFF),
                          };
    setCS(0);
    sendBytes(addrP0seq,sizeof(addrP0seq) / sizeof(addrP0seq[0]));
    setCS(1);

    //配置P1接收地址
    uint8_t addrP1seq[] = {__COMM_W_REG(ADDR_RX_ADDR_P1),
                           (uint8_t)((conf->addr.rx_addr_p1 >> 0 ) & 0xFF),
                           (uint8_t)((conf->addr.rx_addr_p1 >> 8 ) & 0xFF),
                           (uint8_t)((conf->addr.rx_addr_p1 >> 16) & 0xFF),
                           (uint8_t)((conf->addr.rx_addr_p1 >> 24) & 0xFF),
                           (uint8_t)((conf->addr.rx_addr_p1 >> 32) & 0xFF),
                          };
    setCS(0);
    sendBytes(addrP1seq,sizeof(addrP1seq) / sizeof(addrP1seq[0]));
    setCS(1);

    //配置P2接收地址
    uint8_t addrP2seq[] = {__COMM_W_REG(ADDR_RX_ADDR_P2),conf->addr.rx_addr_p2};
    setCS(0);
    sendBytes(addrP2seq,sizeof(addrP2seq) / sizeof(addrP2seq[0]));
    setCS(1);

    //配置P3接收地址
    uint8_t addrP3seq[] = {__COMM_W_REG(ADDR_RX_ADDR_P3),conf->addr.rx_addr_p3};
    setCS(0);
    sendBytes(addrP3seq,sizeof(addrP3seq) / sizeof(addrP3seq[0]));
    setCS(1);

    //配置P4接收地址
    uint8_t addrP4seq[] = {__COMM_W_REG(ADDR_RX_ADDR_P4),conf->addr.rx_addr_p4};
    setCS(0);
    sendBytes(addrP4seq,sizeof(addrP4seq) / sizeof(addrP4seq[0]));
    setCS(1);

    //配置P5接收地址
    uint8_t addrP5seq[] = {__COMM_W_REG(ADDR_RX_ADDR_P5),conf->addr.rx_addr_p5};
    setCS(0);
    sendBytes(addrP5seq,sizeof(addrP5seq) / sizeof(addrP5seq[0]));
    setCS(1);

    //配置使能数据管道地址寄存器
    //先清空
    oper_reg(ADDR_EN_RXADDR,EN_RXADDR_MSK_ERX_P0 | EN_RXADDR_MSK_ERX_P1 | \
                            EN_RXADDR_MSK_ERX_P2 | EN_RXADDR_MSK_ERX_P3 | \
                            EN_RXADDR_MSK_ERX_P4 | EN_RXADDR_MSK_ERX_P5,  \
                            OPER_CLR_REG);
    //再写
    oper_reg(ADDR_EN_RXADDR,conf->rxaddr_en,OPER_SET_REG);

    //配置射频信道
    oper_reg(ADDR_RF_CH,RF_CH_MSK(0b1111111),OPER_CLR_REG);
    oper_reg(ADDR_RF_CH,RF_CH_MSK(conf->rf_ch),OPER_SET_REG);

    //配置接收通道
    oper_reg(ADDR_RX_PW_P0,RX_PW_P0_MSK,OPER_CLR_REG);
    oper_reg(ADDR_RX_PW_P1,RX_PW_P1_MSK,OPER_CLR_REG);
    oper_reg(ADDR_RX_PW_P2,RX_PW_P2_MSK,OPER_CLR_REG);
    oper_reg(ADDR_RX_PW_P3,RX_PW_P3_MSK,OPER_CLR_REG);
    oper_reg(ADDR_RX_PW_P4,RX_PW_P4_MSK,OPER_CLR_REG);
    oper_reg(ADDR_RX_PW_P5,RX_PW_P5_MSK,OPER_CLR_REG);
    oper_reg(ADDR_RX_PW_P0,conf->rx_pw.rx_pw_p0,OPER_SET_REG);
    oper_reg(ADDR_RX_PW_P1,conf->rx_pw.rx_pw_p1,OPER_SET_REG);
    oper_reg(ADDR_RX_PW_P2,conf->rx_pw.rx_pw_p2,OPER_SET_REG);
    oper_reg(ADDR_RX_PW_P3,conf->rx_pw.rx_pw_p3,OPER_SET_REG);
    oper_reg(ADDR_RX_PW_P4,conf->rx_pw.rx_pw_p4,OPER_SET_REG);
    oper_reg(ADDR_RX_PW_P5,conf->rx_pw.rx_pw_p5,OPER_SET_REG);

    //配置使能自动确认
    oper_reg(ADDR_EN_AA,EN_AA_MSK_ENAA_P0 |
                        EN_AA_MSK_ENAA_P1 |
                        EN_AA_MSK_ENAA_P2 |
                        EN_AA_MSK_ENAA_P3 |
                        EN_AA_MSK_ENAA_P4 |
                        EN_AA_MSK_ENAA_P5,
                        OPER_CLR_REG);
    oper_reg(ADDR_EN_AA,conf->aa_en,OPER_SET_REG);

    //设置数据传输速度
    oper_reg(ADDR_RF_SETUP,RF_SETUP_MSK_RF_DR_0 | RF_SETUP_MSK_RF_DR_1,OPER_CLR_REG);
    if(conf->rf_spd == RF_SPD_250K){
        oper_reg(ADDR_RF_SETUP,RF_SETUP_MSK_RF_DR_0,OPER_SET_REG);
    }
    else if(conf->rf_spd == RF_SPD_1M){
        //00
    }
    else if(conf->rf_spd == RF_SPD_2M){
        oper_reg(ADDR_RF_SETUP,RF_SETUP_MSK_RF_DR_1,OPER_SET_REG);
    }

    //设置发射功率
    oper_reg(ADDR_RF_SETUP,RF_SETUP_MSK_RF_PWR_0 | RF_SETUP_MSK_RF_PWR_1 | RF_SETUP_MSK_RF_PWR_2,OPER_CLR_REG);
    oper_reg(ADDR_RF_SETUP,conf->rf_pwr,OPER_SET_REG);

    //配置CRC
    if(conf->crc_en == CRC_EN){
        oper_reg(ADDR_CONFIG,CONFIG_MSK_EN_CRC,OPER_SET_REG);
    }else{
        oper_reg(ADDR_CONFIG,CONFIG_MSK_EN_CRC,OPER_CLR_REG);
    }
    if(conf->crc_len == CRC_LEN_2){
        oper_reg(ADDR_CONFIG,CONFIG_MSK_CRCO,OPER_SET_REG);
    }else{
        oper_reg(ADDR_CONFIG,CONFIG_MSK_CRCO,OPER_CLR_REG);
    }

    //配置发射方/接收方
    if(conf->mode == MODE_RX){
        oper_reg(ADDR_CONFIG,CONFIG_MSK_PRIM_RX,OPER_SET_REG);
    }else{
        oper_reg(ADDR_CONFIG,CONFIG_MSK_PRIM_RX,OPER_CLR_REG);
    }
    
    //配置是否需要ACK
    if(conf->ack_mode == ACK_MODE_EN){
        oper_reg(ADDR_FEATURE,FEATURE_MSK_EN_DYN_ACK,OPER_CLR_REG);
    }else{
        oper_reg(ADDR_FEATURE,FEATURE_MSK_EN_DYN_ACK,OPER_SET_REG);
    }
    
    //配置自动重发延时,自动重发次数
    oper_reg(ADDR_SETUP_RETR,SETUP_RETR_MSK_ARD(0b1111),OPER_CLR_REG);
    oper_reg(ADDR_SETUP_RETR,SETUP_RETR_MSK_ARD(conf->ard_us / 250),OPER_SET_REG);

    oper_reg(ADDR_SETUP_RETR,SETUP_RETR_MSK_ARC(0b1111),OPER_CLR_REG);
    oper_reg(ADDR_SETUP_RETR,SETUP_RETR_MSK_ARC(conf->arc_times),OPER_SET_REG);

    //配置动态负载长度
    oper_reg(ADDR_DYNPD,DYNPD_DPL_P0 | DYNPD_DPL_P1 | \
                        DYNPD_DPL_P2 | DYNPD_DPL_P3 | \
                        DYNPD_DPL_P4 | DYNPD_DPL_P4,  \
                        OPER_CLR_REG);
    oper_reg(ADDR_DYNPD,conf->dynpd,OPER_SET_REG);

    //配置是否使能动态负载长度
    if(conf->en_dpl == EN_DPL_EN){
        oper_reg(ADDR_FEATURE,FEATURE_MSK_EN_DPL,OPER_SET_REG);
    }else{
        oper_reg(ADDR_FEATURE,FEATURE_MSK_EN_DPL,OPER_CLR_REG);
    }

    //配置是否使能ACK负载
    if(conf->en_ack_payd == EN_ACK_PAYD_EN){
        oper_reg(ADDR_FEATURE,FEATURE_MSK_EN_ACK_PAYd,OPER_SET_REG);
    }else{
        oper_reg(ADDR_FEATURE,FEATURE_MSK_EN_ACK_PAYd,OPER_CLR_REG);
    }

    //配置接收中断屏蔽
    if(__GET_REG_BIT(conf->irq_msk,IRQ_MSK_RX_DR)){
        //写1关闭中断
        oper_reg(ADDR_CONFIG,CONFIG_MSK_RX_DR,OPER_SET_REG);
    }else{
        oper_reg(ADDR_CONFIG,CONFIG_MSK_RX_DR,OPER_CLR_REG);
    }

    //配置发射中断屏蔽
    if(__GET_REG_BIT(conf->irq_msk,IRQ_MSK_TX_DS)){
        oper_reg(ADDR_CONFIG,CONFIG_MSK_TX_DS,OPER_SET_REG);
    }else{
        oper_reg(ADDR_CONFIG,CONFIG_MSK_TX_DS,OPER_CLR_REG);
    }

    //配置最大重发计数中断屏蔽
    if(__GET_REG_BIT(conf->irq_msk,IRQ_MSK_MAX_RT)){
        oper_reg(ADDR_CONFIG,CONFIG_MSK_MAX_RT,OPER_SET_REG);
    }else{
        oper_reg(ADDR_CONFIG,CONFIG_MSK_MAX_RT,OPER_CLR_REG);
    }

}

//中断处理函数
void sDRV_Si24R1_IrqHandler(){
    if(oper_reg(__COMM_R_REG(ADDR_STATUS),STATUS_MSK_TX_DS,OPER_GET_REG)){
        //检测到发射完成中断,清除标志位
        oper_reg(__COMM_W_REG(ADDR_STATUS),STATUS_MSK_TX_DS,OPER_SET_REG);
        tran_cplt_flag = 1;
        //sHMI_Debug_Printf("Tran cplt!\n");
    }
    else if(oper_reg(__COMM_R_REG(ADDR_STATUS),STATUS_MSK_RX_DR,OPER_GET_REG)){
        //检测到接收完成中断
        oper_reg(__COMM_W_REG(ADDR_STATUS),STATUS_MSK_RX_DR,OPER_SET_REG);
        //sHMI_Debug_Printf("Reci cplt!\n");
        //读取数据长度
        setCS(0);
        sendByte(COMM_R_RX_PL_WID);
        data.len = reciByte();
        setCS(1);
        //读取当前数据的接收管道
        setCS(0);
        sendByte(__COMM_R_REG(ADDR_STATUS));
        data.ppp = reciByte();
        data.ppp = __GET_REG_BIT(data.ppp,STATUS_MSK_RX_P_NO_3 | STATUS_MSK_RX_P_NO_2 | STATUS_MSK_RX_P_NO_1);
        setCS(1);
        //读取当前的RSSI
        setCS(0);
        sendByte(__COMM_R_REG(ADDR_RSSI));
        data.rssi = reciByte();
        data.rssi = __GET_REG_BIT(data.rssi,RSSI_MSK_RSSI);
        setCS(1);
        //把接收完成的数据保存下来
        setCS(0);
        sendByte(COMM_RX_PAYLOAD);
        reciBytes(data.msg,data.len);
        setCS(1);

        //调用用户函数
        reci_data_cb(&data);
    }
    else if(oper_reg(__COMM_R_REG(ADDR_STATUS),STATUS_MSK_MAX_RT,OPER_GET_REG)){
        //检测到最大重发次数中断
        oper_reg(__COMM_W_REG(ADDR_STATUS),STATUS_MSK_MAX_RT,OPER_SET_REG);
        //sHMI_Debug_Printf("Max re-tran!\n");
    }
}

/*@brief  进入关断模式,此时最省电
*
* @param  无
*
* @return 无
*/
void sDRV_Si24R1_SetShutdown(){
    //进入条件:PWR_UP=0
    oper_reg(ADDR_CONFIG,CONFIG_MSK_PWR_UP,OPER_CLR_REG);
    setCE(0);
}

/*@brief  进入待机模式,此时进入工作模式最快
*         从关断模式进入后要等待2mS
*
* @param  无
*
* @return 无
*/
void sDRV_Si24R1_SetStandby(){
    //进入条件:PWR_UP=1
    oper_reg(ADDR_CONFIG,CONFIG_MSK_PWR_UP,OPER_SET_REG);
    setCE(0);
    //等待2mS
}

/*@brief  设置为发射空闲模式
*
* @param  无
*
* @return 无
*/
void sDRV_Si24R1_SetIdleTX(){
    //进入条件:PRIM_RX=0,CE=1
    oper_reg(ADDR_CONFIG,CONFIG_MSK_PRIM_RX,OPER_CLR_REG);
    //设置CE=1
    setCE(1);
}

/*@brief  进入接收模式,如果接收到数据会调用用户函数
*         进入后需要等待最少130uS
*
* @param  无
*
* @return 无
*/
void sDRV_Si24R1_StartRX(){
    //进入条件:PRIM_RX=1,CE=1
    oper_reg(ADDR_CONFIG,CONFIG_MSK_PRIM_RX,OPER_SET_REG);
    //设置CE=1
    setCE(1);
    //等待130uS
}

/*@brief  发送数据,不需要ACK
*
* @param  uint8_t* data    : 数据指针
* @param  uint8_t length   : 数据长度
*
* @return int8_t           : OK返回0, Error返回-1
*/
int8_t sDRV_Si24R1_TranDataNoAck(uint8_t* data,uint8_t length){
    tran_cplt_flag = 0;
    setCS(0);
    sendByte(COMM_W_TX_PAYLOAD_NO_ACK);
    sendBytes(data,length);
    setCS(1);
    return 0;
}

/*@brief  发送数据,需要ACK
*
* @param  uint8_t* data    : 数据指针
* @param  uint8_t length   : 数据长度
*
* @return int8_t           : OK返回0, Error返回-1
*/
int8_t sDRV_Si24R1_TranDataAck(uint8_t* data,uint8_t length){
    tran_cplt_flag = 0;
    setCS(0);
    sendByte(COMM_W_TX_PAYLOAD);
    sendBytes(data,length);
    setCS(1);
    return 0;
}

/*@brief  设置接收/发送模式
*
* @param  Si24R1_MODE_t mode : 模式
*
* @return 无
*/
void sDRV_Si24R1_SetMode(Si24R1_MODE_t mode){
    if(mode == MODE_RX){
        oper_reg(ADDR_CONFIG,CONFIG_MSK_PRIM_RX,OPER_SET_REG);
    }else{
        oper_reg(ADDR_CONFIG,CONFIG_MSK_PRIM_RX,OPER_CLR_REG);
    }
}

/*@brief  设置射频功率
*
* @param  Si24R1_RF_PWR rf_pwr : 射频功率
*
* @return 无
*/
void sDRV_Si24R1_SetRFPWR(Si24R1_RF_PWR rf_pwr){
    oper_reg(ADDR_RF_SETUP,RF_SETUP_MSK_RF_PWR_0 | RF_SETUP_MSK_RF_PWR_1 | RF_SETUP_MSK_RF_PWR_2,OPER_CLR_REG);
    oper_reg(ADDR_RF_SETUP,rf_pwr,OPER_SET_REG);
}

/*@brief  设置射频发射速度
*
* @param  Si24R1_RF_PWR rf_pwr : 射频速率
*
* @return 无
*/
void sDRV_Si24R1_SetRFSPD(Si24R1_RF_SPD rf_spd){
    oper_reg(ADDR_RF_SETUP,RF_SETUP_MSK_RF_DR_0 | RF_SETUP_MSK_RF_DR_1,OPER_CLR_REG);
    if(rf_spd == RF_SPD_250K){
        oper_reg(ADDR_RF_SETUP,RF_SETUP_MSK_RF_DR_0,OPER_SET_REG);
    }
    else if(rf_spd == RF_SPD_1M){
        //00
    }
    else if(rf_spd == RF_SPD_2M){
        oper_reg(ADDR_RF_SETUP,RF_SETUP_MSK_RF_DR_1,OPER_SET_REG);
    }
}

/*@brief  设置射频信道
*
* @param  uint8_t rf_ch : 射频信道,从2400MHz开始到2525MHz
*
* @return 无
*/
void sDRV_Si24R1_SetRFCH(uint8_t rf_ch){
    oper_reg(ADDR_RF_CH,RF_CH_MSK(0b1111111),OPER_CLR_REG);
    oper_reg(ADDR_RF_CH,RF_CH_MSK(rf_ch),OPER_SET_REG);
}

/*@brief  检查发送是否完成
*
* @param  无
*
* @return uint8_t : 发送完成:1,还未完成:0
*/
uint8_t sDRV_Si24R1_IsTranCplt(){
    return tran_cplt_flag;
}


void sDRV_Si24R1_Init(Si24R1_Conf_t* init_conf_struct,reci_data_cb_t reci_cb){
    //初始化GPIO
    init_gpio();
    //初始化SPI
    sBSP_softSPI_Init();
    //sBSP_SPI1_Init(SPI_BAUDRATEPRESCALER_256);
    
    if(reci_cb != NULL){
        reci_data_cb = reci_cb;
    }


    init_conf(init_conf_struct);

    sDRV_Si24R1_IrqHandler();
}





