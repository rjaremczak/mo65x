#include "runlevel.h"

const char* formatRunLevel(CpuRunLevel runLevel) {
  switch (runLevel) {
  case CpuRunLevel::PendingReset: return "pending reset";
  case CpuRunLevel::PendingNmi: return "pending nmi";
  case CpuRunLevel::PendingIrq: return "pending irq";
  case CpuRunLevel::Normal: return "normal";
  }
  return nullptr;
}
