#include "sBSP_softI2C.h"

//软件I2C,STM32版本 v1.0 by sightseer. in9607

#define GPIO_SCL        GPIOB
#define GPIO_SCL_PIN    GPIO_PIN_7
#define GPIO_SDA        GPIOB
#define GPIO_SDA_PIN    GPIO_PIN_6

#define XUS    (10)


static inline void delay(){
    for(uint16_t i = 0;i < 100;i++){
        __NOP();
    }
    
}

static inline void setSCL(uint8_t lv){
    HAL_GPIO_WritePin(GPIO_SCL,GPIO_SCL_PIN,lv);
}

static inline void setSDA(uint8_t lv){
    HAL_GPIO_WritePin(GPIO_SDA,GPIO_SDA_PIN,lv);
}

static inline uint8_t getSDA(){
    return HAL_GPIO_ReadPin(GPIO_SDA,GPIO_SDA_PIN);
}




void sBSP_softI2C_Init(){
    __GPIOB_CLK_ENABLE();
        
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pin = GPIO_SCL_PIN | GPIO_SDA_PIN;
    //GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = 0x0;
    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);

    setSCL(1); setSDA(1);

}

/**
  * @brief  I2C启动传输
  * @param  无
  * @retval 无
  */
void sBSP_softI2C_Start(){
	setSDA(1);
	setSCL(1);
	delay();
	setSDA(0);
	delay();
	setSCL(0);
	delay();
}

/**
  * @brief  I2C停止传输
  * @param  无
  * @retval 无
  */
void sBSP_softI2C_Stop(){
	setSDA(0);
	delay();
	setSCL(1);
	delay();
	setSDA(1);
	delay();
}

/**
  * @brief  I2C发送一个字节
  * @param  要发送的字节
  * @retval 无
  */
void sBSP_softI2C_SendByte(uint8_t byte){
	uint8_t i;
	for(i = 0; i <= 7; i++){
		setSDA(byte & (0x80 >> i));		//取出最高一位
		delay();
		setSCL(1);		//给一个SCL下降沿脉冲
		delay();
		setSCL(0);
		delay();
	}
}

/**
  * @brief  I2C接收一个字节
  * @param  无
  * @retval 接收到的字节
  */
uint8_t sBSP_softI2C_ReciByte(){
	uint8_t byte = 0x00;
	uint8_t i = 0;
	
	setSDA(1);
	delay();
	for(i = 0;i < 8;i++){
		setSCL(1);
		delay();
		if(getSDA()){byte |= (0x80 >> i);}
		setSCL(0);
		delay();
	}
    setSDA(1);
	return byte;
}

/**
  * @brief  I2C发送一个应答
  * @param  ACKBIT:0为应答,1为非应答
  * @retval 无
  */
void sBSP_softI2C_SendACK(uint8_t ACKBIT){
	setSDA(ACKBIT);
	setSCL(1);
	delay();
	setSCL(0);
	delay();
}

/**
  * @brief  I2C接收一个应答
  * @param  无
  * @retval 接收到的应答
  */
uint8_t sBSP_softI2C_ReciACK(){
	uint8_t ACKBIT;
	setSDA(1);
	delay();
	setSCL(1);
	ACKBIT = getSDA();
	delay();
	setSCL(0);
	delay();
	return ACKBIT;
}


void sBSP_softI2C_SendData(uint16_t DevAddress, uint8_t *pData, uint16_t Size){
    sBSP_softI2C_Start();
    // 发送设备地址和写操作位
    sBSP_softI2C_SendByte((DevAddress) & 0xFE); // 确保最低位是0（写操作）
    sBSP_softI2C_ReciACK();
    for(uint16_t i = 0; i < Size; i++){
        sBSP_softI2C_SendByte(pData[i]);
        sBSP_softI2C_ReciACK();
    }
    sBSP_softI2C_Stop();
}

void sBSP_softI2C_ReciData(uint16_t DevAddress, uint8_t *pData, uint16_t Size){
    sBSP_softI2C_Start();
    // 发送设备地址和读操作位
    sBSP_softI2C_SendByte((DevAddress) | 1); // 确保最低位是1（读操作）
    sBSP_softI2C_ReciACK();
    for(uint16_t i = 0; i < Size; i++){
        pData[i] = sBSP_softI2C_ReciByte();
        // 对最后一个字节发送NACK，其他字节发送ACK
        if (i < Size - 1) {
            sBSP_softI2C_SendACK(0); // 发送ACK
        } else {
            sBSP_softI2C_SendACK(1); // 最后一个字节后发送NACK
        }
    }
    sBSP_softI2C_Stop();
}


