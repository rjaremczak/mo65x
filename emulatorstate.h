#pragma once

#include "defs.h"
#include "executionstate.h"
#include "executionstatistics.h"
#include "registers.h"
#include <QMetaType>
#include <chrono>

struct EmulatorState {
  ExecutionState executionState;
  Registers registers;
  ExecutionStatistics avgExecutionStatistics;
  ExecutionStatistics lastExecutionStatistics;
};
