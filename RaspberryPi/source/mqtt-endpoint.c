/* vim: set et fenc=utf-8 ff=unix ts=4 sw=4 sts=4 : */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kii_cloud.h>
#include <jansson.h>
#include <sys/time.h>
#include "config.h"

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

    // Install push
    kii_bool_t is_development = KII_TRUE;
    kii_char_t* installation_id = NULL;

    error_code = kii_install_thing_push(app, thing_token, is_development, &installation_id);
    if (error_code != KIIE_OK) {
        goto CLEANUP_INSTALLATION_ID;
    }
    printf("Installation ID: %s\n", installation_id);

    // Retrieve MQTT endpoint
    kii_mqtt_endpoint_t* endpoint = NULL;
    kii_uint_t retry_after = 0;
    uint retry_count = 0;
    uint retry_max = 3;

GET_MQTT_ENDPOINT:
    error_code = kii_get_mqtt_endpoint(app, thing_token, installation_id, &endpoint, &retry_after);
    if (error_code != KIIE_OK) {
        kii_error_t* last_error = kii_get_last_error(app);
        if (last_error->status_code == 503) {
            retry_count++;
            printf("Retrieving MQTT endpoint is failed. Retry: %u/%u\n", retry_count, retry_max);
            sleep(retry_after);
            if (retry_count < retry_max) {
                goto GET_MQTT_ENDPOINT;
            }
        }
        goto CLEANUP_MQTT_ENDPOINT;
    }

    // Show endpoint information
    char port_tcp_buf[6];
    sprintf(port_tcp_buf, "%u", endpoint->port_tcp);
    const char* port_tcp = strdup(port_tcp_buf);
    printf("MQTT Username: %s\n", endpoint->username);
    printf("MQTT Password: %s\n", endpoint->password);
    printf("MQTT Topic   : %s\n", endpoint->topic);
    printf("MQTT ClientID: %s\n", endpoint->topic);
    printf("MQTT Host    : %s\n", endpoint->host);
    printf("MQTT Port    : %s\n", port_tcp);

    // Create config
    config_t* mqtt = config_object();
    config_error_code_t conf_error = CONFIG_OK;
    conf_error |= config_set(mqtt, "username", endpoint->username);
    conf_error |= config_set(mqtt, "password", endpoint->password);
    conf_error |= config_set(mqtt, "topic", endpoint->topic);
    conf_error |= config_set(mqtt, "client_id", endpoint->topic);
    conf_error |= config_set(mqtt, "host", endpoint->host);
    conf_error |= config_set(mqtt, "port", port_tcp);
    if (conf_error != CONFIG_OK) {
        goto CLEANUP_MQTT_CONFIG;
    }
    config_error_code_t save_error = config_save("mqtt-endpoint.json", mqtt);
    if (save_error) {
        printf("Couldn't save MQTT endpoint configuration\n");
    } else {
        printf("MQTT endpoint configuration is saved on disk :)\n");
    }

CLEANUP_MQTT_CONFIG:
    config_decref(mqtt);

CLEANUP_MQTT_ENDPOINT:
    kii_dispose_mqtt_endpoint(endpoint);

CLEANUP_INSTALLATION_ID:
    kii_dispose_kii_char(installation_id);

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

