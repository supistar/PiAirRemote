/* vim: set et fenc=utf-8 ff=unix ts=4 sw=4 sts=4 : */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kii_cloud.h>
#include <jansson.h>
#include "config.h"

char* _get_raspberry_pi_serial(void) {
    char serial_string[32];
    FILE* file = fopen("/proc/cpuinfo", "r");
    if (!file) {
        return strdup(serial_string);
    }

    char line[256];
    while (fgets(line, 256, file)) {
        if (strncmp(line, "Serial", 6) == 0) {
            strcpy(serial_string, strchr(line, ':') + 2);
            serial_string[strlen(serial_string) - 1] = '\0';
            break;
        }
    }
    return strdup(serial_string);
}

int main(void) {
    kii_error_code_t error_code;

    error_code = kii_global_init();
    if (error_code != KIIE_OK) {
        goto END;
    }

    // Initialize application
    config_t* app_info = NULL;
    char* app_id = NULL;
    char* app_key = NULL;
    char* site_url = NULL;
    config_error_code_t conf_error_info = config_load("appinfo.json", &app_info);
    config_error_code_t read_error_appid = config_get(app_info, "AppID", &app_id);
    config_error_code_t read_error_appkey = config_get(app_info, "AppKey", &app_key);
    config_error_code_t read_error_site = config_get(app_info, "Site", &site_url);
    if (conf_error_info || read_error_appid || read_error_appkey || read_error_site) {
        goto CLEANUP_CONFIG_APPINFO;
    }
    kii_app_t app = kii_init_app(app_id, app_key, site_url);

    // Register KiiThing
    const char* vendor_thing_id = _get_raspberry_pi_serial();
    const char* thing_password = "password";
    kii_thing_t kii_thing = NULL;
    kii_char_t* access_token = NULL;

    printf("Vendor Thing ID: %s\n", vendor_thing_id);
    printf("Password: %s\n", thing_password);

    error_code = kii_register_thing(app, vendor_thing_id, thing_password, NULL, NULL, &kii_thing, &access_token);
    if (error_code != KIIE_OK) {
        printf("Couldn't register new thing. Already registered?\n");
        goto CLEANUP_KII_APP;
    }

    // Print information of registered KiiThing
    kii_char_t* thing_id = kii_thing_serialize(kii_thing);
    printf("Thing ID: %s\n", thing_id);
    printf("Access Token: %s\n", access_token);

    // Create json
    json_t* credential = NULL;

    credential = json_pack("{s:s, s:s}", "ThingID", thing_id, "ThingToken", access_token);
    if (credential == NULL) {
        goto CLEANUP_KII_THINGS;
    }
    config_error_code_t save_error = config_save("credential.json", credential);
    if (save_error) {
        printf("Couldn't save credential configuration\n");
    } else {
        printf("Credential configuration is saved on disk :)\n");
    }

CLEANUP_KII_THINGS:
    kii_dispose_kii_char(thing_id);
    kii_dispose_kii_char(access_token);
    kii_dispose_thing(kii_thing);

CLEANUP_KII_APP:
    kii_dispose_app(app);

CLEANUP_CONFIG_APPINFO:
    config_decref(app_info);

CLEANUP:
    kii_global_cleanup();

END:
    return EXIT_SUCCESS;
}

