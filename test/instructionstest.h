#pragma once

#include <gtest/gtest.h>

#define private public
#include "assembler.h"
#include "cpu.h"
#undef private

class InstructionsTest : public ::testing::Test {
protected:
  InstructionsTest();
  SymbolTable symbols;
  Assembler assembler;
  Memory memory;
  Cpu cpu;
  int lastCycles;

  void SetUp() override;
};
