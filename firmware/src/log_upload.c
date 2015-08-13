#include "log_upload.h"

#include <stdint.h>
#include <memory.h>
#include <string.h>

#include <kaa/kaa_logging.h>
#include <kaa/gen/kaa_logging_gen.h>

int log_upload(kaa_client_t *kaa_client, char *str) {
    if (!str || !kaa_client)
        return 1;
    kaa_user_log_record_t *log_record = kaa_logging_log_create();
    if(!log_record)
        return 1;

    log_record->json = kaa_string_copy_create(str); //ppvPortMalloc(strlen(str)+1);

    kaa_error_t err = kaa_logging_add_record(
                      kaa_client_get_context(kaa_client)->log_collector,
                      log_record);

    log_record->destroy(log_record);

    return err;
}

