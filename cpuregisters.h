#pragma once

#include <cstdint>

struct CpuRegisters {
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint16_t sp;
    uint16_t pc;

    void spFromByte(uint8_t v) { sp = v | 0x100; }
    uint8_t spAsByte() const { return sp & 0xff; }
    void incSP() { sp = ++sp & 0xff; }
};
