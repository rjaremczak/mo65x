#pragma once

#include <cstdint>

struct CpuRegisters {
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sp;
    uint16_t pc;
};
