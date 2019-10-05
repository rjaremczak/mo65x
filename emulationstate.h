#pragma once

#include <stdint.h>
#include <chrono>

struct EmulationState
{
    std::chrono::nanoseconds clockPeriod{1000};
};
