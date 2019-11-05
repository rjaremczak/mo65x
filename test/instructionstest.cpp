#include "instructionstest.h"
#include "disassembler.h"
#include <QTest>
#include <algorithm>

static constexpr auto AsmOrigin = 0x800;

OpCodesTest::OpCodesTest(QObject* parent) : QObject(parent), cpu(memory), assembler(memory) {
}

void OpCodesTest::exec(const char* str, int cycles) {
  cpu.cycles = 0;
  QVERIFY(assembler.assemble(str));
  cpu.execute();
  QCOMPARE(cpu.executionStatus, Stopped);
  if (cycles > 0) QCOMPARE(cpu.cycles, cycles);
}

void OpCodesTest::exec(InstructionType type, AddressingMode mode, int operand) {
  cpu.cycles = 0;
  QVERIFY(assembler.assemble(type, mode, operand));
  cpu.execute();
  QCOMPARE(cpu.executionStatus, Stopped);
}

void OpCodesTest::verifyANZCV(uint8_t acc, bool n, bool z, bool c, bool v) {
  QCOMPARE(cpu.registers.a, acc);
  QCOMPARE(cpu.registers.p.n, n);
  QCOMPARE(cpu.registers.p.z, z);
  QCOMPARE(cpu.registers.p.c, c);
  QCOMPARE(cpu.registers.p.v, v);
}

void OpCodesTest::initTestCase() {
  QVERIFY(&cpu.memory_ == &memory);
  std::fill(memory.begin(), memory.end(), 0);
  QVERIFY(std::accumulate(memory.begin(), memory.end(), 0) == 0);
}

void OpCodesTest::init() {
  cpu.registers.pc = 0x0000;
  memory.write16(Cpu::VectorRESET, 0xFCE2);
  cpu.reset();
  QCOMPARE(cpu.registers.pc, 0xFCE2);
  assembler.setOrigin(AsmOrigin);
  cpu.registers.pc = AsmOrigin;
  cpu.registers.p = 0;
}

void OpCodesTest::testImpliedMode() {
  cpu.registers.p.i = true;
  exec(CLI);
  QVERIFY(!cpu.registers.p.i);
}

void OpCodesTest::testAccumulatorMode() {
  cpu.prepAccumulatorMode();
  QCOMPARE(cpu.effectiveOperandPtr_, &cpu.registers.a);
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
  cpu.registers.x = 0x90;
  cpu.prepZeroPageXMode();
  QCOMPARE(cpu.effectiveAddress_, 0x10);
  QCOMPARE(*cpu.effectiveOperandPtr_, 0xe8);
}

void OpCodesTest::testZeroPageYMode() {
  memory[0xa0] = 0x80;
  memory[0xb1] = 0xe8;
  cpu.operandPtr_ = &memory[0xa0];
  cpu.registers.y = 0x31;
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
  cpu.registers.x = 0x10;
  cpu.prepAbsoluteXMode();
  QCOMPARE(cpu.effectiveAddress_, 0x20b0);
  QCOMPARE(*cpu.effectiveOperandPtr_, 0x34);
}

void OpCodesTest::testAbsoluteYMode() {
  memory[0xa020] = 0x84;
  memory.write16(0x2002, 0xa010);
  cpu.operandPtr_ = &memory[0x2002];
  cpu.registers.y = 0x10;
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
  cpu.registers.x = 0x12;
  cpu.prepIndexedIndirectXMode();
  QCOMPARE(cpu.effectiveAddress_, 0xaf01);
  QCOMPARE(cpu.effectiveOperandPtr_, &memory[0xaf01]);
}

void OpCodesTest::testIndirectIndexedYMode() {
  uint8_t vector = 0x82;
  memory.write16(vector, 0xcf81);
  memory[0xd001] = 0xea;
  cpu.operandPtr_ = &vector;
  cpu.registers.y = 0x80;
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

void OpCodesTest::testADC() {
  cpu.registers.a = 0x03;
  exec("ADC #$f0", 2);
  verifyANZCV(0xf3, 1, 0, 0, 0);

  cpu.registers.p = 0;
  cpu.registers.p.c = true;
  cpu.registers.a = 0xf0;
  memory[0x2000] = 0x0f;
  exec("ADC $2000", 4);
  verifyANZCV(0x00, 0, 1, 1, 0);
}

void OpCodesTest::testAND() {
  cpu.registers.a = 0x84;
  exec("AND #$fb", 2);
  verifyANZCV(0x80, 1, 0, 0, 0);

  cpu.registers.a = 0x84;
  cpu.registers.y = 0x12;
  memory.write16(0x70, 0x20f0);
  memory[0x2102] = 0xfb;
  exec("AND ($70),Y", 6);
  verifyANZCV(0x80, 1, 0, 0, 0);
}

void OpCodesTest::testASL() {
  cpu.registers.a = 0b11000001;
  exec("ASL", 2);
  verifyANZCV(0b10000010, 1, 0, 1, 0);

  memory[0xf002] = 0b01000001;
  cpu.registers.x = 0x12;
  exec("ASL $eff0,X", 7);
  verifyANZCV(0b10000010, 1, 0, 0, 0);
}
