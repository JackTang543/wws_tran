#include "sDRV_UART.h"

#ifdef __cplusplus
extern "C"{
#endif


void sDRV_UART_Init(uint32_t bandrate){
    sBSP_UART1_Init(bandrate);
}


char format_buffer[128] = {0};

void sDRV_UART_Printf(char *p,...){
    va_list ap;
    va_start(ap,p);
    vsprintf(format_buffer,p,ap);
    va_end(ap);

    sBSP_UART1_SendBytes((uint8_t*)format_buffer,strlen(format_buffer));
}


#ifdef __cplusplus
}
#endif
