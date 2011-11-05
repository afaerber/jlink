/*
 * Helpers for communicating with Segger J-Link
 *
 * Copyright (c) 2011 Andreas Färber <andreas.faerber@web.de>
 *
 * Licensed under the GNU LGPL version 2.1 or (at your option) any later version.
 */
#ifndef JLINK_H
#define JLINK_H


#include <libusb-1.0/libusb.h>


#define JLINK_MAX_OUT_TRANSACTION_SIZE 0xffff
#define JLINK_MAX_IN_TRANSACTION_SIZE 0x8000

#define JLINK_TIMEOUT_MS 5000


// TODO LE
static inline uint16_t u16(uint16_t x)
{
    return x;
}

static inline uint32_t u32(uint32_t x)
{
    return x;
}


int jlink_get_version(libusb_device_handle *handle, char **str);
int jlink_get_caps(libusb_device_handle *handle, uint32_t *caps);


#endif
