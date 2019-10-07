#pragma once

#include <cstdint>
#include <chrono>

struct EmulatorState
{
    std::chrono::nanoseconds clockPeriod{1000};
};
