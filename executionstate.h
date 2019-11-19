#pragma once

enum class ExecutionState { Idle, Stopped, Halted, Running, Stopping, Halting };

const char* executionStateStr(ExecutionState es);
