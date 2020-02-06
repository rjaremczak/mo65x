#include "cpuexecutionstate.h"

const char* formatCpuState(CpuExecutionState state) {
  switch (state) {
  case CpuExecutionState::Idle: return "idle";
  case CpuExecutionState::Stopped: return "stopped";
  case CpuExecutionState::Halted: return "halted";
  case CpuExecutionState::Running: return "running";
  case CpuExecutionState::Stopping: return "stopping";
  }
  return nullptr;
}
