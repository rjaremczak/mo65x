#pragma once

#include "defs.h"
#include "executionstate.h"
#include "registers.h"
#include <QMetaType>
#include <chrono>

struct EmulatorState {
  ExecutionState state;
  Registers regs;
  uint64_t totalCycles;
  Duration totalDuration;
  uint64_t lastCycles = 0;
  Duration lastDuration = Duration::zero();
};
