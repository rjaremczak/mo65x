#pragma once

#include "executionstate.h"
#include "registers.h"

struct CpuInfo {
  ExecutionState state;
  Registers regs;
  int cycles;
};
