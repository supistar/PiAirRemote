/* vim: set et fenc=utf-8 ff=unix ts=4 sw=4 sts=4 : */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

int main(int argc, char *argv[]) {
    // Load configurations
    config_t* mqtt = NULL;
    char* username = NULL;
    char* password = NULL;
    char* topic = NULL;
    char* client_id = NULL;
    char* host = NULL;
    char* port = NULL;
    config_error_code_t conf_error = CONFIG_OK;
    conf_error |= config_load("mqtt-endpoint.json", &mqtt);
    conf_error |= config_get(mqtt, "username", &username);
    conf_error |= config_get(mqtt, "password", &password);
    conf_error |= config_get(mqtt, "topic", &topic);
    conf_error |= config_get(mqtt, "client_id", &client_id);
    conf_error |= config_get(mqtt, "host", &host);
    conf_error |= config_get(mqtt, "port", &port);
    if (conf_error) {
        printf("Couldn't load MQTT endpoint configuration\n");
        goto CLEANUP_CONFIG_MQTT;
    }
    printf("MQTT endpoint configuration is loaded :)\n");

    printf("MQTT Username: %s\n", username);
    printf("MQTT Password: %s\n", password);
    printf("MQTT Topic   : %s\n", topic);
    printf("MQTT ClientID: %s\n", client_id);
    printf("MQTT Host    : %s\n", host);
    printf("MQTT Port    : %s\n", port);

    const char* filename = "mqttcd";
    char wd_buf[256];
    getcwd(wd_buf, 256);
    char exec_buf[512];
    sprintf(exec_buf, "%s/%s/%s", wd_buf, dirname(argv[0]), filename);

    const char* mqtt_argv[] = {
        exec_buf,
        "--username", username,
        "--password", password,
        "--topic", topic,
        "--client_id", client_id,
        "--host", host,
        "--port", port,
        NULL };
    char* const envp[] = { NULL };
    int ret = execve(exec_buf, mqtt_argv, envp);
    if (ret == -1) {
        printf("Couldn't launch mqttcd process\n");
    }

CLEANUP_CONFIG_MQTT:
    config_decref(mqtt);

END:
    return EXIT_SUCCESS;
}

