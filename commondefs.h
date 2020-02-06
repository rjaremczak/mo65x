#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <vector>

using Address = uint16_t;
using Data = std::vector<uint8_t>;
using FileOperationCallBack = std::function<void(uint64_t)>;
using Frequency = uint32_t;
