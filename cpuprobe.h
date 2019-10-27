#pragma once

#include "cpu.h"

class CpuProbe
{
    Cpu& cpu_;

public:
    CpuProbe(Cpu&);
    CpuRegisters& registers() { return cpu_.registers_; }
    CpuFlags& flags() { return cpu_.flags_; }
};
