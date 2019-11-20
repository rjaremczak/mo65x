#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <vector>

using Data = std::vector<uint8_t>;
using FileOperationCallBack = std::function<void(uint64_t)>;
using Clock = std::chrono::high_resolution_clock;
using Duration = std::chrono::microseconds;
