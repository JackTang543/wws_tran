#include "sDRV_TEMT6000.h"
#include "sBSP_ADC.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEMT_CH     ADC_CHANNEL_1

#define AVERAGE_NUM     (8)

float sDRV_TEMT_GetLight(){
    uint16_t raw = 0;
    for(uint8_t i = 0;i < AVERAGE_NUM;i++){
        raw += sBSP_ADC_GetChannel(TEMT_CH);
    }


    float raw_val = ((float)raw / AVERAGE_NUM) / 4095.0f * 3300.0f;


    return raw_val;
}



#ifdef __cplusplus
}
#endif
