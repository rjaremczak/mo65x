#pragma once

#include "cpustate.h"
#include "executionstatistics.h"
#include "runlevel.h"

struct CpuInfo {
  CpuRunLevel runLevel;
  CpuState state;
  ExecutionStatistics executionStatistics;
};
