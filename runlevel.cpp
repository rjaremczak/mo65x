#include "runlevel.h"

const char* formatRunLevel(CpuRunLevel runLevel) {
  switch (runLevel) {
  case CpuRunLevel::SingleStep: return "single step";
  case CpuRunLevel::Reset: return "reset";
  case CpuRunLevel::Nmi: return "nmi";
  case CpuRunLevel::Irq: return "irq";
  case CpuRunLevel::Program: return "program";
  }
  return nullptr;
}
