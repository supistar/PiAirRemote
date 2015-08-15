/* vim: set et fenc=utf-8 ff=unix ts=4 sw=4 sts=4 : */

#include <jansson.h>

typedef enum config_error_code_t {
    CONFIG_OK = 0,
    CONFIG_FILE_NOT_EXIST,
    CONFIG_READ_FAIL,
    CONFIG_WRITE_FAIL,
    CONFIG_GET_FAIL,
    CONFIG_SET_FAIL
} config_error_code_t;

typedef json_t config_t;

config_t* config_object(void);

config_error_code_t config_load(const char* config_file_name, config_t** out_config);

config_error_code_t config_save(const char* config_file_name, const config_t* config);

config_error_code_t config_get(const config_t* config, const char* config_key, char** out_value);

config_error_code_t config_set(config_t* config, const char* config_key, const char* config_value);

void config_decref(config_t* config);

