/* vim: set et fenc=utf-8 ff=unix ts=4 sw=4 sts=4 : */

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

float w1_get_temperature (void) {
    DIR *dir;
    struct dirent *dirent;
    char dev[16];      // Dev ID
    char devPath[128]; // Path to device
    char buf[256];     // Data from device
    char tmpData[6];   // Temp C * 1000 reported by device 
    char path[] = "/sys/bus/w1/devices"; 
    float tempC = 0.0;

    dir = opendir(path);
    if (dir != NULL) {
        while (dirent = readdir (dir)) {
            // 1-wire devices are links beginning with 28-
            if (dirent->d_type == DT_LNK && strstr(dirent->d_name, "28-") != NULL) { 
                strcpy(dev, dirent->d_name);
                printf("Device: %s\n", dev);
            }
        }
        (void) closedir (dir);
    } else {
        perror("Couldn't open the w1 devices directory");
        return tempC;
    }

    // Assemble path to OneWire device
    sprintf(devPath, "%s/%s/w1_slave", path, dev);

    // Opening the device's file triggers new reading
    int fd = open(devPath, O_RDONLY);
    if (fd == -1) {
        perror("Couldn't open the w1 device.");
        return tempC;
    }
    if (read(fd, buf, 256) > 0) {
        strncpy(tmpData, strstr(buf, "t=") + 2, 5); 
        tempC = strtof(tmpData, NULL) / 1000;
    }
    close(fd);

    return tempC;
}

