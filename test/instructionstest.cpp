#include "instructionstest.h"
#include "disassembler.h"
#include <algorithm>
#include <numeric>

#define TEST_NZC(n, z, c)                                                                                                        \
  EXPECT_EQ(cpu.regs.p.negative, n);                                                                                              \
  EXPECT_EQ(cpu.regs.p.zero, z);                                                                                                  \
  EXPECT_EQ(cpu.regs.p.carry, c)

#define TEST_ANZC(rega, n, z, c)                                                                                                 \
  EXPECT_EQ(cpu.regs.a, uint8_t(rega));                                                                                           \
  TEST_NZC(n, z, c)

#define TEST_ANZCV(rega, n, z, c, v)                                                                                             \
  TEST_ANZC(rega, n, z, c);                                                                                                      \
  EXPECT_EQ(cpu.regs.p.overflow, v)

#define TEST_XNZC(regx, n, z, c)                                                                                                 \
  EXPECT_EQ(cpu.regs.x, uint8_t(regx));                                                                                           \
  TEST_NZC(n, z, c)

#define TEST_YNZC(regy, n, z, c)                                                                                                 \
  EXPECT_EQ(cpu.regs.y, uint8_t(regy));                                                                                           \
  TEST_NZC(n, z, c)

#define TEST_MEMNZ(addr, val, n, z)                                                                                              \
  EXPECT_EQ(memory[addr], val);                                                                                                   \
  EXPECT_EQ(cpu.regs.p.negative, n);                                                                                              \
  EXPECT_EQ(cpu.regs.p.zero, z)

#define TEST_MEMNZC(addr, val, n, z, c)                                                                                          \
  TEST_MEMNZ(addr, val, n, z);                                                                                                   \
  EXPECT_EQ(cpu.regs.p.carry, c)

#define TEST_INST(instr, numCycles)                                                                                              \
  EXPECT_EQ(assembler.processLine(instr), AssemblyResult::Ok);                                                                    \
  cpu.preRun();                                                                                                                  \
  lastCycles = cpu.stepRun();                                                                                                    \
  EXPECT_EQ(lastCycles, numCycles);                                                                                               \
  cpu.postRun();                                                                                                                 \
  EXPECT_EQ(cpu.state().execution, CpuState::Execution::Idle)

#define TEST_BRANCH_TAKEN()                                                                                                      \
  const auto base = assembler.m_locationCounter;                                                                                 \
  EXPECT_EQ(cpu.regs.pc, base + static_cast<int8_t>(*cpu.m_operandPtr.lo));                                                       \
  EXPECT_EQ(lastCycles, (base ^ cpu.regs.pc) & 0xff00 ? 4 : 3)

#define TEST_BRANCH_NOT_TAKEN()                                                                                                  \
  EXPECT_EQ(cpu.regs.pc, assembler.m_locationCounter);                                                                            \
  EXPECT_EQ(lastCycles, 2)

static constexpr auto AsmOrigin = 0x800;
static constexpr auto StackPointerOffset = 0xff;

InstructionsTest::InstructionsTest() : ::testing::Test(), assembler(memory, symbols), cpu(memory) {
}

void InstructionsTest::SetUp() {
  std::fill(memory.begin(), memory.end(), 0);
  EXPECT_TRUE(std::accumulate(memory.begin(), memory.end(), 0) == 0);
  assembler.init(AsmOrigin);
  assembler.changeMode(Assembler::ProcessingMode::EmitCode);
  cpu.reset();
  cpu.regs.pc = AsmOrigin;
  cpu.regs.sp.offset = StackPointerOffset;
  lastCycles = 0;
}

TEST_F(InstructionsTest, testIRQ) {
  memory.setWord(CpuAddress::IrqVector, 0xabcd);
  cpu.regs.p = 0b11001111;
  auto sp0 = cpu.regs.sp.address();
  auto pc0 = cpu.regs.pc;
  cpu.irq();
  EXPECT_EQ(cpu.regs.p.interrupt, true);
  EXPECT_EQ(memory[cpu.regs.sp.address() + 1], 0b11001111);
  EXPECT_EQ(memory[cpu.regs.sp.address() + 2], uint8_t(pc0));
  EXPECT_EQ(memory[cpu.regs.sp.address() + 3], uint8_t(pc0 >> 8));
  EXPECT_EQ(cpu.regs.sp.address(), sp0 - 3);
  EXPECT_EQ(cpu.regs.pc, 0xabcd);
}

TEST_F(InstructionsTest, testReset) {
  memory.setWord(CpuAddress::ResetVector, 0xFCE2);
  cpu.reset();
  EXPECT_EQ(cpu.regs.pc, 0xFCE2);
}

TEST_F(InstructionsTest, testImpliedMode) {
  cpu.regs.p.interrupt = true;
  TEST_INST("CLI", 2);
}

TEST_F(InstructionsTest, testAccumulatorMode) {
  cpu.prepImpliedOrAccumulatorMode();
  EXPECT_EQ(cpu.m_effectiveOperandPtr.lo, &cpu.regs.a);
}

TEST_F(InstructionsTest, testImmediateMode) {
  cpu.m_operandPtr.lo = &memory[0x10];
  cpu.prepImmediateMode();
  EXPECT_EQ(cpu.m_effectiveOperandPtr.lo, &memory[0x10]);
}

TEST_F(InstructionsTest, testZeroPageMode) {
  memory[0xa0] = 0x40;
  memory[0x40] = 0xf8;
  cpu.m_operandPtr.lo = &memory[0xa0];
  cpu.prepZeroPageMode();
  EXPECT_EQ(cpu.m_effectiveAddress, 0x40);
  EXPECT_EQ(*cpu.m_effectiveOperandPtr.lo, 0xf8);
}

TEST_F(InstructionsTest, testZeroPageXMode) {
  memory[0xa0] = 0x80;
  memory[0x10] = 0xe8;
  cpu.m_operandPtr.lo = &memory[0xa0];
  cpu.regs.x = 0x90;
  cpu.prepZeroPageXMode();
  EXPECT_EQ(cpu.m_effectiveAddress, 0x10);
  EXPECT_EQ(*cpu.m_effectiveOperandPtr.lo, 0xe8);
}

TEST_F(InstructionsTest, testZeroPageYMode) {
  memory[0xa0] = 0x80;
  memory[0xb1] = 0xe8;
  cpu.m_operandPtr.lo = &memory[0xa0];
  cpu.regs.y = 0x31;
  cpu.prepZeroPageYMode();
  EXPECT_EQ(cpu.m_effectiveAddress, 0xb1);
  EXPECT_EQ(*cpu.m_effectiveOperandPtr.lo, 0xe8);
}

TEST_F(InstructionsTest, testAbsoluteMode) {
  memory[0x20fa] = 0xf0;
  memory.setWord(0x1002, 0x20fa);
  cpu.m_operandPtr.lo = &memory[0x1002];
  cpu.m_operandPtr.hi = &memory[0x1003];
  cpu.prepAbsoluteMode();
  EXPECT_EQ(cpu.m_effectiveAddress, 0x20fa);
  EXPECT_EQ(*cpu.m_effectiveOperandPtr.lo, 0xf0);
}

TEST_F(InstructionsTest, testAbsoluteXMode) {
  memory[0x20b0] = 0x34;
  memory.setWord(0x1002, 0x20a0);
  cpu.m_operandPtr.lo = &memory[0x1002];
  cpu.m_operandPtr.hi = &memory[0x1003];
  cpu.regs.x = 0x10;
  cpu.prepAbsoluteXMode();
  EXPECT_EQ(cpu.m_effectiveAddress, 0x20b0);
  EXPECT_EQ(*cpu.m_effectiveOperandPtr.lo, 0x34);
}

TEST_F(InstructionsTest, testAbsoluteYMode) {
  memory[0xa020] = 0x84;
  memory.setWord(0x2002, 0xa010);
  cpu.m_operandPtr.lo = &memory[0x2002];
  cpu.m_operandPtr.hi = &memory[0x2003];
  cpu.regs.y = 0x10;
  cpu.prepAbsoluteYMode();
  EXPECT_EQ(cpu.m_effectiveAddress, 0xa020);
  EXPECT_EQ(*cpu.m_effectiveOperandPtr.lo, 0x84);
}

TEST_F(InstructionsTest, testIndirectMode) {
  memory.setWord(0xfff0, 0xfab0);
  memory.setWord(0xfab0, 0x34f0);
  cpu.m_operandPtr.lo = &memory[0xfff0];
  cpu.m_operandPtr.hi = &memory[0xfff1];
  cpu.prepIndirectMode();
  EXPECT_EQ(cpu.m_effectiveAddress, 0x34f0);
}

TEST_F(InstructionsTest, testIndexedIndirectXMode) {
  memory.setWord(0x82, 0xaf01);
  memory[0x2001] = 0x70;
  cpu.m_operandPtr.lo = &memory[0x2001];
  cpu.regs.x = 0x12;
  cpu.prepIndexedIndirectXMode();
  EXPECT_EQ(cpu.m_effectiveAddress, 0xaf01);
  EXPECT_EQ(cpu.m_effectiveOperandPtr.lo, &memory[0xaf01]);
}

TEST_F(InstructionsTest, testIndirectIndexedYMode) {
  uint8_t vector = 0x82;
  memory.setWord(vector, 0xcf81);
  memory[0xd001] = 0xea;
  cpu.m_operandPtr.lo = &vector;
  cpu.regs.y = 0x80;
  cpu.prepIndirectIndexedYMode();
  EXPECT_EQ(cpu.m_effectiveAddress, 0xd001);
  EXPECT_EQ(*cpu.m_effectiveOperandPtr.lo, 0xea);
}

TEST_F(InstructionsTest, testPageBoundaryCrossingDetection) {
  cpu.m_pageBoundaryCrossed = false;
  cpu.calculateEffectiveAddress(0x1080, 0x7f);
  EXPECT_TRUE(!cpu.m_pageBoundaryCrossed);
  cpu.calculateEffectiveAddress(0x1080, 0x80);
  EXPECT_TRUE(cpu.m_pageBoundaryCrossed);
  cpu.calculateEffectiveAddress(0x3080, 0x02);
  EXPECT_TRUE(!cpu.m_pageBoundaryCrossed);
}

TEST_F(InstructionsTest, testBranchForward) {
  SetUp();
  int8_t d = 13;
  cpu.m_operandPtr.lo = reinterpret_cast<uint8_t*>(&d);
  cpu.execBranch();
  EXPECT_EQ(cpu.regs.pc, AsmOrigin + 13);
  EXPECT_EQ(cpu.m_extraCycles, 1);
}

TEST_F(InstructionsTest, testBranchBackward) {
  int8_t d = -100;
  cpu.regs.pc += 120;
  cpu.m_operandPtr.lo = reinterpret_cast<uint8_t*>(&d);
  cpu.execBranch();
  EXPECT_EQ(cpu.regs.pc, AsmOrigin + 20);
  EXPECT_EQ(cpu.m_extraCycles, 1);
}

TEST_F(InstructionsTest, testBranchWithPageBoundaryCrossed) {
  int8_t d = 13;
  cpu.regs.pc += 250;
  cpu.m_operandPtr.lo = reinterpret_cast<uint8_t*>(&d);
  cpu.execBranch();
  EXPECT_EQ(cpu.regs.pc, AsmOrigin + 263);
  EXPECT_EQ(cpu.m_extraCycles, 2);
}

TEST_F(InstructionsTest, testWordPushPull) {
  cpu.pushWord(0x203a);
  EXPECT_EQ(cpu.pullWord(), 0x203a);
}

TEST_F(InstructionsTest, testADC) {
  auto setup = [&](bool c, uint8_t a, uint8_t op) {
    cpu.regs.p = 0;
    cpu.regs.p.carry = c;
    cpu.regs.a = a;
    memory[0x2000] = op;
  };

  setup(true, 0x7f, 0x00);
  TEST_INST("ADC $2000", 4);
  TEST_ANZC(0x80, 1, 0, 0);

  setup(true, 0xf0, 0x0f);
  TEST_INST("ADC $2000", 4);
  TEST_ANZCV(0x00, 0, 1, 1, 0);

  setup(false, 0xf0, 0x10);
  TEST_INST("ADC $2000", 4);
  TEST_ANZCV(0x00, 0, 1, 1, 0);

  setup(false, 0xf0, 0x0f);
  TEST_INST("ADC $2000", 4);
  TEST_ANZCV(0xff, 1, 0, 0, 0);

  setup(false, uint8_t(-20), uint8_t(-120));
  TEST_INST("ADC $2000", 4);
  TEST_ANZCV(0x74, 0, 0, 1, 1);

  setup(false, uint8_t(-20), uint8_t(-100));
  TEST_INST("ADC $2000", 4);
  TEST_ANZCV(-120, 1, 0, 1, 0);
}

TEST_F(InstructionsTest, testADC_decimal) {
  auto setup = [&](uint8_t a, bool c) {
    cpu.regs.p = 0;
    cpu.regs.p.decimal = true;
    cpu.regs.p.carry = c;
    cpu.regs.a = a;
  };

  setup(0x00, false);
  TEST_INST("ADC #$00", 2);
  TEST_ANZC(0x00, 0, 1, 0);

  setup(0x79, true);
  TEST_INST("ADC #$00", 2);
  TEST_ANZC(0x80, 1, 0, 0);
}

TEST_F(InstructionsTest, testSBC) {
  auto setup = [&](bool c, int a, int op) {
    cpu.regs.p = 0;
    cpu.regs.p.carry = c;
    cpu.regs.a = uint8_t(a);
    memory[0x2000] = uint8_t(op);
  };

  setup(true, 100, 80);
  TEST_INST("SBC $2000", 4);
  TEST_ANZCV(20, 0, 0, 1, 0);

  setup(true, 80, -80);
  TEST_INST("SBC $2000", 4);
  TEST_ANZCV(-96, 1, 0, 0, 1);

  setup(true, -48, 112);
  TEST_INST("SBC $2000", 4);
  TEST_ANZCV(-160, 0, 0, 1, 1);

  setup(true, 80, 112);
  TEST_INST("SBC $2000", 4);
  TEST_ANZCV(-32, 1, 0, 0, 0);

  setup(true, -121, -121);
  TEST_INST("SBC $2000", 4);
  TEST_ANZCV(0, 0, 1, 1, 0);
}

TEST_F(InstructionsTest, testAND) {
  cpu.regs.a = 0x84;
  TEST_INST("AND #$fb", 2);
  TEST_ANZCV(0x80, 1, 0, 0, 0);

  cpu.regs.a = 0x84;
  cpu.regs.y = 0x12;
  memory.setWord(0x70, 0x20f0);
  memory[0x2102] = 0xfb;
  TEST_INST("AND ($70),Y", 6);
  TEST_ANZCV(0x80, 1, 0, 0, 0);
}

TEST_F(InstructionsTest, testEOR) {
  cpu.regs.a = 0b11011110;
  memory[0x2102] = 0b01001101;
  TEST_INST("EOR $2102", 4);
  TEST_ANZCV(0b10010011, 1, 0, 0, 0);

  cpu.regs.a = 0b01001101;
  memory[0x21] = 0b01001101;
  TEST_INST("EOR $21", 3);
  TEST_ANZCV(0, 0, 1, 0, 0);
}

TEST_F(InstructionsTest, testASL) {
  cpu.regs.a = 0b11000001;
  TEST_INST("ASL", 2);
  TEST_ANZCV(0b10000010, 1, 0, 1, 0);

  memory[0xf002] = 0b01000001;
  cpu.regs.x = 0x12;
  TEST_INST("ASL $eff0,X", 7);
  TEST_ANZCV(0b10000010, 1, 0, 0, 0);
}

TEST_F(InstructionsTest, testLSR) {
  cpu.regs.a = 0b11000001;
  TEST_INST("LSR", 2);
  TEST_ANZC(0b01100000, 0, 0, 1);

  memory[0xf002] = 0b10000010;
  TEST_INST("LSR $f002", 6);
  TEST_MEMNZC(0xf002, 0b01000001, 0, 0, 0);

  memory[0xf0] = 1;
  TEST_INST("LSR $f0", 5);
  TEST_MEMNZC(0xf0, 0, 0, 1, 1);
}

TEST_F(InstructionsTest, testORA) {
  cpu.regs.a = 0b11000001;
  TEST_INST("ORA #$02", 2);
  TEST_ANZC(0xc3, 1, 0, 0);

  cpu.regs.a = 0b01000000;
  TEST_INST("ORA #$23", 2);
  TEST_ANZC(0x63, 0, 0, 0);

  cpu.regs.a = 0;
  TEST_INST("ORA #$0", 2);
  TEST_ANZC(0, 0, 1, 0);
}

TEST_F(InstructionsTest, testROL) {
  cpu.regs.p.carry = true;
  cpu.regs.a = 0b11000001;
  TEST_INST("ROL", 2);
  TEST_ANZC(0b10000011, 1, 0, 1);

  cpu.regs.p.carry = false;
  cpu.regs.a = 0b01000001;
  TEST_INST("ROL", 2);
  TEST_ANZC(0b10000010, 1, 0, 0);

  cpu.regs.p.carry = false;
  cpu.regs.a = 0b10000000;
  TEST_INST("ROL", 2);
  TEST_ANZC(0, 0, 1, 1);
}

TEST_F(InstructionsTest, testROR) {
  cpu.regs.p.carry = true;
  cpu.regs.a = 0b11000001;
  TEST_INST("ROR", 2);
  TEST_ANZC(0b11100000, 1, 0, 1);

  cpu.regs.p.carry = false;
  cpu.regs.a = 0b01000000;
  TEST_INST("ROR", 2);
  TEST_ANZC(0b00100000, 0, 0, 0);

  cpu.regs.p.carry = false;
  cpu.regs.a = 1;
  TEST_INST("ROR", 2);
  TEST_ANZC(0, 0, 1, 1);
}

TEST_F(InstructionsTest, testDEC) {
  memory[0x3102] = 0x00;
  cpu.regs.x = 0x82;
  TEST_INST("DEC $3080,X", 7);
  TEST_MEMNZ(0x3102, 0xff, 1, 0);

  memory[0x31] = 0x01;
  TEST_INST("DEC $31", 5);
  TEST_MEMNZ(0x31, 0x00, 0, 1);
}

TEST_F(InstructionsTest, testDEX) {
  cpu.regs.x = 0;
  TEST_INST("DEX", 2);
  TEST_XNZC(0xff, 1, 0, 0);

  cpu.regs.x = 1;
  TEST_INST("DEX", 2);
  TEST_XNZC(0, 0, 1, 0);
}

TEST_F(InstructionsTest, testDEY) {
  cpu.regs.y = 0;
  TEST_INST("DEY", 2);
  TEST_YNZC(0xff, 1, 0, 0);

  cpu.regs.y = 1;
  TEST_INST("DEY", 2);
  TEST_YNZC(0, 0, 1, 0);
}

TEST_F(InstructionsTest, testINC) {
  memory[0x3102] = 0xff;
  cpu.regs.x = 0x82;
  TEST_INST("INC $3080,X", 7);
  TEST_MEMNZ(0x3102, 0x0, 0, 1);

  memory[0x31] = 0x7f;
  TEST_INST("INC $31", 5);
  TEST_MEMNZ(0x31, 0x80, 1, 0);
}

TEST_F(InstructionsTest, testINX) {
  cpu.regs.x = 0xff;
  TEST_INST("INX", 2);
  TEST_XNZC(0, 0, 1, 0);

  cpu.regs.x = 0x7f;
  TEST_INST("INX", 2);
  TEST_XNZC(0x80, 1, 0, 0);
}

TEST_F(InstructionsTest, testINY) {
  cpu.regs.y = 0xff;
  TEST_INST("INY", 2);
  TEST_YNZC(0, 0, 1, 0);

  cpu.regs.y = 0x9f;
  TEST_INST("INY", 2);
  TEST_YNZC(0xa0, 1, 0, 0);
}

TEST_F(InstructionsTest, testLDA) {
  TEST_INST("LDA #$23", 2);
  TEST_ANZC(0x23, 0, 0, 0);

  memory.setWord(0xf0, 0x2080);
  cpu.regs.y = 0x92;
  memory[0x2112] = 0xf4;
  TEST_INST("LDA ($f0),Y", 6);
  TEST_ANZC(0xf4, 1, 0, 0);
}

TEST_F(InstructionsTest, testLDX) {
  TEST_INST("LDX #$23", 2);
  TEST_XNZC(0x23, 0, 0, 0);

  memory[0x2112] = 0xf4;
  cpu.regs.y = 0x13;
  TEST_INST("LDX $20ff,Y", 5);
  TEST_XNZC(0xf4, 1, 0, 0);
}

TEST_F(InstructionsTest, testLDY) {
  TEST_INST("LDY #$23", 2);
  TEST_YNZC(0x23, 0, 0, 0);

  memory[0xeaf0] = 0xf4;
  cpu.regs.x = 0xf0;
  TEST_INST("LDY $EA00,X", 4);
  TEST_YNZC(0xf4, 1, 0, 0);
}

TEST_F(InstructionsTest, testSTA) {
  cpu.regs.a = 0x8a;
  TEST_INST("STA $2300", 4);
  TEST_MEMNZ(0x2300, 0x8a, 0, 0);

  cpu.regs.a = 0x00;
  TEST_INST("STA $2300", 4);
  TEST_MEMNZ(0x2300, 0x00, 0, 0);
}

TEST_F(InstructionsTest, testSTX) {
  cpu.regs.x = 0x8a;
  TEST_INST("STX $3300", 4);
  TEST_MEMNZ(0x3300, 0x8a, 0, 0);

  cpu.regs.x = 0x00;
  TEST_INST("STX $3300", 4);
  TEST_MEMNZ(0x3300, 0x00, 0, 0);
}

TEST_F(InstructionsTest, testSTY) {
  cpu.regs.y = 0x8a;
  TEST_INST("STY $3300", 4);
  TEST_MEMNZ(0x3300, 0x8a, 0, 0);

  cpu.regs.y = 0x00;
  TEST_INST("STY $3300", 4);
  TEST_MEMNZ(0x3300, 0x00, 0, 0);
}

TEST_F(InstructionsTest, testTXA) {
  cpu.regs.x = 0xcf;
  TEST_INST("TXA", 2);
  TEST_ANZC(0xcf, 1, 0, 0);

  cpu.regs.x = 0;
  TEST_INST("TXA", 2);
  TEST_ANZC(0, 0, 1, 0);
}

TEST_F(InstructionsTest, testTAX) {
  cpu.regs.a = 0xcf;
  TEST_INST("TAX", 2);
  TEST_XNZC(0xcf, 1, 0, 0);

  cpu.regs.a = 0;
  TEST_INST("TAX", 2);
  TEST_XNZC(0, 0, 1, 0);
}

TEST_F(InstructionsTest, testTYA) {
  cpu.regs.y = 0xcf;
  TEST_INST("TYA", 2);
  TEST_ANZC(0xcf, 1, 0, 0);

  cpu.regs.y = 0;
  TEST_INST("TYA", 2);
  TEST_ANZC(0, 0, 1, 0);
}

TEST_F(InstructionsTest, testTAY) {
  cpu.regs.a = 0xcf;
  TEST_INST("TAY", 2);
  TEST_YNZC(0xcf, 1, 0, 0);

  cpu.regs.a = 0;
  TEST_INST("TAY", 2);
  TEST_YNZC(0, 0, 1, 0);
}

TEST_F(InstructionsTest, testTSX) {
  cpu.regs.sp.offset = 0x8f;
  TEST_INST("TSX", 2);
  TEST_XNZC(0x8f, 1, 0, 0);

  cpu.regs.sp.offset = 0x00;
  TEST_INST("TSX", 2);
  TEST_XNZC(0, 0, 1, 0);
}

TEST_F(InstructionsTest, testTXS) {
  cpu.regs.x = 0x81;
  TEST_INST("TXS", 2);
  EXPECT_EQ(cpu.regs.sp.offset, 0x81);
  EXPECT_EQ(cpu.regs.p.negative, false);
  EXPECT_EQ(cpu.regs.p.zero, false);

  cpu.regs.x = 0;
  TEST_INST("TXS", 2);
  EXPECT_EQ(cpu.regs.sp.offset, 0);
  EXPECT_EQ(cpu.regs.p.negative, false);
  EXPECT_EQ(cpu.regs.p.zero, false);
}

TEST_F(InstructionsTest, testBCC_taken) {
  cpu.regs.p.carry = false;
  TEST_INST("BCC +3", 3);
  TEST_BRANCH_TAKEN();
}

TEST_F(InstructionsTest, testBCC_notTaken) {
  cpu.regs.p.carry = true;
  TEST_INST("BCC +13", 2);
  TEST_BRANCH_NOT_TAKEN();
}

TEST_F(InstructionsTest, testBCS_taken) {
  cpu.regs.p.carry = true;
  TEST_INST("BCS +33", 3);
  TEST_BRANCH_TAKEN();
}

TEST_F(InstructionsTest, testBCS_notTaken) {
  cpu.regs.p.carry = false;
  TEST_INST("BCS +13", 2);
  TEST_BRANCH_NOT_TAKEN();
}

TEST_F(InstructionsTest, testBEQ_taken) {
  cpu.regs.p.zero = true;
  TEST_INST("BEQ +103", 3);
  TEST_BRANCH_TAKEN();
}

TEST_F(InstructionsTest, testBEQ_notTaken) {
  cpu.regs.p.zero = false;
  TEST_INST("BEQ -2", 2);
  TEST_BRANCH_NOT_TAKEN();
}

TEST_F(InstructionsTest, testBNE_taken) {
  cpu.regs.p.zero = false;
  TEST_INST("BNE +43", 3);
  TEST_BRANCH_TAKEN();
}

TEST_F(InstructionsTest, testBNE_notTaken) {
  cpu.regs.p.zero = true;
  TEST_INST("BNE -27", 2);
  TEST_BRANCH_NOT_TAKEN();
}

TEST_F(InstructionsTest, testBMI_taken) {
  cpu.regs.p.negative = true;

  // increase PC and LC to avoid page crossing

  cpu.regs.pc += 10;
  assembler.m_locationCounter += 10;
  TEST_INST("BMI -3", 3);
  TEST_BRANCH_TAKEN();
}

TEST_F(InstructionsTest, testBMI_notTaken) {
  cpu.regs.p.negative = false;
  TEST_INST("BMI -42", 2);
  TEST_BRANCH_NOT_TAKEN();
}

TEST_F(InstructionsTest, testBPL_taken) {
  cpu.regs.p.negative = false;
  TEST_INST("BPL +2", 3);
  TEST_BRANCH_TAKEN();
}

TEST_F(InstructionsTest, testBPL_notTaken) {
  cpu.regs.p.negative = true;
  TEST_INST("BPL -82", 2);
  TEST_BRANCH_NOT_TAKEN();
}

TEST_F(InstructionsTest, testBVC_taken) {
  cpu.regs.p.overflow = false;
  TEST_INST("BVC +29", 3);
  TEST_BRANCH_TAKEN();
}

TEST_F(InstructionsTest, testBVC_notTaken) {
  cpu.regs.p.overflow = true;
  TEST_INST("BVC +29", 2);
  TEST_BRANCH_NOT_TAKEN();
}

TEST_F(InstructionsTest, testBVS_taken) {
  cpu.regs.p.overflow = true;
  TEST_INST("BVS +29", 3);
  TEST_BRANCH_TAKEN();
}

TEST_F(InstructionsTest, testBVS_notTaken) {
  cpu.regs.p.overflow = false;
  TEST_INST("BVS +29", 2);
  TEST_BRANCH_NOT_TAKEN();
}

TEST_F(InstructionsTest, testJMP_absolute) {
  assembler.m_symbols.put("c", 0x4af0);
  TEST_INST("  jmp c", 3);
  EXPECT_EQ(cpu.regs.pc, 0x4af0);
}

TEST_F(InstructionsTest, testJMP_indirect) {
  memory.setWord(0xa000, 0x1f80);
  TEST_INST("JMP ($a000)", 5);
  EXPECT_EQ(cpu.regs.pc, 0x1f80);
}

TEST_F(InstructionsTest, testPHA) {
  const auto addr = cpu.regs.sp.address();
  cpu.regs.a = 0x1f;
  TEST_INST("PHA", 3);
  EXPECT_EQ(cpu.regs.sp.address(), addr - 1);
  EXPECT_EQ(memory[addr], 0x1f);
}

TEST_F(InstructionsTest, testPLA) {
  cpu.regs.sp.offset = 0x80;
  memory[0x181] = 0xab;
  TEST_INST("PLA", 4);
  TEST_ANZC(0xab, 1, 0, 0);
  EXPECT_EQ(cpu.regs.sp.address(), 0x181);
}

TEST_F(InstructionsTest, testPHP) {
  const auto addr = cpu.regs.sp.address();
  memory[addr] = 0;
  cpu.regs.p = 0b11001100;
  TEST_INST("PHP", 3);
  EXPECT_EQ(cpu.regs.sp.address(), addr - 1);
  EXPECT_EQ(memory[addr], 0b11001100);
}

TEST_F(InstructionsTest, testPLP) {
  cpu.regs.sp.offset = 0x80;
  memory[0x181] = 0b00001100;
  TEST_INST("PLP", 4);
  EXPECT_EQ(cpu.regs.p, 0b00001100);
  EXPECT_EQ(cpu.regs.sp.address(), 0x181);
}

TEST_F(InstructionsTest, testJSR) {
  auto sp0 = cpu.regs.sp.address();
  auto pc0 = cpu.regs.pc + 2;
  TEST_INST("JSR $f000", 6);
  EXPECT_EQ(memory[cpu.regs.sp.address() + 1], uint8_t(pc0));
  EXPECT_EQ(memory[cpu.regs.sp.address() + 2], uint8_t(pc0 >> 8));
  EXPECT_EQ(cpu.regs.sp.address(), sp0 - 2);
}

TEST_F(InstructionsTest, testRTS) {
  cpu.pushWord(0x8002);
  TEST_INST("RTS", 6);
  EXPECT_EQ(cpu.regs.pc, 0x8003);
}

TEST_F(InstructionsTest, testBRK) {
  memory.setWord(CpuAddress::IrqVector, 0xabcd);
  cpu.regs.p = 0b11001111;
  auto sp0 = cpu.regs.sp.address();
  auto pc0 = cpu.regs.pc + 2;
  TEST_INST("BRK", 7);
  EXPECT_EQ(cpu.regs.p.interrupt, true);
  EXPECT_EQ(memory[cpu.regs.sp.address() + 1], 0b11011111);
  EXPECT_EQ(memory[cpu.regs.sp.address() + 2], uint8_t(pc0));
  EXPECT_EQ(memory[cpu.regs.sp.address() + 3], uint8_t(pc0 >> 8));
  EXPECT_EQ(cpu.regs.sp.address(), sp0 - 3);
  EXPECT_EQ(cpu.regs.pc, 0xabcd);
}

TEST_F(InstructionsTest, testRTI) {
  cpu.regs.p.interrupt = true;
  cpu.pushWord(0x8003);
  cpu.push(0b11010011);
  TEST_INST("RTI", 6);
  EXPECT_EQ(cpu.regs.p.interrupt, false);
  EXPECT_EQ(cpu.regs.pc, 0x8003);
  EXPECT_EQ(uint8_t(cpu.regs.p), 0b11000011);
}

TEST_F(InstructionsTest, testNOP) {
  TEST_INST("NOP", 2);
}

TEST_F(InstructionsTest, testBIT) {
  cpu.regs.a = 0x81;
  memory[0x2001] = 0x41;
  TEST_INST("BIT $2001", 4);
  TEST_ANZCV(0x81, 0, 0, 0, 1);

  cpu.regs.a = 0x41;
  memory[0x20] = 0x02;
  TEST_INST("BIT $20", 3);
  TEST_ANZCV(0x41, 0, 1, 0, 0);
}

TEST_F(InstructionsTest, testCMP) {
  cpu.regs.a = 0x81;
  TEST_INST("CMP #$80", 2);
  TEST_ANZCV(0x81, 0, 0, 1, 0);

  cpu.regs.a = 0x71;
  TEST_INST("CMP #$90", 2);
  TEST_ANZCV(0x71, 1, 0, 0, 0);

  cpu.regs.a = 0x01;
  TEST_INST("CMP #$01", 2);
  TEST_ANZCV(0x01, 0, 1, 1, 0);

  cpu.regs.a = uint8_t(-100);
  memory[0x2000] = uint8_t(-110);
  TEST_INST("CMP $2000", 4);
  TEST_ANZCV(-100, 0, 0, 1, 0);

  cpu.regs.a = 150;
  memory[0x2000] = 120;
  TEST_INST("CMP $2000", 4);
  TEST_ANZCV(150, 0, 0, 1, 0);
}

TEST_F(InstructionsTest, testCPX) {
  cpu.regs.x = uint8_t(-100);
  memory[0x2000] = uint8_t(-110);
  TEST_INST("CPX $2000", 4);
  TEST_XNZC(-100, 0, 0, 1);

  cpu.regs.x = 150;
  memory[0x20] = 120;
  TEST_INST("CPX $20", 3);
  TEST_XNZC(150, 0, 0, 1);
}

TEST_F(InstructionsTest, testCPY) {
  cpu.regs.y = 0x71;
  TEST_INST("CPY #$90", 2);
  TEST_YNZC(0x71, 1, 0, 0);

  cpu.regs.y = uint8_t(-100);
  memory[0x2000] = uint8_t(-110);
  TEST_INST("CPY $2000", 4);
  TEST_YNZC(-100, 0, 0, 1);
}

TEST_F(InstructionsTest, testCLC) {
  cpu.regs.p.carry = true;
  TEST_INST("CLC", 2);
  EXPECT_EQ(cpu.regs.p.carry, false);
}

TEST_F(InstructionsTest, testSEC) {
  cpu.regs.p.carry = false;
  TEST_INST("SEC", 2);
  EXPECT_EQ(cpu.regs.p.carry, true);
}

TEST_F(InstructionsTest, testCLD) {
  cpu.regs.p.decimal = true;
  TEST_INST("CLD", 2);
  EXPECT_EQ(cpu.regs.p.decimal, false);
}

TEST_F(InstructionsTest, testSED) {
  cpu.regs.p.decimal = false;
  TEST_INST("SED", 2);
  EXPECT_EQ(cpu.regs.p.decimal, true);
}

TEST_F(InstructionsTest, testCLI) {
  cpu.regs.p.interrupt = true;
  TEST_INST("CLI", 2);
  EXPECT_EQ(cpu.regs.p.interrupt, false);
}

TEST_F(InstructionsTest, testSEI) {
  cpu.regs.p.interrupt = false;
  TEST_INST("SEI", 2);
  EXPECT_EQ(cpu.regs.p.interrupt, true);
}
