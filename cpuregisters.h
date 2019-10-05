#pragma once

#include <stdint.h>

struct CpuRegisters
{
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sp;
    uint16_t pc;

    struct {
        bool n, v, b, d, i , z, c;
    } flags;
};
