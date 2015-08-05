#include "profile_upd.h"

#include <stdint.h>
#include <memory.h>
#include <string.h>

#include <kaa/kaa_profile.h>
#include <kaa/gen/kaa_profile_gen.h>

int profile_update(kaa_client_t *kaa_client, char *str) {

    if (!kaa_client)
        return 1;
    kaa_profile_t *profile = kaa_profile_profile_create();
    if (!profile)
        return 1;
    if(!str) {
        /* TODO: decide on defaultProfile value */
        static const char *defaultProfile = "{\"Default\":\"Value\"}";
        profile->string = kaa_string_copy_create(defaultProfile);
    } else {
        profile->string = kaa_string_copy_create(str);
    }
    kaa_error_t err = kaa_profile_manager_update_profile(
            kaa_client_get_context(kaa_client)->profile_manager, 
            profile);

    return err;
}
