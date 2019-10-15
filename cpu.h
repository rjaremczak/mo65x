#pragma once

#include <functional>
#include <chrono>
#include "cpuregisters.h"
#include "memory.h"
#include "opcodes.h"

class Cpu
{
    Memory& m_memory;

    CpuRegisters m_registers;
    Instruction m_instruction;

public:
    Cpu(Memory&);
    CpuRegisters registers() const { return m_registers; }
};
