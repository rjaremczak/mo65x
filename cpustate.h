#pragma once

#include <cstdint>

struct CpuState {
  enum class Mode : uint8_t { Normal = 0, PendingIrq, PendingNmi, PendingReset };
  enum class Execution : uint8_t { Idle, Stopped, Halted, Running, Stopping };

  Mode mode = Mode::Normal;
  Execution execution = Execution::Idle;

  bool running() const { return execution == Execution::Running; }

  static const char* format(Mode);
  static const char* format(Execution);
};
