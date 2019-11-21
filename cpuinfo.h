#pragma once

#include "cpustate.h"
#include "runlevel.h"

struct CpuInfo {
  CpuRunLevel runLevel;
  CpuState executionState;
};
