#include "executionstate.h"

const char* executionStateStr(ExecutionState es) {
  switch (es) {
  case ExecutionState::Idle: return "idle";
  case ExecutionState::Stopped: return "stopped";
  case ExecutionState::Halted: return "halted";
  case ExecutionState::Running: return "running";
  case ExecutionState::Stopping: return "stopping";
  case ExecutionState::Halting: return "halting";
  }
  return nullptr;
}
