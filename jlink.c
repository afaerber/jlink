/*
 * Helpers for communicating with Segger J-Link
 *
 * Copyright (c) 2011 Andreas Färber <andreas.faerber@web.de>
 *
 * Licensed under the GNU LGPL version 2.1 or (at your option) any later version.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include "jlink.h"

#define RETRY_MAX 5
#define ENDPOINT_OUT 0x02
#define ENDPOINT_IN  0x81

// RM08001 5.2 Protocol commands
enum {
    EMU_CMD_VERSION = 0x01,
};


static int send_command(libusb_device_handle *handle,
                        uint8_t *buffer, int buffer_length)
{
    printf("Sending command:");
    for(int i = 0; i < buffer_length; i++) {
        printf(" %02" PRIX8, buffer[i]);
    }
    printf("\n");

    uint8_t endpoint = ENDPOINT_OUT;
    int transferred;
    int ret;
    int try = 0;
    do {
        ret = libusb_bulk_transfer(handle, endpoint, buffer, buffer_length,
                                   &transferred, JLINK_TIMEOUT_MS);
        if (ret == LIBUSB_ERROR_PIPE) {
            libusb_clear_halt(handle, endpoint);
        }
        try++;
    } while ((ret == LIBUSB_ERROR_PIPE) && (try < RETRY_MAX));
    if (ret != LIBUSB_SUCCESS) {
        fprintf(stderr, "%s: sending failed: %d\n", __func__, ret);
        return -1;
    }
    if (transferred != buffer_length) {
        fprintf(stderr, "%s: transferred unexpected amount: %d (%d)\n", __func__, transferred, buffer_length);
        return -1;
    }
    return 0;
}

static int receive_reply(libusb_device_handle *handle,
                         uint8_t *buffer, int length, int *transferred)
{
    uint8_t endpoint = ENDPOINT_IN;
    int ret;
    int try = 0;
    do {
        ret = libusb_bulk_transfer(handle, endpoint, buffer, length,
                                   transferred, JLINK_TIMEOUT_MS);
        if (ret == LIBUSB_ERROR_PIPE) {
            libusb_clear_halt(handle, endpoint);
        }
        try++;
    } while ((ret == LIBUSB_ERROR_PIPE) && (try < RETRY_MAX));
    if (ret != LIBUSB_SUCCESS) {
        fprintf(stderr, "%s: receiving failed: %d (transferred %d)\n", __func__, ret, *transferred);
        return -1;
    }

    printf("Received data:");
    for(int i = 0; i < *transferred; i++) {
        printf(" %02" PRIX8, buffer[i]);
    }
    printf("\n");

    if (*transferred != length) {
        fprintf(stderr, "%s: received unexpected amount: %d (expected %d)\n", __func__, *transferred, length);
        return -2;
    }
    return 0;
}

int jlink_get_version(libusb_device_handle *handle, char **version)
{
    uint8_t buf[1];
    buf[0] = EMU_CMD_VERSION;
    int ret = send_command(handle, buf, 1);
    if (ret != 0)
        return ret;
    int received;
    uint16_t len;
    ret = receive_reply(handle, (uint8_t *)&len, 2, &received);
    if (ret != 0)
        return ret;
    len = u16(len);
    *version = malloc(len + 1);
    ret = receive_reply(handle, (uint8_t *)(*version), len, &received);
    if (ret != 0) {
        free(*version);
        return ret;
    }
    (*version)[len] = '\0';
    return 0;
}
