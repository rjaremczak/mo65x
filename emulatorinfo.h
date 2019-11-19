#pragma once

#include "executionstate.h"
#include "registers.h"
#include <QMetaType>
#include <chrono>

struct EmulatorInfo {
  ExecutionState state;
  Registers regs;
  uint64_t totalCycles;
  std::chrono::microseconds duration;
};
