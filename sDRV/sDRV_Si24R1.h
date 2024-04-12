#ifndef __SDRV_SI24R1_H__
#define __SDRV_SI24R1_H__


#include "stm32l0xx_hal.h"

#ifdef __cplusplus
extern "C"{
#endif


//************************命令字表******************************************************************
//读寄存器:__REG__为寄存器地址
#define __COMM_R_REG(__REG__)         (__REG__)            
//写寄存器
#define __COMM_W_REG(__REG__)         (0x20 | (__REG__))   
//从FIFO中读收到的数据,后可读1~32字节,读出即删除,适用接收模式
#define COMM_RX_PAYLOAD               (0x61)           
//写FIFO数据,1~32字节,适用发射模式    
#define COMM_W_TX_PAYLOAD             (0xA0)
//清空TX FIFO,适用发射模式
#define COMM_FLUSH_TX                 (0xE1)
//清空RX FIFO,适用接收模式,不能在回ACK完成之前清,不然通信失败
#define COMM_FLUSH_RX                 (0xE2)
//适用发送方
#define COMM_REUSE_TX_PL              (0xE3)
//读取收到的数据字节数
#define COMM_R_RX_PL_WID              (0x60)
//适用接收方,通过PIPE PPP将数据通过ACK发出去,最多允许三帧数据
#define __COMM_W_ACK_PAYLOAD(__PPP__) (0xA8 | (__PPP__))
//适用发射模式,使用这个命令同时需要将AUTOACK位置1
#define COMM_W_TX_PAYLOAD_NO_ACK      (0xB0)
//无操作,可用于返回STATUS值
#define COMM_NOP                      (0xFF)


//************************寄存器地址和寄存器位掩码表***********************************************

//配置寄存器地址
#define ADDR_CONFIG                   (0x00)
//接收中断屏蔽控制,复位值0. 0:接收中断使能(RX_DR中断标志在IRQ产生中断信号) 1:接收中断关闭(RX_DR不影响IRQ电平)
#define CONFIG_MSK_RX_DR              (0x01 << 6)
//发射中断屏蔽控制,复位值0. 0:发射中断使能 1:发射中断关闭
#define CONFIG_MSK_TX_DS              (0x01 << 5)
//最大重发计数中断屏蔽控制,复位值0. 0:使能 1:关闭
#define CONFIG_MSK_MAX_RT             (0x01 << 4)
//使能CRC控制,复位值1,如果EN_AA不全为0时,EN_CRC必须为1, 0:关闭CRC 1:开启CRC
#define CONFIG_MSK_EN_CRC             (0x01 << 3)
//CRC长度配置,复位值0 0:1byte 1:2bytes
#define CONFIG_MSK_CRCO               (0x01 << 2)
//关断/开机模式配置,复位值0 0:关断模式 1:开机模式
#define CONFIG_MSK_PWR_UP             (0x01 << 1)
//发射/接收配置,复位值0 0:发射模式 1:接收模式
#define CONFIG_MSK_PRIM_RX            (0x01 << 0)

//使能自动控制
#define ADDR_EN_AA                    (0x01)
//使能数据管道5自动确认,复位值1
#define EN_AA_MSK_ENAA_P5             (0x01 << 5)
#define EN_AA_MSK_ENAA_P4             (0x01 << 4)
#define EN_AA_MSK_ENAA_P3             (0x01 << 3)
#define EN_AA_MSK_ENAA_P2             (0x01 << 2)
#define EN_AA_MSK_ENAA_P1             (0x01 << 1)
#define EN_AA_MSK_ENAA_P0             (0x01 << 0)

//使能接收数据管道地址
#define ADDR_EN_RXADDR                (0x02)
//使能数据管道5,复位值0
#define EN_RXADDR_MSK_ERX_P5          (0x01 << 5)
#define EN_RXADDR_MSK_ERX_P4          (0x01 << 4)
#define EN_RXADDR_MSK_ERX_P3          (0x01 << 3)
#define EN_RXADDR_MSK_ERX_P2          (0x01 << 2)
//使能数据管道5,复位值1
#define EN_RXADDR_MSK_ERX_P1          (0x01 << 1)
//使能数据管道5,复位值1
#define EN_RXADDR_MSK_ERX_P0          (0x01 << 0)

//地址宽度配置,复位值0b11 [1:0] 01:3bytes 10:4bytes 11:5bytes
#define ADDR_SETUP_AW                 (0x03)
#define SETUP_AW_MSK_AW_1             (0x01 << 1)
#define SETUP_AW_MSK_AW_0             (0x01 << 0)

//自动重发配置
#define ADDR_SETUP_RETR               (0x04)
//自动重发延时配置,复位值0b0000:0b0000~0b1111
#define SETUP_RETR_MSK_ARD(__VAL__)   ((__VAL__) << 4)
//最大自动重发次数,复位值0b0011(三次):0b0000~0b1111
#define SETUP_RETR_MSK_ARC(__VAL__)   ((__VAL__))

//射频信道
#define ADDR_RF_CH                    (0x05)
//设置芯片工作时的信道,0~125个信道,复位值0b10(2402MHz)
#define RF_CH_MSK(__VAL__)            ((__VAL__))

//射频配置
#define ADDR_RF_SETUP                 (0x06)
//复位0,为1时,使能恒载波发射模式，用来测试发射功率
#define RF_SETUP_MSK_CONT_WAVE        (0x01 << 7)
//设置射频数据率,复位10(250Kbps) [1:0] 00:1Mbps 01:2Mbps 11:250kbps
#define RF_SETUP_MSK_RF_DR_0          (0x01 << 5)
#define RF_SETUP_MSK_RF_DR_1          (0x01 << 3)
//设置TX发射功率,复位110 [2:0] 111:7dBm 110:4dBm 101:3dBm 100:1dBm 011:0dBM 010:-4dBm 001:-6dBm 000:-12dBM
#define RF_SETUP_MSK_RF_PWR_2         (0x01 << 2)
#define RF_SETUP_MSK_RF_PWR_1         (0x01 << 1)
#define RF_SETUP_MSK_RF_PWR_0         (0x01 << 0)

//状态寄存器
#define ADDR_STATUS                   (0x07)
//RX FIFO有值标志位,复位0,1清除
#define STATUS_MSK_RX_DR              (0x01 << 6)
//发射端发射完成中断位,复位0,如果是ACK模式,则收到ACK确认信号后TX_DS位置1,写1清除
#define STATUS_MSK_TX_DS              (0x01 << 5)
//达到最大重发次数中断位,复位0,写1清除
#define STATUS_MSK_MAX_RT             (0x01 << 4)
//收到数据的接收管道PPP号,复位111,只读,可通过SPI读出 [3:1] 000~101:数据管道0~5 111:RX FIFO为空
#define STATUS_MSK_RX_P_NO_3          (0x01 << 3)
#define STATUS_MSK_RX_P_NO_2          (0x01 << 2)
#define STATUS_MSK_RX_P_NO_1          (0x01 << 1)
//TX FIFO满标志位,复位值0,只读
#define STATUS_MSK_TX_FULL            (0x01 << 0)

//发射结果统计
#define ADDR_OBSERVE_TX               (0x08)
//丢包计数,最大计数15,改变RF_CH后重新计数
#define OBSERVE_TX_MSK_PLOS_CNT(__VAL__) ((__VAL__) << 4)
//重发计数,发射一个新包时,ARC_CNT重新计数
#define OBSERVE_TX_MSK_ARC_CNT(__VAL__)  ((__VAL__))

//接收信号强度检测
#define ADDR_RSSI                     (0x09)
//如果接收信号强度小于-60dBm则为0
#define RSSI_MSK_RSSI                 (0x01)

//数据管道0的接收地址,最大宽度5bytes,复位值0xE7E7E7E7E7 [39:0] 地址
//LSByte最先写入,通过SETUP_AW配置地址宽度
#define ADDR_RX_ADDR_P0               (0x0A)
//数据管道1的接收地址,同上,复位值0xC2C2C2C2C2
#define ADDR_RX_ADDR_P1               (0x0B)
//数据管道2的接收地址的最低字节,接收地址高字节与RX_ADDR_P1[39:8]相同,复位值0xC3
#define ADDR_RX_ADDR_P2               (0x0C)
//数据管道3的接收地址的最低字节,接收地址高字节与RX_ADDR_P1[39:8]相同,复位值0xC4
#define ADDR_RX_ADDR_P3               (0x0D)
//数据管道4的接收地址的最低字节,接收地址高字节与RX_ADDR_P1[39:8]相同,复位值0xC5
#define ADDR_RX_ADDR_P4               (0x0E)
//数据管道5的接收地址的最低字节,接收地址高字节与RX_ADDR_P1[39:8]相同,复位值0xC6
#define ADDR_RX_ADDR_P5               (0x0F)

//发射方的发射地址(LSByte最先写入),如果发射方需要收ACK,则需要配置RX_ADDR_P0的值等于TX_ADDR,并使能ARQ
//复位值0xE7E7E7E7E7
#define ADDR_TX_ADDR                  (0x10)

//接收数据管道0数据字节数寄存器
#define ADDR_RX_PW_P0                 (0x11)
//接收数据管道0数据字节数
#define RX_PW_P0_MSK                  (0b111111)

//接收数据管道1数据字节数寄存器
#define ADDR_RX_PW_P1                 (0x12)
//接收数据管道1数据字节数
#define RX_PW_P1_MSK                  (0b111111)

//接收数据管道2数据字节数寄存器
#define ADDR_RX_PW_P2                 (0x13)
//接收数据管道2数据字节数
#define RX_PW_P2_MSK                  (0b111111)

//接收数据管道3数据字节数寄存器
#define ADDR_RX_PW_P3                 (0x14)
//接收数据管道0数据字节数
#define RX_PW_P3_MSK                  (0b111111)

//接收数据管道4数据字节数寄存器
#define ADDR_RX_PW_P4                 (0x15)
//接收数据管道0数据字节数
#define RX_PW_P4_MSK                  (0b111111)

//接收数据管道5数据字节数寄存器
#define ADDR_RX_PW_P5                 (0x16)
//接收数据管道5数据字节数
#define RX_PW_P5_MSK                  (0b111111)

//FIFO状态
#define ADDR_FIFO_STATUS              (0x17)
//只用于发送端,FIFO数据重新利用
//当用RESUSE_TX_PL命令后,发射上次以成功发射的消息,通过W_TX_PAYLOAD或FLUSH TX命令关闭该功能
#define FIFO_STATUS_MSK_TX_REUSE      (0x01 << 6)
//TX FIFO满标志
#define FIFO_STATUS_MSK_TX_FULL       (0x01 << 5)
//TX FIFO空标志,复位值1
#define FIFO_STATUS_MSK_TX_EMPTY      (0x01 << 4)
//RX FIFO满标志
#define FIFO_STATUS_MSK_RX_FULL       (0x01 << 1)
//RX EMPTY空标志,复位值1
#define FIFO_STATUS_MSK_RX_EMPTY      (0x01 << 0)

//使能动态负载长度
#define ADDR_DYNPD                    (0x1C)
//使能接收管道5动态负载长度(需EN_DPL及ENAA_P5)
#define DYNPD_MSK_DPL_P5              (0x01 << 5)
#define DYNPD_MSK_DPL_P4              (0x01 << 4)
#define DYNPD_MSK_DPL_P3              (0x01 << 3)
#define DYNPD_MSK_DPL_P2              (0x01 << 2)
#define DYNPD_MSK_DPL_P1              (0x01 << 1)
#define DYNPD_MSK_DPL_P0              (0x01 << 0)

//特性寄存器
#define ADDR_FEATURE                  (0x1D)
//使能动态负载长度
#define FEATURE_MSK_EN_DPL            (0x01 << 2)
//使能ACK负载,带负载数据的ACK包
#define FEATURE_MSK_EN_ACK_PAYd       (0x01 << 1)
//使能命令W_TX_PAYLOAD_NOACK
#define FEATURE_MSK_EN_DYN_ACK        (0x01 << 0)

//************************寄存器操作***********************************************

//获取
#define __GET_REG_BIT(__REG__,__REG_MSK__)       ((__REG__) &  (  __REG_MSK__))
//置位
#define __SET_REG_BIT(__REG__,__REG_MSK__)    do{((__REG__) |= (  __REG_MSK__));}while(0)
//清除
#define __CLR_REG_BIT(__REG__,__REG_MSK__)    do{((__REG__) &= (~ __REG_MSK__));}while(0)

//************************数据结构***********************************************

//是否应答
typedef enum{
    ACK_MODE_DIS = 0,
    ACK_MODE_EN
}Si24R1_ACK_MODE_t;
//地址宽度
typedef enum{
    ADDR_WIDTH_3 = 0x01,
    ADDR_WIDTH_4,
    ADDR_WIDTH_5
}Si24R1_ADDR_WIDTH_t;
//是否CRC校验
typedef enum{
    CRC_DIS = 0,
    CRC_EN
}Si24R1_CRC_EN_t;
//CRC校验位长度
typedef enum{
    CRC_LEN_1 = 0,
    CRC_LEN_2
}Si24R1_CRC_LEN_t;

//接收数据管道地址使能
#define RXADDR_EN_ERX_P5    (0x01 << 5)
#define RXADDR_EN_ERX_P4    (0x01 << 4)
#define RXADDR_EN_ERX_P3    (0x01 << 3)
#define RXADDR_EN_ERX_P2    (0x01 << 2)
#define RXADDR_EN_ERX_P1    (0x01 << 1)
#define RXADDR_EN_ERX_P0    (0x01 << 0)

//自动确认
#define EN_AA_ENAA_P5    (0x01 << 5)
#define EN_AA_ENAA_P4    (0x01 << 4)
#define EN_AA_ENAA_P3    (0x01 << 3)
#define EN_AA_ENAA_P2    (0x01 << 2)
#define EN_AA_ENAA_P1    (0x01 << 1)
#define EN_AA_ENAA_P0    (0x01 << 0)

//动态负载长度
#define DYNPD_DPL_P5    (0x01 << 5)
#define DYNPD_DPL_P4    (0x01 << 4)
#define DYNPD_DPL_P3    (0x01 << 3)
#define DYNPD_DPL_P2    (0x01 << 2)
#define DYNPD_DPL_P1    (0x01 << 1)
#define DYNPD_DPL_P0    (0x01 << 0)

//射频速率
typedef enum{
    RF_SPD_250K = 0x02,
    RF_SPD_1M   = 0x00,
    RF_SPD_2M   = 0x01
}Si24R1_RF_SPD;
//射频功率
typedef enum{
    RF_PWR_7DBM  = 0b111,
    RF_PWR_4DBM  = 0b110,
    RF_PWR_3DBM  = 0b101,
    RF_PWR_1DBM  = 0b100,
    RF_PWR_0DBM  = 0b011,
    RF_PWR_N4DBM = 0b010,
    RF_PWR_N6DBM = 0b001,
    RF_PWR_N12DBM= 0b000
}Si24R1_RF_PWR;

typedef enum{
    OPER_SET_REG = 0,
    OPER_CLR_REG,
    OPER_GET_REG
}Si24R1_OPER_t;

typedef enum{
    MODE_TX = 0,
    MODE_RX
}Si24R1_MODE_t;

//是否使能动态负载长度
typedef enum{
    EN_DPL_DIS = 0,
    EN_DPL_EN  = 1
}Si24R1_EN_DPL_t;

//是否使能ACK负载
typedef enum{
    EN_ACK_PAYD_DIS = 0,
    EN_ACK_PAYD_EN  = 1
}Si24R1_EN_ACK_PAYD_t;


typedef struct{
    uint64_t tx_addr;
    uint64_t rx_addr_p0;
    uint64_t rx_addr_p1;
    uint8_t  rx_addr_p2;
    uint8_t  rx_addr_p3;
    uint8_t  rx_addr_p4;
    uint8_t  rx_addr_p5;
}Si24R1_ADDR_Struct_t;

//接收数据管道数据字节数
typedef struct{
    uint8_t rx_pw_p0;
    uint8_t rx_pw_p1;
    uint8_t rx_pw_p2;
    uint8_t rx_pw_p3;
    uint8_t rx_pw_p4;
    uint8_t rx_pw_p5;
}Si24R1_RX_PW_Struct_t;


#define IRQ_MSK_RX_DR   (0x01 << 6)
#define IRQ_MSK_TX_DS   (0x01 << 5)
#define IRQ_MSK_MAX_RT  (0x01 << 4)

//0x520520cc
typedef struct{
    //地址宽度5bytes
    Si24R1_ADDR_WIDTH_t addr_width; //数据宽度
    Si24R1_ACK_MODE_t ack_mode;     //是否需要ACK
    Si24R1_CRC_EN_t crc_en;         //CRC是否使能
    Si24R1_CRC_LEN_t crc_len;       //CRC校验位长度
    uint16_t ard_us;                //自动重发延迟
    uint8_t arc_times;              //最大自动重发次数
    uint8_t rf_ch;                  //芯片工作的信道,从2400MHz开始
    Si24R1_RF_SPD rf_spd;           //射频速率
    Si24R1_RF_PWR rf_pwr;           //射频功率
    uint8_t aa_en;                  //自动确认使能组
    uint8_t rxaddr_en;              //接收数据管道组
    Si24R1_ADDR_Struct_t addr;      //地址配置 
    Si24R1_RX_PW_Struct_t rx_pw;    //接收数据管道数据字节数
    Si24R1_MODE_t mode;             //发射/接收模式
    uint8_t dynpd;                  //动态负载长度
    Si24R1_EN_DPL_t en_dpl;         //使能动态负载长度
    Si24R1_EN_ACK_PAYD_t en_ack_payd;//使能ACK负载
    uint8_t irq_msk;                //中断屏蔽控制
}Si24R1_Conf_t;

typedef struct{
    uint8_t msg[32];
    uint8_t len;
    uint8_t ppp;
    uint8_t rssi;
}Si24R1_Data_t;


//用于接收到数据触发用户回调
typedef void(*reci_data_cb_t)(Si24R1_Data_t* data);

void sDRV_Si24R1_SetShutdown();
void sDRV_Si24R1_SetStandby();
void sDRV_Si24R1_SetIdleTX();
void sDRV_Si24R1_StartRX();

void sDRV_Si24R1_SetMode(Si24R1_MODE_t mode);
void sDRV_Si24R1_SetRFPWR(Si24R1_RF_PWR rf_pwr);
void sDRV_Si24R1_SetRFSPD(Si24R1_RF_SPD rf_spd);
void sDRV_Si24R1_SetRFCH(uint8_t rf_ch);

uint8_t sDRV_Si24R1_IsTranCplt();

int8_t sDRV_Si24R1_TranDataNoAck(uint8_t* data,uint8_t length);
int8_t sDRV_Si24R1_TranDataAck(uint8_t* data,uint8_t length);

void sDRV_Si24R1_Init(Si24R1_Conf_t* init_conf_struct,reci_data_cb_t reci_cb);
void sDRV_Si24R1_IrqHandler();


#ifdef __cplusplus
}
#endif

#endif
