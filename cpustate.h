#pragma once

#include <cstdint>

enum class CpuState : uint8_t { Idle, Stopped, Halted, Running, Stopping, Halting };

const char* formatCpuState(CpuState);
