#pragma once

#include "executionstate.h"
#include "registers.h"
#include <QMetaType>

struct CpuInfo {
  ExecutionState state;
  Registers regs;
  int cycles;
};
