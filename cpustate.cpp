#include "cpustate.h"

const char* CpuState::format(CpuState::Mode mode) {
  switch (mode) {
  case Mode::PendingReset: return "pending reset";
  case Mode::PendingNmi: return "pending nmi";
  case Mode::PendingIrq: return "pending irq";
  case Mode::Normal: return "normal";
  }
  return nullptr;
}

const char* CpuState::format(CpuState::Execution execution) {
  switch (execution) {
  case Execution::Idle: return "idle";
  case Execution::Stopped: return "stopped";
  case Execution::Halted: return "halted";
  case Execution::Running: return "running";
  case Execution::Stopping: return "stopping";
  }
  return nullptr;
}
