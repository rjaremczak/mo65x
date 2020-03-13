#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <vector>

using Address = uint16_t;
using Data = std::vector<uint8_t>;
using FileOperationCallBack = std::function<void(uint64_t)>;
using PreciseClock = std::chrono::steady_clock; // as high_resolution_clock with MinGW is less accurate !!!
using Duration = std::chrono::nanoseconds;
using Frequency = uint32_t;
