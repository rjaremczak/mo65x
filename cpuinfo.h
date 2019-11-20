#pragma once

#include "executionstate.h"
#include "runlevel.h"

struct CpuInfo {
  CpuRunLevel runLevel;
  CpuState executionState;
};
