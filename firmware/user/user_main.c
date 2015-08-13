#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stdio.h>

#include "config.h"
#include "driver/uart.h"

void ICACHE_FLASH_ATTR user_init() {
    cmd_init();
    uart_init_new();
    UART_SetBaudrate(UART0, 115200);
    cmd_print(HELLO_MSG);
   
#ifndef SHOW_SYSTEM_MESSAGES
    /* redirect system garbage messages to UART1 */
    UART_SetPrintPort(UART1);
#endif

}

/* Required, don't touch */
void ets_putc(char c) {
    os_putc(c);
 }
