#pragma once

#include "cpu.h"

class CpuProbe
{
    Cpu& m_cpu;

public:
    CpuProbe(Cpu&);
    CpuRegisters& registers() { return m_cpu.registers_; }
    CpuFlags& flags() { return m_cpu.flags_; }
};
