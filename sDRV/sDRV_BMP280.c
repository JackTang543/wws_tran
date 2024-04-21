/*
 * sDRV_BMP280.c
 * BMP280气压温度传感器
 * v1.0 by Sightseer. in9607
 * 
 * 
*/

#include "sDRV_BMP280.h"
#include "sBSP_I2C.h"



//这是BMP280的I2C地址
#define BMP280_ADDR           (0x76 << 1)

//这些是BMP280的寄存器地址
#define BMP280_REG_ID         (0xD0)
#define BMP280_REG_RESET      (0xE0)
#define BMP280_REG_STATUS     (0xF3)
#define BMP280_REG_CTRL_MEAS  (0xF4)
#define BMP280_REG_CONFIG     (0xF5)
#define BMP280_REG_PRESS_MSB  (0xF7)
#define BMP280_REG_PRESS_LSB  (0xF8)
#define BMP280_REG_PRESS_XLSB (0xF9)
#define BMP280_REG_TEMP_MSB   (0xFA)
#define BMP280_REG_TEMP_LSB   (0xFB)
#define BMP280_REG_TEMP_XLSB  (0xFC)
#define BMP280_REG_CAL0       (0x88)    //第0个校准值寄存器的地址
//这些是一些值
#define BMP280_VAL_RESET      (0xB6)
#define BMP280_VAL_CHIPID     (0x58)

//引用BSP的I2C句柄
extern I2C_HandleTypeDef hi2c1;

//用来存储BMP280的一些信息
sDrv_BMP280_t bmp280;

//这是算法的校准值
int32_t t_fine;

//接口,用于自定义延时方式
inline static void Delay_ms(uint32_t time){
    HAL_Delay(time);
}

/*@brief  BMP280初始化
*
* @param  无
*
* @return HAL_StatusTypeDef:如果初始化失败(通信异常)会返回HAL_ERROR,否则返回HAL_OK
*/
HAL_StatusTypeDef sDRV_BMP280_Init(){
    //初始化I2C
    sBSP_I2C1_Init();
    //读取BMP280里的chip_id寄存器与标准值比较,用于检查通信是否正常
    uint8_t chip_id = 0;
    uint8_t chip_id_reg = BMP280_REG_ID;
    //发送一帧数据设置数据指针,但并不发送stop
    HAL_I2C_Master_Seq_Transmit_IT(&hi2c1,BMP280_ADDR,&chip_id_reg,1,I2C_FIRST_FRAME);
    Delay_ms(2);
    //接收数据指针指向的内容,发送stop
    HAL_I2C_Master_Seq_Receive_IT(&hi2c1,BMP280_ADDR,&chip_id,1,I2C_LAST_FRAME);
    Delay_ms(2);
    //如果通信异常就返回ERR
    if(chip_id != BMP280_VAL_CHIPID){
        return HAL_ERROR;
    }
    //读取校准寄存器
    uint8_t cal_reg[] = {BMP280_REG_CAL0};
    HAL_I2C_Master_Seq_Transmit_IT(&hi2c1,BMP280_ADDR,cal_reg,1,I2C_FIRST_FRAME);
    Delay_ms(10);
    HAL_I2C_Master_Seq_Receive_IT(&hi2c1,BMP280_ADDR,bmp280.cal_val,26,I2C_LAST_FRAME);
    Delay_ms(100);
    //复位BMP280
    uint8_t reset_reg[] = {BMP280_REG_RESET,BMP280_VAL_RESET};
    HAL_I2C_Master_Transmit_IT(&hi2c1,BMP280_ADDR,reset_reg,2);
    Delay_ms(10);
    
    //设置为超高解析度
    #ifdef sDRV_BMP280_USE_MAX_RESOLUTION
    //气压过采样:x16,温度过采样:x2,IIR滤波:16,Timing:0.5ms,Mode:Normal
    uint8_t conf_seq[] = {BMP280_REG_CTRL_MEAS,0x57,BMP280_REG_CONFIG,0x14};
    HAL_I2C_Master_Transmit_IT(&hi2c1,BMP280_ADDR,conf_seq,4);
    Delay_ms(5);
    #endif

    #ifdef sDRV_BMP280_USE_ULTRA_LOW_PWR
    //气压过采样:x1,温度过采样:x1,IIR滤波:off,Timing:0.5ms,Mode:Normal
    uint8_t conf_seq[] = {BMP280_REG_CTRL_MEAS,0x27,BMP280_REG_CONFIG,0x00};
    HAL_I2C_Master_Transmit_IT(&hi2c1,BMP280_ADDR,conf_seq,4);
    Delay_ms(5);
    #endif

//    sHMI_Debug_Printf("calibration data:");
//    for(uint8_t i = 0;i < 26;i++){
//        sHMI_Debug_Printf("0x%02X  ",bmp280.cal_val[i]);
//    }
//    sHMI_Debug_Printf("\n\n");
    
    //把接收到的数据变成数据手册里的样子
    //下面两块代码等价
    // for(uint8_t i = 0;i < 12;i++){
    //     *((uint16_t*)(&(bmp280.dig_T1)) + (uint16_t)i) =  \
    //     (((uint16_t)bmp280.cal_val[((i * 2)) + 1]) << 8) | \
    //     ((uint16_t)bmp280.cal_val[(i * 2)]);               
    // }

    for(uint8_t i=0;i<12;i++)*((uint16_t*)(&(bmp280.dig_T1))+(uint16_t)i)= \
    (((uint16_t)bmp280.cal_val[((i*2))+1])<<8)|((uint16_t)bmp280.cal_val[(i*2)]);
    
    return HAL_OK;
}

void sDRV_BMP280_SetModeSleep(){
    uint8_t conf_seq[] = {BMP280_REG_CTRL_MEAS,0x24};
    HAL_I2C_Master_Transmit_IT(&hi2c1,BMP280_ADDR,conf_seq,2);
}

/*@brief  BMP280获取测量值
*
* @param  无
*
* @return HAL_StatusTypeDef:如果传感器在忙会返回HAL_ERROR,否则返回HAL_OK
*/
HAL_StatusTypeDef sDRV_BMP280_GetMeasure(){
    uint8_t status_reg_val = 0xFF;
    uint8_t status_reg = BMP280_REG_STATUS;
    //读取状态寄存器
    HAL_I2C_Master_Seq_Transmit_IT(&hi2c1,BMP280_ADDR,&status_reg,1,I2C_FIRST_FRAME);
    Delay_ms(2);
    HAL_I2C_Master_Seq_Receive_IT(&hi2c1,BMP280_ADDR,&status_reg_val,1,I2C_LAST_FRAME);
    Delay_ms(2);
    //读取忙标志
    if((status_reg_val & 0x08) == 1){
        return HAL_ERROR;
    }
    //获取温度气压数据
    uint8_t press_msb_reg = BMP280_REG_PRESS_MSB;
    HAL_I2C_Master_Seq_Transmit_IT(&hi2c1,BMP280_ADDR,&press_msb_reg,1,I2C_FIRST_FRAME);
    Delay_ms(2);
    HAL_I2C_Master_Seq_Receive_IT(&hi2c1,BMP280_ADDR,&(bmp280.press_msb),6,I2C_LAST_FRAME);
    Delay_ms(10);
    
    return HAL_OK;
}

/*@brief  BMP280获取压力数据
*
* @param  无
*
* @return double:气压数据,单位Pa
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

/*@brief  BMP280获取温度数据
*
* @param  无
*
* @return double:温度数据,单位degC
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


