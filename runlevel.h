#pragma once

#include <cstdint>

enum class CpuRunLevel : uint8_t { Normal = 0, PendingIrq, PendingNmi, PendingReset };

const char* formatRunLevel(CpuRunLevel runLevel);
