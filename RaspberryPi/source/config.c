/* vim: set et fenc=utf-8 ff=unix ts=4 sw=4 sts=4 : */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "config.h"

config_error_code_t config_load(const char* config_file_name, config_t** out_config) {
    json_t *json_root;
    json_error_t json_error;

    char config_file_path[256];
    sprintf(config_file_path, "%s/.PiAirRemote/%s", getenv("HOME"), config_file_name);
    json_root = json_load_file(config_file_path, 0, &json_error);

    if (!json_root) {
        perror("Error: Target JSON file does not exist, or garbled\n");
        return CONFIG_FILE_NOT_EXIST;
    }
    *out_config = json_root;
    return CONFIG_OK;
}

config_error_code_t config_get(const config_t* config, const char* config_key, char** out_value) {
    json_t* value = json_object_get(config, config_key);
    if (!value) {
        perror("Error: There is no corresponding value\n");
        return CONFIG_READ_FAIL;
    }

    *out_value = (char*) json_string_value(value);
    return CONFIG_OK;
}

void config_decref(config_t* config) {
    json_decref(config);
}

