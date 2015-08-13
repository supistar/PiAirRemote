/* vim: set et fenc=utf-8 ff=unix ts=4 sw=4 sts=4 : */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kii_cloud.h>
#include <jansson.h>
#include <sys/time.h>
#include "config.h"
#include "w1.h"

long long get_current_time_millis(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (long long)((((long long) t.tv_sec) * 1000) + (((long long) t.tv_usec) / 1000));
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

    // KiiThing
    config_t* credential = NULL;
    char* thing_id = NULL;
    char* thing_token = NULL;
    config_error_code_t conf_error_credential = config_load("credential.json", &credential);
    config_error_code_t read_error_id = config_get(credential, "ThingID", &thing_id);
    config_error_code_t read_error_token = config_get(credential, "ThingToken", &thing_token);
    if (conf_error_credential || read_error_id || read_error_token) {
        goto CLEANUP_CONFIG_CREDENTIAL;
    }

    kii_app_t app = kii_init_app(app_id, app_key, site_url);
    kii_thing_t kii_thing = kii_thing_deserialize(thing_id);

    // Initialize target kii bucket
    const char* bucket_name = "temperature";
    kii_bucket_t kii_bucket = kii_init_thing_bucket(kii_thing, bucket_name);

    // Get temperature
    float temp = w1_get_temperature();

    // Create json
    json_error_t json_error;
    json_t* json = NULL;

    printf("Current time: %lld\n", get_current_time_millis());
    json = json_pack("{s:f, s:I}", "temperature", temp, "time", (json_int_t)(get_current_time_millis()));
    if (json == NULL) {
        goto CLEANUP_KIIBUCKET;
    }

    // Upload json as kii object
    kii_char_t* object_id = NULL;
    kii_char_t* etag = NULL;

    error_code = kii_create_new_object(app, thing_token, kii_bucket, json, &object_id, &etag);
    if (error_code != KIIE_OK) {
        goto CLEANUP_JSON;
    }

    printf("Bucket name: %s\n", bucket_name);
    printf("Temperature: %f\n", temp);
    printf("Object ID: %s\n", object_id);
    printf("ETag: %s\n", etag);

    kii_dispose_kii_char(object_id);
    kii_dispose_kii_char(etag);

CLEANUP_JSON:
    json_decref(json);

CLEANUP_KIIBUCKET:
    kii_dispose_bucket(kii_bucket);

CLEANUP_KIITHING:
    kii_dispose_thing(kii_thing);
    kii_dispose_app(app);

CLEANUP_CONFIG_CREDENTIAL:
    config_decref(credential);

CLEANUP_CONFIG_APPINFO:
    config_decref(app_info);

CLEANUP:
    kii_global_cleanup();

END:
    return EXIT_SUCCESS;
}

