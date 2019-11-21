#pragma once

enum class CpuRunLevel { Normal = 0, Irq, Nmi, Reset, SingleStep };

const char* formatRunLevel(CpuRunLevel runLevel);
