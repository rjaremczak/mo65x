#pragma once

#include <stdint.h>

struct CpuState
{
    uint8_t regA;
    uint8_t regX;
    uint8_t regY;
    uint8_t regSP;
    uint16_t regPC;

    bool flagN;
    bool flagV;
    bool flagB;
    bool flagD;
    bool flagI;
    bool flagZ;
    bool flagC;
};
