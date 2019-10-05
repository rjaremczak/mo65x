#pragma once

#include <stdint.h>

struct CpuRegisters
{
    uint8_t A;
    uint8_t X;
    uint8_t Y;
    uint8_t SP;
    uint16_t PC;

    bool flagN;
    bool flagV;
    bool flagB;
    bool flagD;
    bool flagI;
    bool flagZ;
    bool flagC;
};
