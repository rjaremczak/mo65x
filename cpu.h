#pragma once

#include <functional>
#include <chrono>
#include "cpuregisters.h"

class Emulator;

class Cpu
{
public:
    using BusRead = uint8_t(Emulator::*)(uint16_t);
    using BusWrite = void(Emulator::*)(uint16_t, uint8_t);

private:
    CpuRegisters m_registers;
    const BusRead m_busRead;
    const BusWrite m_busWrite;

public:
    Cpu(const BusRead busRead, const BusWrite busWrite);

    inline CpuRegisters registers() const { return m_registers; }
};
