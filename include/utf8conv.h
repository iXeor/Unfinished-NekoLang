#ifndef _INCLUDE_UTF8CONV_H
#define _INCLUDE_UTF8CONV_H

#include <stdint.h>

uint32_t getByteNOOfEncUTF8(int value);
uint32_t getByteNOOfDecUTF8(uint8_t byte);
uint8_t UTF8Encoder(uint8_t *buff, int value);
int UTF8Decoder(const uint8_t *bytePointer, uint32_t length);

#endif