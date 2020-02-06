#pragma once

#include "commondefs.h"
#include "cpuexecutionstate.h"
#include "cpustatistics.h"
#include "registers.h"
#include "runlevel.h"
#include <QMetaType>
#include <chrono>

struct EmulatorState {
  CpuExecutionState state;
  CpuRunLevel runLevel;
  Registers regs;
  CpuStatistics avgExecutionStatistics;
  CpuStatistics lastExecutionStatistics;

  bool running() const { return state == CpuExecutionState::Running; }
};
