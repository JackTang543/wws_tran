#include "sBSP_softSPI.h"


#ifdef  __SBSP_SOFT_SPI_EN__


#define GPIO_SCK        GPIOA
#define GPIO_SCK_PIN    GPIO_PIN_5
#define GPIO_MOSI       GPIOA
#define GPIO_MOSI_PIN   GPIO_PIN_7
#define GPIO_MISO       GPIOA
#define GPIO_MISO_PIN   GPIO_PIN_6
#define GPIO_CS         GPIOA
#define GPIO_CS_PIN     GPIO_PIN_4



static inline void delay(){
    //HAL_Delay(1);
}

static inline void setSCK(uint8_t lv){
    HAL_GPIO_WritePin(GPIO_SCK,GPIO_SCK_PIN,lv);
}

static inline void setMOSI(uint8_t lv){
    HAL_GPIO_WritePin(GPIO_MOSI,GPIO_MOSI_PIN,lv);
}

static inline void setMISO(uint8_t lv){
    HAL_GPIO_WritePin(GPIO_MISO,GPIO_MISO_PIN,lv);
}

static inline void setCS(uint8_t lv){
    HAL_GPIO_WritePin(GPIO_CS,GPIO_CS_PIN,lv);
}

static inline uint8_t getMISO(){
    return HAL_GPIO_ReadPin(GPIO_MISO,GPIO_MISO_PIN);
}




void sBSP_softSPI_Init(){
    //sBSP_DWT_Init(HAL_RCC_GetSysClockFreq());
    
    __GPIOA_CLK_ENABLE();
        
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = GPIO_SCK_PIN | GPIO_MOSI_PIN | GPIO_CS_PIN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);

    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = GPIO_MISO_PIN;
    //GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    //SPI MODE:0,CPOL:0,CPHA=0
    //空闲SCK低电平,SCK的第一个边沿移入数据,第二个边沿移出数据
    HAL_GPIO_WritePin(GPIOA,GPIO_SCK_PIN ,0);
    HAL_GPIO_WritePin(GPIOA,GPIO_MOSI_PIN,1);
    HAL_GPIO_WritePin(GPIOA,GPIO_CS_PIN  ,1);

}

void sBSP_softSPI_SetCS(uint8_t lv){
    setCS(lv);
}

void sBSP_softSPI_SendByte(uint8_t byte){
    for(uint8_t i = 0; i < 8;i++){
        setMOSI(byte & (0x80 >> i));
        delay();
        setSCK(1);
        delay();
        setSCK(0);
    }
    setMOSI(0);
    setSCK(0);
}

uint8_t sBSP_softSPI_ReciByte(){
    uint8_t reci_byte = 0;
    for(uint8_t i = 0; i < 8;i++){
        getMISO()?reci_byte |= (0x80 >> i):(void)NULL;
        delay();
        setSCK(1);
        delay();
        setSCK(0);
    }
    return reci_byte;
}

void sBSP_softSPI_SendBytes(uint8_t* pData,uint32_t num){
    for(uint32_t i = 0; i < num;i++){
        for(uint8_t j = 0; j < 8;j++){
            setMOSI(pData[i] & (0x80 >> j));
            delay();
            setSCK(1);
            delay();
            setSCK(0);
        }
        setMOSI(0);
    }
    setSCK(0);
}

void sBSP_softSPI_ReciBytes(uint8_t* pData,uint32_t num){
    for(uint32_t i = 0; i < num;i++){
        pData[i] = sBSP_softSPI_ReciByte();
    }
    setSCK(0);
}

uint8_t sBSP_softSPI_SwapByte(uint8_t send_byte){
    uint8_t reci_byte = 0;
    
    
    
    return reci_byte;
}



#endif

