#pragma once

#include "assembler.h"
#include <gtest/gtest.h>

static constexpr auto AsmOrigin = 0x0800;

class AssemblerTest : public ::testing::Test {
protected:
  AssemblerTest();
  SymbolTable symbols;
  Assembler assembler;
  Memory memory;
};
