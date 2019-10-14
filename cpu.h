#pragma once

#include <functional>
#include <chrono>
#include "cpuregisters.h"
#include "memory.h"

class Cpu
{
    Memory& m_memory;

    CpuRegisters m_registers;

public:
    Cpu(Memory&);
    CpuRegisters registers() const { return m_registers; }
};
