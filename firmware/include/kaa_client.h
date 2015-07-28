#ifndef __KAACLIENT_H
#define __KAACLIENT_H

typedef enum {
    /* KAA to CMD */
    KAA_CMD_CREATED,
    KAA_CMD_ERR,
    KAA_CMD_FINISHED,


    /* CMD to KAA */
    KAA_CMD_STOP_CLIENT,
    KAA_CMD_PROFILE_UPDATE
} kaa_cmd_evt_t;

/*
 * Creates and starts kaa_client as separate OS task
 */
void kaa_run(void *pvParameters);

/*
 *  external_process_fn
 *  for kaa_start
 */
void kaa_loop_task(void *context);

#endif /* __KAACLIENT_H */
