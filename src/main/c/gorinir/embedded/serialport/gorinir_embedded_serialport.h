#ifndef GORINIR_EMBEDDED_SERIALPORT_H
#define GORINIR_EMBEDDED_SERIALPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern void gorinir_embedded_serialport_usb_transfer(uint8_t* buffer, uint32_t* length);

#ifdef __cplusplus
}
#endif

#endif // GORINIR_EMBEDDED_SERIALPORT_H
