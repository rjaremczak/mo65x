#include "executionstate.h"

const char* formatExecutionState(CpuState es) {
  switch (es) {
  case CpuState::Idle: return "idle";
  case CpuState::Stopped: return "stopped";
  case CpuState::Halted: return "halted";
  case CpuState::Running: return "running";
  case CpuState::Stopping: return "stopping";
  case CpuState::Halting: return "halting";
  }
  return nullptr;
}
