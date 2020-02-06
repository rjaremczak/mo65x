#pragma once

#include "cpuexecutionstate.h"
#include "runlevel.h"

struct CpuInfo {
  CpuRunLevel runLevel;
  CpuExecutionState executionState;
};
