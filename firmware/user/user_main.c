#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stdio.h>
#include <mem.h>

#include <osapi.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>

#include "driver/uart.h"

#define DEFAULT_STACK_SIZE 512

void main_task(void *pvParameters) {
    (void)pvParameters;
    printf("Stack highwater mark: %d\r\n",
            uxTaskGetStackHighWaterMark(xTaskGetCurrentTaskHandle()));
    printf("main_task() started\r\n");
loop:
    while(1); 
}

void ICACHE_FLASH_ATTR user_init() {
    cmd_init();
    uart_init_new();
    UART_SetBaudrate(UART0, 115200);
    system_print_meminfo();
    printf("Free heap: %d\r\n", system_get_free_heap_size());
    
    /* redirect system garbage messages to UART1 */
    UART_SetPrintPort(UART1);

}

/* Required, don't touch */
void ets_putc(char c) {
    os_putc(c);
 }
