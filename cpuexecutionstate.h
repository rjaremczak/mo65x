#pragma once

#include <cstdint>

enum class CpuExecutionState : uint8_t { Idle, Stopped, Halted, Running, Stopping };

const char* formatCpuState(CpuExecutionState);
