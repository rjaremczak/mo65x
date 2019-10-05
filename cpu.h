#pragma once

#include "cpuregisters.h"

class Cpu
{
private:
    CpuRegisters m_registers;

public:
    Cpu();

    inline CpuRegisters registers() const { return m_registers; }
};
