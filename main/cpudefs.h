#pragma once

#include "commondefs.h"

enum CpuAddress : Address {
  IoPortConfig = 0x00,
  IoPortData = 0x01,
  StackPointerBase = 0x100,
  NmiVector = 0xfffa,
  ResetVector = 0xfffc,
  IrqVector = 0xfffe
};
