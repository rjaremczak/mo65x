#pragma once

enum class CpuRunLevel { Program = 0, Irq, Nmi, Reset, SingleStep };

const char* formatRunLevel(CpuRunLevel runLevel);
