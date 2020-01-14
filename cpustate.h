#pragma once

#include <cstdint>

enum class CpuState : uint8_t { Idle, Stopped, Halted, Running, Stopping };

const char* formatCpuState(CpuState);
