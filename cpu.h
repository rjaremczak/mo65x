#pragma once

#include <functional>
#include <chrono>
#include "cpustate.h"
#include "memory.h"
#include "opcodes.h"

class Cpu
{
    Memory& m_memory;

    CpuState m_state;
    Instruction m_instruction;

public:
    Cpu(Memory&);
    CpuState state() const { return m_state; }
    uint16_t pc() const { return m_state.pc; }
    void setPC(uint16_t pc) { m_state.pc = pc; }
};
