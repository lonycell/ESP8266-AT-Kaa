#include "cmd.h"
#include <freertos/FreeRTOS.h>
#include <espressif/esp_sta.h>
#include <memory.h>
#include "driver/uart.h"

struct station_config sta_cfg;

void cmd_init() {
    memset(&sta_cfg, 0, sizeof(struct station_config));
}

void cmd_print(const char *str) {
    while(*str)
        uart_tx_one_char(UART0, *str++);
    uart_tx_one_char(UART0, '\r');
    uart_tx_one_char(UART0, '\n');
}

