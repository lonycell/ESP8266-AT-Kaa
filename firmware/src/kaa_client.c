#include "./kaa_client.h"
#include "config.h"
#include "cmd.h"
#include "profile_upd.h"
#include "log_upload.h"

#include <stddef.h>
#include <time.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include <kaa/kaa_error.h>
#include <kaa/platform/kaa_client.h>

#define KAA_CMD_QUEUE_LEN 2

static kaa_client_t *kaa_client = NULL;

xQueueHandle kaa_cmd_queue;
xQueueHandle cmd_kaa_queue;
char *cmd_arg;
xSemaphoreHandle cmd_arg_semphr;

static void kaa_task_destroy() {
    kaa_client = NULL;
    vQueueDelete(cmd_kaa_queue);
    vQueueDelete(kaa_cmd_queue);
    vSemaphoreDelete(cmd_arg_semphr);
    vTaskDelete(xTaskGetCurrentTaskHandle());
}

void kaa_run(void *pvParameters) {
    (void)pvParameters;
    kaa_cmd_evt_t kaa_cmd_evt;

    kaa_cmd_queue = xQueueCreate(KAA_CMD_QUEUE_LEN, sizeof(kaa_cmd_evt_t));
    cmd_kaa_queue = xQueueCreate(KAA_CMD_QUEUE_LEN, sizeof(kaa_cmd_evt_t));
    vSemaphoreCreateBinary(cmd_arg_semphr);
    if(!cmd_arg_semphr || !kaa_cmd_queue || !cmd_kaa_queue)
        kaa_task_destroy();
    
    if(kaa_client) {
        kaa_cmd_evt = KAA_CMD_CREATED;
        xQueueSendToBack(kaa_cmd_queue, &kaa_cmd_evt, 0);
    }

    kaa_error_t err = kaa_client_create(&kaa_client, NULL);
    if(err) {
        kaa_cmd_evt = KAA_CMD_ERR;
        xQueueSendToBack(kaa_cmd_queue, &kaa_cmd_evt, 0);
        kaa_task_destroy();
    } else {
        kaa_cmd_evt = KAA_CMD_CREATED;
        xQueueSendToBack(kaa_cmd_queue, &kaa_cmd_evt, 0);
    }

    if(profile_update(kaa_client, NULL)) {
        kaa_cmd_evt = KAA_CMD_ERR;
        xQueueSendToBack(kaa_cmd_queue, &kaa_cmd_evt, 0);
    }
    err = kaa_client_start(kaa_client, kaa_loop_task, NULL,  0);

    if(err) {
        kaa_cmd_evt = KAA_CMD_ERR;
        xQueueSendToBack(kaa_cmd_queue, &kaa_cmd_evt, 0);
        kaa_task_destroy();
    }

    kaa_client_destroy(kaa_client);

    kaa_cmd_evt = KAA_CMD_FINISHED;
    xQueueSendToBack(kaa_cmd_queue, &kaa_cmd_evt, 0);
    kaa_task_destroy();
}


void kaa_loop_task(void *context) {
    kaa_cmd_evt_t cmd_evt;
    if(xQueueReceive(cmd_kaa_queue, (void*)&cmd_evt, (portTickType)CMD_KAA_LOOP_TASK_MAX_DELAY)) {
        switch(cmd_evt) {
            case KAA_CMD_STOP_CLIENT:
                kaa_client_stop(kaa_client);
/*                cmd_evt = KAA_CMD_FINISHED;
                xQueueSendToBack(kaa_cmd_queue, &cmd_evt, 0);*/
                break;
            case KAA_CMD_PROFILE_UPDATE:
                if(xSemaphoreTake(cmd_arg_semphr, (portTickType)portMAX_DELAY)) {
                    cmd_print(cmd_arg);
                    int e = profile_update(kaa_client, cmd_arg);
                    xSemaphoreGive(cmd_arg_semphr);
                    if(e)
                        cmd_evt = KAA_CMD_ERR;
                    else
                        cmd_evt = KAA_CMD_FINISHED;
                    xQueueSendToBack(kaa_cmd_queue, &cmd_evt, 0);
                } else {
                    cmd_evt = KAA_CMD_ERR;
                    xQueueSendToBack(kaa_cmd_queue, &cmd_evt, 0);
                }
                break;
            case KAA_CMD_LOG_UPLOAD:
                if(xSemaphoreTake(cmd_arg_semphr, (portTickType)CMD_KAA_LOOP_TASK_MAX_DELAY)) {
                    cmd_print(cmd_arg);
                    int e = log_upload(kaa_client, cmd_arg);
                    xSemaphoreGive(cmd_arg_semphr);
                    if(e)
                        cmd_evt = KAA_CMD_ERR;
                    else
                        cmd_evt = KAA_CMD_FINISHED;
                    xQueueSendToBack(kaa_cmd_queue, &cmd_evt, 0);
                } else {
                    cmd_evt = KAA_CMD_ERR;
                    xQueueSendToBack(kaa_cmd_queue, &cmd_evt, 0);
                }
                break;
            default:
                break;
        }
    }
}

