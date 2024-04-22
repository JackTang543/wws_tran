#include "sDRV_BMP280.h"
#include "sBSP_I2C.h"

//sDRV_BMP280.c
//Sightseer's BMP280驱动
//
//v2.0 2024.04.16 inHNIP9607
//在第一版上进行了很多优化,包括但不限于:
//优化寄存器操作,对外接口,提供了模式配置...


//使用定点数计算补偿,如果不定义默认使用浮点数计算
#define sDRV_BMP280_USE_FIXED_POINT_COMPE


//引用BSP的I2C句柄
extern I2C_HandleTypeDef hi2c1;
//用来存储BMP280的一些信息
sDRV_BMP280_t bmp280;
//这是算法的校准值
int32_t t_fine;
//寄存器操作
typedef enum{
    OPER_SET_REG = 0,
    OPER_CLR_REG,
    OPER_GET_REG
}OPER_t;


#define BMP280_ADDR         (0x76 << 1)     //这是BMP280的I2C地址

//*****************************************BMP280的寄存器地址*****************************************

#define ADDR_REG_ID         (0xD0)          //芯片ID,读出是0x58
#define ADDR_REG_RESET      (0xE0)          //复位,写入0xB6复位芯片
#define ADDR_REG_STATUS     (0xF3)          //状态寄存器
#define ADDR_REG_CTRL_MEAS  (0xF4)          //控制测量
#define ADDR_REG_CONFIG     (0xF5)          //配置寄存器
#define ADDR_REG_PRESS_MSB  (0xF7)          //气压高位
#define ADDR_REG_PRESS_LSB  (0xF8)          //气压低位
#define ADDR_REG_PRESS_XLSB (0xF9)          //气压次低位
#define ADDR_REG_TEMP_MSB   (0xFA)          //温度高位
#define ADDR_REG_TEMP_LSB   (0xFB)          //温度低位  
#define ADDR_REG_TEMP_XLSB  (0xFC)          //温度次低位
#define ADDR_REG_CAL0       (0x88)          //第0个校准值寄存器的地址

//****************************************一些常数****************************************************

#define VAL_RESET           (0xB6)          //复位写入的值
#define VAL_CHIPID          (0x58)          //芯片ID读出的值

//如果传感器正在测量,这个值就是1
#define REG_STATUS_MSK_MEASURING        (0x01 << 3)
//当NVM数据已经复制完成了就是0
#define REG_STATUS_MSK_IM_UPDATE        (0x01 << 0)
//控制气压过采样 0b000:跳过 0b001:x1 0b010:x2 0b011:x4 0b100: x8 0b101:x16
#define REG_CTRLMEAS_MSK_OSRS_P         (0b0111 << 5)
//控制温度过采样 0b000:跳过 0b001:x1 0b010:x2 0b011:x4 0b100: x8 0b101:x16
#define REG_CTRLMEAS_MSK_OSRS_T         (0b0111 << 2)
//控制器件模式 0b00:Sleep模式 0b01:Forced模式 0b11:Normal模式
#define REG_CTRLMEAS_MSK_MODE_0         (0b0001 << 0)
#define REG_CTRLMEAS_MSK_MODE_1         (0b0001 << 1)
//设置Tstandy时间
#define REG_CONFIG_MSK_T_SB             (0b0111 << 5)
//设置IIR滤波器的时间常数
#define REG_CONFIG_MSK_FILTER           (0b0111 << 2)
//使能3线SPI模式
#define REG_CONFIG_MSK_SPI3W_EN         (0b0001 << 0)

//****************************************一些寄存器操作************************************************

//获取
#define __GET_REG_BIT(__REG__,__REG_MSK__)       ((__REG__) &  (  __REG_MSK__))
//置位
#define __SET_REG_BIT(__REG__,__REG_MSK__)    do{((__REG__) |= (  __REG_MSK__));}while(0)
//清除
#define __CLR_REG_BIT(__REG__,__REG_MSK__)    do{((__REG__) &= (~ __REG_MSK__));}while(0)

//****************************************一些接口******************************************************

//接口,用于自定义延时方式
inline static void delay(uint32_t time){
    HAL_Delay(time);
}

/**
  * @brief  读寄存器
  *
  * @param  addr_reg 读的寄存器的地址
  *
  * @return 读到的值
  */
static uint8_t read_reg(uint8_t addr_reg){
    // sBSP_I2C1M_SeqSendByte(BMP280_ADDR,addr_reg,I2C_FIRST_FRAME);
    // delay(1);
    // sBSP_I2C1M_SeqReadByte(BMP280_ADDR,&tmpreg,I2C_LAST_FRAME);
    // delay(1);
    return sBSP_I2C1M_MemReadByte(BMP280_ADDR,addr_reg,I2C_MEMADD_SIZE_8BIT);
}

/**
  * @brief  写寄存器
  *
  * @param  addr_reg  要操作的寄存器地址
  * @param  data      要写入的值
  *
  * @return 无
  */
static void write_reg(uint8_t addr_reg,uint8_t data){
    sBSP_I2C1M_MemSendByte(BMP280_ADDR,addr_reg,I2C_MEMADD_SIZE_8BIT,data);
    // sBSP_I2C1M_SeqSendByte(BMP280_ADDR,addr_reg,I2C_FIRST_FRAME);
    // delay(1);
    // sBSP_I2C1M_SeqSendByte(BMP280_ADDR,data,I2C_LAST_FRAME);
    // delay(1);
}

/**
  * @brief  对寄存器进行位操作
  *
  * @param  reg_addr 要操作的寄存器地址
  * @param  reg_bit  要操作的位掩码
  * @param  oper     要执行的操作
  *
  * @return 如果进行读操作,返回读的值,否则返回值无效
  */
static uint8_t op_reg(uint8_t reg_addr,uint8_t reg_bit,OPER_t oper){
    //首先把要操作的寄存器读过来
    uint8_t reg = read_reg(reg_addr);
    //正式操作
    //操作寄存器置位
    if(oper == OPER_SET_REG){
        //置位
        __SET_REG_BIT(reg,reg_bit);
        //写回
        write_reg(reg_addr,reg);
    }
    //操作寄存器清位
    else if(oper == OPER_CLR_REG){
        //清空
        __CLR_REG_BIT(reg,reg_bit);
        //写回
        write_reg(reg_addr,reg);
    }
    //获取寄存器位
    else if(oper == OPER_GET_REG){
        //清空读位的其他位
        __CLR_REG_BIT(reg,~reg_bit);
    }
    return reg;
}

/**
  * @brief  BMP280初始化
  *
  * @param  _conf   : 参数结构体
  *
  * @return 如果初始化正常返回0,其他为异常值
  */
int8_t sDRV_BMP280_Init(sDRV_BMP280_Conf_t* _conf){
    //初始化I2C:1KHz
    sBSP_I2C1_Init();
    //读取BMP280里的chip_id寄存器与标准值比较,用于检查通信是否正常
    if(read_reg(ADDR_REG_ID) != VAL_CHIPID){
       return -1;
    }   
    //读取校准寄存器
    sBSP_I2C1M_MemReadBytes(BMP280_ADDR,ADDR_REG_CAL0,I2C_MEMADD_SIZE_8BIT,bmp280.cal_val,26);
    //复位BMP280
    sBSP_I2C1M_SendByte(BMP280_ADDR,VAL_RESET);
    delay(5);

    write_reg(ADDR_REG_CTRL_MEAS,(_conf->osrs_t << 5) | (_conf->osrs_p << 2) | (_conf->mode));
    write_reg(ADDR_REG_CONFIG,(_conf->tsb << 5) | (_conf->filter << 2));

    //sHMI_Debug_Printf("calibration data:");
    //for(uint8_t i = 0;i < 26;i++){
    //    sHMI_Debug_Printf("0x%02X  ",bmp280.cal_val[i]);
    //}
    //sHMI_Debug_Printf("\n\n");
    
    //把接收到的数据变成数据手册里的样子
    //下面两块代码等价
    // for(uint8_t i = 0;i < 12;i++){
    //     *((uint16_t*)(&(bmp280.dig_T1)) + (uint16_t)i) =  \
    //     (((uint16_t)bmp280.cal_val[((i * 2)) + 1]) << 8) | \
    //     ((uint16_t)bmp280.cal_val[(i * 2)]);               
    // }
    for(uint8_t i=0;i<12;i++)*((uint16_t*)(&(bmp280.dig_T1))+(uint16_t)i)=(((uint16_t)bmp280.cal_val[((i*2))+1])<<8)|((uint16_t)bmp280.cal_val[(i*2)]);

    return 0;
}

/**
  * @brief  BMP280启动测量,注意,这个操作本质是让BMP280进入Forced模式
  *
  * @param  无
  *
  * @return 如果正常返回0,其他为异常值
  */
int8_t sDRV_BMP280_StartMeasure(){
    op_reg(ADDR_REG_CTRL_MEAS,REG_CTRLMEAS_MSK_MODE_0,OPER_SET_REG);
    return 0;
}

/**
  * @brief  BMP280获取测量值
  *
  * @param  无
  *
  * @return 如果传感器在忙会返回-1,否则返回0
  */
int8_t sDRV_BMP280_EndMeasure(){
    //读取忙标志
    if(op_reg(ADDR_REG_STATUS,REG_STATUS_MSK_MEASURING,OPER_GET_REG) == 1){return -1;}
    
    sBSP_I2C1M_MemReadBytes(BMP280_ADDR,ADDR_REG_PRESS_MSB,I2C_MEMADD_SIZE_8BIT,&(bmp280.press_msb),6);
    
    return 0;
}

/**
  * @brief  BMP280获取压力数据
  *
  * @param  无
  *
  * @return 气压数据,单位Pa
  */
double sDRV_BMP280_GetPress(){
    //数据合并
    int32_t press = ((uint32_t)(bmp280.press_msb) << 12) | ((uint32_t)(bmp280.press_lsb) << 4) | (((uint32_t)(bmp280.press_xlsb) >> 4));
    
    //如果使用定点运算
    #ifdef sDRV_BMP280_USE_FIXED_POINT_COMPE
    //这些是博世提供的算法
    int64_t var1, var2, p;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)bmp280.dig_P6;
	var2 = var2 + ((var1*(int64_t)bmp280.dig_P5)<<17);
	var2 = var2 + (((int64_t)bmp280.dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)bmp280.dig_P3)>>8) + ((var1 * (int64_t)bmp280.dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)bmp280.dig_P1)>>33;
	if (var1 == 0)
	{
	return 0; // avoid exception caused by division by zero
	}
	p = 1048576-press;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((int64_t)bmp280.dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)bmp280.dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)bmp280.dig_P7)<<4);
	return (double)p / 256.0;
    #else
    //使用浮点运算
    //这些是博世提供的算法
    double var1, var2, p;
	var1 = ((double)t_fine/2.0) - 64000.0;
	var2 = var1 * var1 * ((double)bmp280.dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double)bmp280.dig_P5) * 2.0;
	var2 = (var2/4.0)+(((double)bmp280.dig_P4) * 65536.0);
	var1 = (((double)bmp280.dig_P3) * var1 * var1 / 524288.0 + ((double)bmp280.dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0)*((double)bmp280.dig_P1);
	if (var1 == 0.0)
	{
	return 0; // avoid exception caused by division by zero
	}
	p = 1048576.0 - (double)press;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double)bmp280.dig_P9) * p * p / 2147483648.0;
	var2 = p * ((double)bmp280.dig_P8) / 32768.0;
	p = p + (var1 + var2 + ((double)bmp280.dig_P7)) / 16.0;
	return p;
    #endif
}

/**
  * @brief  BMP280获取温度数据
  *
  * @param  无
  *
  * @return 温度数据,单位degC
  */
double sDRV_BMP280_GetTemp(){
    //数据合并
    int32_t temp = ((uint32_t)(bmp280.temp_msb) << 12) | ((uint32_t)(bmp280.temp_lsb) << 4) | (((uint32_t)(bmp280.press_xlsb) >> 4));
    
    //如果使用定点运算
    #ifdef sDRV_BMP280_USE_FIXED_POINT_COMPE
    int32_t var1, var2, T;
	var1 = ((((temp>>3) - ((int32_t)bmp280.dig_T1<<1))) * ((int32_t)bmp280.dig_T2)) >> 11;
	var2 = (((((temp>>4) - ((int32_t)bmp280.dig_T1)) * ((temp>>4) - ((int32_t)bmp280.dig_T1))) >> 12) * 
	((int32_t)bmp280.dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return (double)T / 100.0;
    #else
    //浮点运算
    double var1, var2, T;
	var1 = (((double)temp)/16384.0 - ((double)bmp280.dig_T1)/1024.0) * ((double)bmp280.dig_T2);
	var2 = ((((double)temp)/131072.0 - ((double)bmp280.dig_T1)/8192.0) *
	(((double)temp)/131072.0 - ((double)bmp280.dig_T1)/8192.0)) * ((double)bmp280.dig_T3);
	t_fine = (int32_t)(var1 + var2);
	T = (var1 + var2) / 5120.0;
    return T;
    #endif
}



