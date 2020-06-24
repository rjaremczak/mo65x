#pragma once

#include "commondefs.h"
#include "cpustate.h"
#include "cpustatistics.h"
#include "registers.h"
#include <chrono>

struct EmulatorState {
  CpuState cpuState;
  Registers regs;
  CpuStatistics accExecutionStatistics;
  CpuStatistics lastExecutionStatistics;
};
