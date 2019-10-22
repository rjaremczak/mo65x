#pragma once

#include "cpu.h"

class CpuProbe
{
    Cpu& m_cpu;

public:
    CpuProbe(Cpu&);
    CpuRegisters& registers() { return m_cpu.m_regs; }
    CpuFlags& flags() { return m_cpu.m_flags; }
};
