#include "instructionstest.h"
#include "disassembler.h"
#include <QTest>
#include <algorithm>

#define TEST_ANZCV(acc, n, z, c, v)                                                                                              \
  QCOMPARE(cpu.regs.a, uint8_t(acc));                                                                                            \
  QCOMPARE(cpu.regs.p.negative, n);                                                                                              \
  QCOMPARE(cpu.regs.p.zero, z);                                                                                                  \
  QCOMPARE(cpu.regs.p.carry, c);                                                                                                 \
  QCOMPARE(cpu.regs.p.overflow, v)

#define TEST_INST(instr, numCycles)                                                                                              \
  cpu.cycles = 0;                                                                                                                \
  QVERIFY(assembler.assemble(instr));                                                                                            \
  cpu.execute();                                                                                                                 \
  QCOMPARE(cpu.executionStatus, Stopped);                                                                                        \
  QCOMPARE(cpu.cycles, numCycles)

#define TEST_BRANCH_TAKEN()                                                                                                      \
  const auto base = assembler.address();                                                                                         \
  QCOMPARE(cpu.regs.pc, base + *cpu.operandPtr_);                                                                                \
  QCOMPARE(cpu.cycles, (base ^ cpu.regs.pc) & 0xff00 ? 4 : 3)

#define TEST_BRANCH_NOT_TAKEN()                                                                                                  \
  QCOMPARE(cpu.regs.pc, assembler.address());                                                                                    \
  QCOMPARE(cpu.cycles, 2)

static constexpr auto AsmOrigin = 0x800;

OpCodesTest::OpCodesTest(QObject* parent) : QObject(parent), cpu(memory), assembler(memory) {
}

void OpCodesTest::initTestCase() {
  QVERIFY(&cpu.memory_ == &memory);
  std::fill(memory.begin(), memory.end(), 0);
  QVERIFY(std::accumulate(memory.begin(), memory.end(), 0) == 0);
}

void OpCodesTest::init() {
  assembler.setOrigin(AsmOrigin);
  cpu.regs.pc = AsmOrigin;
  cpu.regs.p = 0;
  cpu.cycles = 0;
}

void OpCodesTest::testReset() {
  memory.write16(Cpu::VectorRESET, 0xFCE2);
  cpu.reset();
  QCOMPARE(cpu.regs.pc, 0xFCE2);
}

void OpCodesTest::testImpliedMode() {
  cpu.regs.p.interrupt = true;
  TEST_INST("CLI", 2);
  QVERIFY(!cpu.regs.p.interrupt);
}

void OpCodesTest::testAccumulatorMode() {
  cpu.prepAccumulatorMode();
  QCOMPARE(cpu.effectiveOperandPtr_, &cpu.regs.a);
}

void OpCodesTest::testImmediateMode() {
  cpu.operandPtr_ = &memory[0x10];
  cpu.prepImmediateMode();
  QCOMPARE(cpu.effectiveOperandPtr_, &memory[0x10]);
}

void OpCodesTest::testZeroPageMode() {
  memory[0xa0] = 0x40;
  memory[0x40] = 0xf8;
  cpu.operandPtr_ = &memory[0xa0];
  cpu.prepZeroPageMode();
  QCOMPARE(cpu.effectiveAddress_, 0x40);
  QCOMPARE(*cpu.effectiveOperandPtr_, 0xf8);
}

void OpCodesTest::testZeroPageXMode() {
  memory[0xa0] = 0x80;
  memory[0x10] = 0xe8;
  cpu.operandPtr_ = &memory[0xa0];
  cpu.regs.x = 0x90;
  cpu.prepZeroPageXMode();
  QCOMPARE(cpu.effectiveAddress_, 0x10);
  QCOMPARE(*cpu.effectiveOperandPtr_, 0xe8);
}

void OpCodesTest::testZeroPageYMode() {
  memory[0xa0] = 0x80;
  memory[0xb1] = 0xe8;
  cpu.operandPtr_ = &memory[0xa0];
  cpu.regs.y = 0x31;
  cpu.prepZeroPageYMode();
  QCOMPARE(cpu.effectiveAddress_, 0xb1);
  QCOMPARE(*cpu.effectiveOperandPtr_, 0xe8);
}

void OpCodesTest::testAbsoluteMode() {
  memory[0x20fa] = 0xf0;
  memory.write16(0x1002, 0x20fa);
  cpu.operandPtr_ = &memory[0x1002];
  cpu.prepAbsoluteMode();
  QCOMPARE(cpu.effectiveAddress_, 0x20fa);
  QCOMPARE(*cpu.effectiveOperandPtr_, 0xf0);
}

void OpCodesTest::testAbsoluteXMode() {
  memory[0x20b0] = 0x34;
  memory.write16(0x1002, 0x20a0);
  cpu.operandPtr_ = &memory[0x1002];
  cpu.regs.x = 0x10;
  cpu.prepAbsoluteXMode();
  QCOMPARE(cpu.effectiveAddress_, 0x20b0);
  QCOMPARE(*cpu.effectiveOperandPtr_, 0x34);
}

void OpCodesTest::testAbsoluteYMode() {
  memory[0xa020] = 0x84;
  memory.write16(0x2002, 0xa010);
  cpu.operandPtr_ = &memory[0x2002];
  cpu.regs.y = 0x10;
  cpu.prepAbsoluteYMode();
  QCOMPARE(cpu.effectiveAddress_, 0xa020);
  QCOMPARE(*cpu.effectiveOperandPtr_, 0x84);
}

void OpCodesTest::testIndirectMode() {
  memory.write16(0xfff0, 0xfab0);
  memory.write16(0xfab0, 0x34f0);
  cpu.operandPtr_ = &memory[0xfff0];
  cpu.prepIndirectMode();
  QCOMPARE(cpu.effectiveAddress_, 0x34f0);
}

void OpCodesTest::testIndexedIndirectXMode() {
  memory.write16(0x82, 0xaf01);
  memory[0x2001] = 0x70;
  cpu.operandPtr_ = &memory[0x2001];
  cpu.regs.x = 0x12;
  cpu.prepIndexedIndirectXMode();
  QCOMPARE(cpu.effectiveAddress_, 0xaf01);
  QCOMPARE(cpu.effectiveOperandPtr_, &memory[0xaf01]);
}

void OpCodesTest::testIndirectIndexedYMode() {
  uint8_t vector = 0x82;
  memory.write16(vector, 0xcf81);
  memory[0xd001] = 0xea;
  cpu.operandPtr_ = &vector;
  cpu.regs.y = 0x80;
  cpu.prepIndirectIndexedYMode();
  QCOMPARE(cpu.effectiveAddress_, 0xd001);
  QCOMPARE(*cpu.effectiveOperandPtr_, 0xea);
}

void OpCodesTest::testPageBoundaryCrossingDetection() {
  cpu.pageBoundaryCrossed_ = false;
  cpu.calculateEffectiveAddress(0x1080, 0x7f);
  QVERIFY(!cpu.pageBoundaryCrossed_);
  cpu.calculateEffectiveAddress(0x1080, 0x80);
  QVERIFY(cpu.pageBoundaryCrossed_);
  cpu.calculateEffectiveAddress(0x3080, 0x02);
  QVERIFY(!cpu.pageBoundaryCrossed_);
}

void OpCodesTest::testBranchForward() {
  int8_t d = 13;
  cpu.operandPtr_ = reinterpret_cast<uint8_t*>(&d);
  cpu.execBranch();
  QCOMPARE(cpu.regs.pc, AsmOrigin + 13);
  QCOMPARE(cpu.cycles, 1);
}

void OpCodesTest::testBranchBackward() {
  int8_t d = -100;
  cpu.regs.pc += 120;
  cpu.operandPtr_ = reinterpret_cast<uint8_t*>(&d);
  cpu.execBranch();
  QCOMPARE(cpu.regs.pc, AsmOrigin + 20);
  QCOMPARE(cpu.cycles, 1);
}

void OpCodesTest::testBranchWithPageBoundaryCrossed() {
  int8_t d = 13;
  cpu.regs.pc += 250;
  cpu.operandPtr_ = reinterpret_cast<uint8_t*>(&d);
  cpu.execBranch();
  QCOMPARE(cpu.regs.pc, AsmOrigin + 263);
  QCOMPARE(cpu.cycles, 2);
}

void OpCodesTest::testADC() {
  auto setup = [&](bool c, uint8_t a, uint8_t op) {
    cpu.regs.p = 0;
    cpu.regs.p.carry = c;
    cpu.regs.a = a;
    memory[0x2000] = op;
  };

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
  TEST_ANZCV(uint8_t(-120), 1, 0, 1, 0);
}

void OpCodesTest::testSBC() {
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

void OpCodesTest::testAND() {
  cpu.regs.a = 0x84;
  TEST_INST("AND #$fb", 2);
  TEST_ANZCV(0x80, 1, 0, 0, 0);

  cpu.regs.a = 0x84;
  cpu.regs.y = 0x12;
  memory.write16(0x70, 0x20f0);
  memory[0x2102] = 0xfb;
  TEST_INST("AND ($70),Y", 6);
  TEST_ANZCV(0x80, 1, 0, 0, 0);
}

void OpCodesTest::testASL() {
  cpu.regs.a = 0b11000001;
  TEST_INST("ASL", 2);
  TEST_ANZCV(0b10000010, 1, 0, 1, 0);

  memory[0xf002] = 0b01000001;
  cpu.regs.x = 0x12;
  TEST_INST("ASL $eff0,X", 7);
  TEST_ANZCV(0b10000010, 1, 0, 0, 0);
}

void OpCodesTest::testBCC_taken() {
  cpu.regs.p.carry = false;
  TEST_INST("BCC +3", 3);
  TEST_BRANCH_TAKEN();
}

void OpCodesTest::testBCC_notTaken() {
  cpu.regs.p.carry = true;
  TEST_INST("BCC +13", 2);
  TEST_BRANCH_NOT_TAKEN();
}

void OpCodesTest::testBCS_taken() {
  cpu.regs.p.carry = true;
  TEST_INST("BCS +33", 3);
  TEST_BRANCH_TAKEN();
}

void OpCodesTest::testBCS_notTaken() {
  cpu.regs.p.carry = false;
  TEST_INST("BCS +13", 2);
  TEST_BRANCH_NOT_TAKEN();
}

void OpCodesTest::testBEQ_taken() {
  cpu.regs.p.zero = true;
  TEST_INST("BEQ +103", 3);
  TEST_BRANCH_TAKEN();
}

void OpCodesTest::testBEQ_notTaken() {
  cpu.regs.p.zero = false;
  TEST_INST("BEQ -2", 2);
  TEST_BRANCH_NOT_TAKEN();
}

void OpCodesTest::testBNE_taken() {
  cpu.regs.p.zero = false;
  TEST_INST("BNE +43", 3);
  TEST_BRANCH_TAKEN();
}

void OpCodesTest::testBNE_notTaken() {
  cpu.regs.p.zero = true;
  TEST_INST("BNE -27", 2);
  TEST_BRANCH_NOT_TAKEN();
}

void OpCodesTest::testBMI_taken() {
  cpu.regs.p.negative = true;
  TEST_INST("BMI +32", 3);
  TEST_BRANCH_TAKEN();
}

void OpCodesTest::testBMI_notTaken() {
  cpu.regs.p.negative = false;
  TEST_INST("BMI -42", 2);
  TEST_BRANCH_NOT_TAKEN();
}

void OpCodesTest::testBPL_taken() {
  cpu.regs.p.negative = false;
  TEST_INST("BPL +2", 3);
  TEST_BRANCH_TAKEN();
}

void OpCodesTest::testBPL_notTaken() {
  cpu.regs.p.negative = true;
  TEST_INST("BPL -82", 2);
  TEST_BRANCH_NOT_TAKEN();
}

void OpCodesTest::testBVC_taken() {
  cpu.regs.p.overflow = false;
  TEST_INST("BVC +29", 3);
  TEST_BRANCH_TAKEN();
}

void OpCodesTest::testBVC_notTaken() {
  cpu.regs.p.overflow = true;
  TEST_INST("BVC +29", 2);
  TEST_BRANCH_NOT_TAKEN();
}

void OpCodesTest::testBVS_taken() {
  cpu.regs.p.overflow = true;
  TEST_INST("BVS +29", 3);
  TEST_BRANCH_TAKEN();
}

void OpCodesTest::testBVS_notTaken() {
  cpu.regs.p.overflow = false;
  TEST_INST("BVS +29", 2);
  TEST_BRANCH_NOT_TAKEN();
}

void OpCodesTest::testBIT() {
  cpu.regs.a = 0x81;
  memory[0x2001] = 0x41;
  TEST_INST("BIT $2001", 4);
  TEST_ANZCV(0x81, 0, 0, 0, 1);

  cpu.regs.a = 0x41;
  memory[0x20] = 0x02;
  TEST_INST("BIT $20", 3);
  TEST_ANZCV(0x41, 0, 1, 0, 0);
}

void OpCodesTest::testCMP() {
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
  TEST_ANZCV(uint8_t(-100), 0, 0, 1, 0);

  cpu.regs.a = 150;
  memory[0x2000] = 120;
  TEST_INST("CMP $2000", 4);
  TEST_ANZCV(150, 0, 0, 1, 0);
}

void OpCodesTest::testCLC() {
  cpu.regs.p.carry = true;
  TEST_INST("CLC", 2);
  QCOMPARE(cpu.regs.p.carry, false);
}

void OpCodesTest::testSEC() {
  cpu.regs.p.carry = false;
  TEST_INST("SEC", 2);
  QCOMPARE(cpu.regs.p.carry, true);
}

void OpCodesTest::testCLD() {
  cpu.regs.p.decimal = true;
  TEST_INST("CLD", 2);
  QCOMPARE(cpu.regs.p.decimal, false);
}

void OpCodesTest::testSED() {
  cpu.regs.p.decimal = false;
  TEST_INST("SED", 2);
  QCOMPARE(cpu.regs.p.decimal, true);
}

void OpCodesTest::testCLI() {
  cpu.regs.p.interrupt = true;
  TEST_INST("CLI", 2);
  QCOMPARE(cpu.regs.p.interrupt, false);
}

void OpCodesTest::testSEI() {
  cpu.regs.p.interrupt = false;
  TEST_INST("SEI", 2);
  QCOMPARE(cpu.regs.p.interrupt, true);
}
