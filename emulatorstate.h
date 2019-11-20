#pragma once

#include "defs.h"
#include "executionstate.h"
#include "executionstatistics.h"
#include "registers.h"
#include "runlevel.h"
#include <QMetaType>
#include <chrono>

struct EmulatorState {
  CpuState executionState;
  CpuRunLevel runLevel;
  Registers registers;
  ExecutionStatistics avgExecutionStatistics;
  ExecutionStatistics lastExecutionStatistics;
};
