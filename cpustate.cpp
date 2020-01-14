#include "cpustate.h"

const char* formatCpuState(CpuState state) {
  switch (state) {
  case CpuState::Idle: return "idle";
  case CpuState::Stopped: return "stopped";
  case CpuState::Halted: return "halted";
  case CpuState::Running: return "running";
  case CpuState::Stopping: return "stopping";
  }
  return nullptr;
}
