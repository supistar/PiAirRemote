/* vim: set et fenc=utf-8 ff=unix ts=4 sw=4 sts=4 : */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "config.h"

char* _get_config_file_path(const char* config_file_name) {
    char config_file_path[256];
    sprintf(config_file_path, "%s/.PiAirRemote/%s", getenv("HOME"), config_file_name);
    return strdup(config_file_path);
}

config_t* config_object(void) {
    return json_object();
}

config_error_code_t config_load(const char* config_file_name, config_t** out_config) {
    json_t *json_root;
    json_error_t json_error;

    char* config_file_path = _get_config_file_path(config_file_name);
    json_root = json_load_file(config_file_path, 0, &json_error);

    if (!json_root) {
        perror("Error: Target JSON file does not exist, or garbled\n");
        return CONFIG_FILE_NOT_EXIST;
    }
    *out_config = json_root;
    free(config_file_path);
    return CONFIG_OK;
}

config_error_code_t config_save(const char* config_file_name, const config_t* config) {
    char* config_file_path = _get_config_file_path(config_file_name);
    int error = json_dump_file(config, config_file_path, 0);

    if (error != 0) {
        perror("Error: Error happens on saving JSON file\n");
        return CONFIG_WRITE_FAIL;
    }
    free(config_file_path);
    return CONFIG_OK;
}

config_error_code_t config_get(const config_t* config, const char* config_key, char** out_value) {
    json_t* value = json_object_get(config, config_key);
    if (!value) {
        perror("Error: There is no corresponding value\n");
        return CONFIG_GET_FAIL;
    }

    *out_value = (char*) json_string_value(value);
    return CONFIG_OK;
}

config_error_code_t config_set(config_t* config, const char* config_key, const char* config_value) {
    int ret = json_object_set(config, config_key, json_string(config_value));
    if (!ret) {
        perror("Error: There is no corresponding value\n");
        return CONFIG_SET_FAIL;
    }
    return CONFIG_OK;
}

void config_decref(config_t* config) {
    json_decref(config);
}

