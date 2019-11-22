#pragma once

enum class CpuRunLevel { Normal = 0, PendingIrq, PendingNmi, PendingReset };

const char* formatRunLevel(CpuRunLevel runLevel);
