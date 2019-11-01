#include "instructionstest.h"
#include "disassembler.h"
#include <QTest>
#include <algorithm>

namespace {
constexpr auto ZeroPageVectorAddress = 0x23;
constexpr auto ZeroPageVector = 0x2f80;
constexpr auto ZeroPageByteAddress = 0xa0;
constexpr auto ZeroPageByte = 0xfa;
constexpr auto MemVectorAddress = 0x2300;
constexpr auto MemVector = 0xa800;
constexpr auto AsmOrigin = 0x0800;
}

OpCodesTest::OpCodesTest(QObject* parent) : QObject(parent), cpu(memory), assembler(memory) {
}

void OpCodesTest::test(const char* str) {
  cpu.cycles = 0;
  QVERIFY(assembler.assemble(str));
  cpu.execute();
  QCOMPARE(cpu.executionStatus, Stopped);
}

void OpCodesTest::test(InstructionType type, AddressingMode mode, int operand) {
  cpu.cycles = 0;
  QVERIFY(assembler.assemble(type, mode, operand));
  cpu.execute();
  QCOMPARE(cpu.executionStatus, Stopped);
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
  memory.write16(ZeroPageVectorAddress, ZeroPageVector);
  memory[ZeroPageByteAddress] = ZeroPageByte;
  memory.write16(MemVectorAddress, MemVector);
  cpu.registers.pc = AsmOrigin;
  assembler.setOrigin(AsmOrigin);
}

void OpCodesTest::testImpliedMode() {
  cpu.registers.p.i = true;
  test(CLI);
  QVERIFY(!cpu.registers.p.i);
}

void OpCodesTest::testAccumulatorMode() {
  cpu.amAccumulator();
  QCOMPARE(cpu.effectiveOperandPtr_, &cpu.registers.a);
}

void OpCodesTest::testImmediateMode() {
  cpu.operandPtr_ = &memory[0x10];
  cpu.amImmediate();
  QCOMPARE(cpu.effectiveOperandPtr_, &memory[0x10]);
}

void OpCodesTest::testZeroPageMode() {
  memory[0xa0] = 0x40;
  memory[0x40] = 0xf8;
  cpu.operandPtr_ = &memory[0xa0];
  cpu.amZeroPage();
  QCOMPARE(cpu.effectiveAddress_, 0x40);
  QCOMPARE(*cpu.effectiveOperandPtr_, 0xf8);
}

void OpCodesTest::testZeroPageXMode() {
  memory[0xa0] = 0x80;
  memory[0x10] = 0xe8;
  cpu.operandPtr_ = &memory[0xa0];
  cpu.registers.x = 0x90;
  cpu.amZeroPageX();
  QCOMPARE(cpu.effectiveAddress_, 0x10);
  QCOMPARE(*cpu.effectiveOperandPtr_, 0xe8);
}

void OpCodesTest::testASL() {
  cpu.registers.a = 0b11000001;
  test(ASL);
  QCOMPARE(cpu.cycles, 2);
  QCOMPARE(cpu.registers.a, 0b10000010);
  QVERIFY(cpu.registers.p.c);
  QVERIFY(!cpu.registers.p.z);
  QVERIFY(cpu.registers.p.n);
}
