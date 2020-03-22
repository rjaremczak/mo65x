#pragma once

#include <gtest/gtest.h>

#define private public
#include "assembler.h"
#undef private

static constexpr auto AsmOrigin = 0x0800;

class AssemblerTest : public ::testing::Test {
protected:
  AssemblerTest();
  SymbolTable symbols;
  Assembler assembler;
  Memory memory;
};
