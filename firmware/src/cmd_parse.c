#include "cmd.h"
#include "cmd_parse.h"
#include "cmd_list.h"

#include "kaa_client.h"

#include "config.h"
#include <esp_common.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <espressif/esp_misc.h>

#include <string.h>

#include "driver/uart.h"

extern STATUS uart_tx_one_char(uint8 uart, uint8 c);

extern xQueueHandle xQueueUart;
/* Messages from kaa to cmd  and vice versa*/
extern xQueueHandle kaa_cmd_queue;
extern xQueueHandle cmd_kaa_queue;
extern char *cmd_arg;
extern xSemaphoreHandle cmd_arg_semphr;

static kaa_cmd_evt_t kaa_cmd_evt;
static xTaskHandle kaa_task_handle = NULL;

static uint8 cmd_buffer[MAX_CMD_LEN];
static int cmd_buffer_ptr = 0;

extern struct station_config sta_cfg;


void cmd_parser_task(void *pvParameters) {
    (void)pvParameters;
    char c;
    while (1) {
        if (xQueueReceive(xQueueUart, (void *)&c, (portTickType)portMAX_DELAY)) {
#ifdef CMD_ECHO
           uart_tx_one_char(UART0, c);
            if (c=='\n')
                uart_tx_one_char(UART0, '\r');
#endif /* CMD_ECHO */
            if (c=='\n') {
                cmd_buffer[cmd_buffer_ptr] = 0;
                cmd_parse_cmd();
                memset(cmd_buffer, 0, MAX_CMD_LEN);
                cmd_buffer_ptr = 0;
            } else {
                cmd_buffer[cmd_buffer_ptr++] = c;
            }
            if (cmd_buffer_ptr>=MAX_CMD_LEN)
                cmd_print("\n"CMD_ERR);
        }
    }
}

#define CMD_CMP(cmdstr) (memcmp(cmdstr, cmd_buffer, strlen(cmdstr))==0 && (strlen(cmdstr)==cmd_buffer_ptr? true:cmd_buffer[strlen(cmdstr)]==' '))

/*
 * Sorry for using goto so much.
 * Honestly, I don't like it, but this way the code looks clearier
 */
void cmd_parse_cmd() {
    if (!cmd_buffer_ptr)
        return;
    
    if (CMD_CMP(CMD_CHECK)) {
        cmd_print(CMD_OK);
        return;
    }

    if (CMD_CMP(CMD_RST)) {
        system_restart();
    }
#if 0
    if (CMD_CMP(CMD_SET_SSID)) { 
        size_t cmd_len = strlen(CMD_SET_SSID);
        /* -1 for space between cmd name and ssid */
        size_t ssid_len = cmd_buffer_ptr - cmd_len - 1;
        if(cmd_buffer[cmd_len]!=' ' || ssid_len > sizeof(sta_cfg.ssid))
            goto error;
        memset(sta_cfg.ssid, 0, sizeof(sta_cfg.ssid));
        memcpy(sta_cfg.ssid, cmd_buffer + cmd_len + 1, ssid_len);
        cmd_print(CMD_OK);
        return;
    }
    
    if (CMD_CMP(CMD_SET_PWD)) { 
        size_t cmd_len = strlen(CMD_SET_PWD);
        /* -1 for space between cmd name and pwd */
        size_t pwd_len = cmd_buffer_ptr - cmd_len - 1;
        if(cmd_buffer[cmd_len]!=' ' || pwd_len > sizeof(sta_cfg.password))
            goto error;
        memset(sta_cfg.password, 0, sizeof(sta_cfg.password));
        sta_cfg.password[pwd_len]=0;
        memcpy(sta_cfg.password, cmd_buffer + cmd_len + 1, pwd_len);
        cmd_print(CMD_OK);
        return;
    }
#endif
    if (CMD_CMP(CMD_WIFI_CONNECT)) {
        char *params[2];
        size_t ssid_len, pwd_len;
        char *cmd = cmd_buffer+strlen(CMD_WIFI_CONNECT);
        params[0] = strtok(cmd, " "); /* SSID */
        params[1] = strtok(NULL, " "); /* password */
        
        if (!params[0] || !params[1])
            goto error;
        
        ssid_len = strlen(params[0]);
        pwd_len = strlen(params[1]);

        if (ssid_len > sizeof(sta_cfg.ssid) || pwd_len > sizeof(sta_cfg.password))
            goto error;

        memset(sta_cfg.ssid, 0, sizeof(sta_cfg.ssid));
        memcpy(sta_cfg.ssid, params[0], ssid_len);
        
        memset(sta_cfg.password, 0, sizeof(sta_cfg.password));
        memcpy(sta_cfg.password, params[1], pwd_len);

        /* Set WiFi opmode to station mode */
        if (!wifi_set_opmode_current(STATION_MODE))
            goto error;
        /* Set SSID and PWD */
        if (!wifi_station_set_config_current(&sta_cfg))
            goto error;
       if (!wifi_station_connect())
           goto error;
        uint8 status = wifi_station_get_connect_status();
        while (status==STATION_CONNECTING) {
            status = wifi_station_get_connect_status();
            switch (status) {
                case STATION_WRONG_PASSWORD:
                    goto conn_error;
                case STATION_NO_AP_FOUND:
                    goto conn_error;
                case STATION_CONNECT_FAIL:
                    goto conn_error;
                case STATION_GOT_IP:
                    cmd_print(CMD_OK);
                    return;
                case STATION_IDLE:
                    goto conn_error;
                case STATION_CONNECTING:
                    break;
                default:
                    return;
            }
        }
        conn_error:
        wifi_station_disconnect();
        goto error;
    }

    if (CMD_CMP(CMD_GET_IP)) {
        struct ip_info ip;
        char ipstr[] = IPSTR;
        if (!wifi_get_ip_info(0x00, &ip))
            goto error;
        sprintf(ipstr, IPSTR, IP2STR(&ip.ip));
        cmd_print(ipstr);
        return;
    }

    if (CMD_CMP(CMD_KAA_START)) {
        xTaskCreate(kaa_run, (const signed char*)"", DEFAULT_STACK_SIZE, NULL, 3, &kaa_task_handle);

        if (xQueueReceive(kaa_cmd_queue, (void *)&kaa_cmd_evt, (portTickType)portMAX_DELAY)) {
            switch (kaa_cmd_evt) {
                case KAA_CMD_ERR:
                    goto error;
                case KAA_CMD_CREATED:
                    cmd_print(CMD_OK);
                    return;
                default:
                    goto error;
            }
        }
        goto error;
    }

    if (CMD_CMP(CMD_KAA_STOP)) {
        if (!kaa_task_handle)
            goto error;
        kaa_cmd_evt = KAA_CMD_STOP_CLIENT;
        xQueueSendToBack(cmd_kaa_queue, &kaa_cmd_evt, 0);
        if (xQueueReceive(kaa_cmd_queue, (void*)&kaa_cmd_evt, (portTickType)portMAX_DELAY))
            if (kaa_cmd_evt!=KAA_CMD_ERR) {
                cmd_print(CMD_OK);
                return;
            }
        goto error;
    }

    if(CMD_CMP(CMD_KAA_SEND_PROFILE)) {
        if (!kaa_task_handle || !cmd_arg_semphr)
            goto error;
        if (xSemaphoreTake(cmd_arg_semphr, (portTickType)CMD_KAA_PROC_MAX_DELAY)==pdTRUE) {
            cmd_arg = cmd_buffer + strlen(CMD_KAA_SEND_PROFILE) + 1;
            xSemaphoreGive(cmd_arg_semphr);
        } else
            goto error;

        kaa_cmd_evt = KAA_CMD_PROFILE_UPDATE;
        xQueueSendToBack(cmd_kaa_queue, &kaa_cmd_evt, 0);
        
        if (xQueueReceive(kaa_cmd_queue, (void*)&kaa_cmd_evt, (portTickType)portMAX_DELAY))
            if (kaa_cmd_evt!=KAA_CMD_ERR) {
                cmd_print(CMD_OK);
                return;
            }
        goto error;
    }

error:
    cmd_print(CMD_ERR);
    return;
}

