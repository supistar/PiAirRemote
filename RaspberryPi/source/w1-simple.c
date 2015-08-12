/* vim: set et fenc=utf-8 ff=unix ts=4 sw=4 sts=4 : */

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main (void) {
    DIR *dir;
    struct dirent *dirent;
    char dev[16];      // Dev ID
    char devPath[128]; // Path to device
    char buf[256];     // Data from device
    char tmpData[6];   // Temp C * 1000 reported by device 
    char path[] = "/sys/bus/w1/devices"; 
    ssize_t numRead;
    time_t timer;
    char *ctime(const time_t *timer);
    char *current;

    dir = opendir(path);
    if (dir != NULL) {
        while (dirent = readdir (dir)) {
            // 1-wire devices are linked with 28- prefix
            if (dirent->d_type == DT_LNK && strstr(dirent->d_name, "28-") != NULL) { 
                strcpy(dev, dirent->d_name);
                printf("Device: %s\n", dev);
            }
        }
        (void) closedir (dir);
    } else {
        perror ("Couldn't open the w1 devices directory");
        return 1;
    }

    // Assemble path to OneWire device
    sprintf(devPath, "%s/%s/w1_slave", path, dev);

    // Read temperature continuously
    // Opening the device's file triggers new reading
    while (1) {
        int fd = open(devPath, O_RDONLY);
        if (fd == -1) {
            perror ("Couldn't open the w1 device.");
            return 1;
        }
        while ((numRead = read(fd, buf, 256)) > 0) {
            strncpy(tmpData, strstr(buf, "t=") + 2, 5); 
            float tempC = strtof(tmpData, NULL);
            timer = time(NULL);
            current = ctime(&timer);
            strtok(current, "\n\0");
            printf("[Time: %s] ", current); 
            printf("Device: %s - ", dev); 
            printf("Temp: %.3f C  ", tempC / 1000);
            printf("%.3f F\n", (tempC / 1000) * 9 / 5 + 32);
        }
        close(fd);
    }
    return 0;
}

