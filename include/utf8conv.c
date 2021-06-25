#include "utf8conv.h"
#include "common.h"

uint32_t getByteNOOfEncUTF8(int value)
{
    ASSERT(value > 0, "Can`t encode negative value!");
    if (value <= 0x7F)
    {
        return 1;
    }
    if (value <= 0x7FF)
    {
        return 2;
    }
    if (value <= 0xFFFF)
    {
        return 3;
    }
    if (value <= 0x10FFFF)
    {
        return 4;
    }

    return 0;
}
uint8_t UTF8Encoder(uint8_t *buffer, int value)
{
    ASSERT(value > 0, "Can`t encode negative value!");
    if (value <= 0x7F)
    {
        *buffer = value & 0x7F;
        return 1;
    }
    else if (value <= 0x7FF)
    {
        *buffer++ = 0xC0 | ((value & 0x7C0) >> 6);
        *buffer = 0x80 | (value & 0x3F);
        return 2;
    }
    else if (value <= 0xFFFF)
    {
        *buffer++ = 0xe0 | ((value & 0xF000) >> 12);
        *buffer++ = 0x80 | ((value & 0xFC0) >> 6);
        *buffer = 0x80 | (value & 0x3F);
        return 3;
    }
    else if (value <= 0x10FFFF)
    {
        *buffer++ = 0xF0 | ((value & 0x1C0000) >> 18);
        *buffer++ = 0x80 | ((value & 0x3F000) >> 12);
        *buffer++ = 0x80 | ((value & 0xFC0) >> 6);
        *buffer = 0x80 | (value & 0x3F);
        return 4;
    }
    NOT_REACHED();
    return 0;
}
uint32_t getByteNOOfDecUTF8(uint8_t byte)
{
    if ((byte & 0xC0) == 0x80)
    {
        return 0;
    }

    if ((byte & 0xF8) == 0xF0)
    {
        return 4;
    }

    if ((byte & 0xF0) == 0xE0)
    {
        return 3;
    }

    if ((byte & 0xE0) == 0xC0)
    {
        return 2;
    }

    return 1;
}
int UTF8Decoder(const uint8_t *bytePointer, uint32_t length)
{
    if (*bytePointer <= 0x7F)
    {
        return *bytePointer;
    }

    int value;
    uint32_t remainingBytes;
    if ((*bytePointer & 0xE0) == 0xC0)
    {
        value = *bytePointer & 0x1F;
        remainingBytes = 1;
    }
    else if ((*bytePointer & 0xF0) == 0xE0)
    {
        value = *bytePointer & 0x0F;
        remainingBytes = 2;
    }
    else if ((*bytePointer & 0xF8) == 0xF0)
    {
        value = *bytePointer & 0x07;
        remainingBytes = 3;
    }
    else
    {
        return -1;
    }
    if (remainingBytes > length - 1)
    {
        return -1;
    }
    while (remainingBytes > 0)
    {
        bytePointer++;
        remainingBytes--;
        if ((*bytePointer & 0xC0) != 0x80)
        {
            return -1;
        }
        value = value << 6 | (*bytePointer & 0x3F);
    }
    return value;
}