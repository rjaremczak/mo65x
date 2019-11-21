#pragma once

#include "commondefs.h"
#include "cpustate.h"
#include "executionstatistics.h"
#include "registers.h"
#include "runlevel.h"
#include <QMetaType>
#include <chrono>

struct EmulatorState {
  CpuState state;
  CpuRunLevel runLevel;
  Registers regs;
  ExecutionStatistics avgExecutionStatistics;
  ExecutionStatistics lastExecutionStatistics;

  bool running() const { return runLevel != CpuRunLevel::SingleStep && state == CpuState::Running; }
};
