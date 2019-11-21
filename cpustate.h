#pragma once

enum class CpuState { Idle, Stopped, Halted, Running, Stopping, Halting };

const char* formatCpuState(CpuState);
