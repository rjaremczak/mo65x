#pragma once

#include <stdint.h>
#include <chrono>

struct EmulatorState
{
    std::chrono::nanoseconds clockPeriod{1000};
};
