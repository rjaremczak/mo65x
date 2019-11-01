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

void OpCodesTest::testAccumulatorMode() {
  cpu.amAccumulator();
  QCOMPARE(cpu.effectiveOperandPtr_, &cpu.registers.a);
}

void OpCodesTest::testImmediateMode() {
  assembler.assemble("LDA #$23");
  cpu.execute();
  QCOMPARE(cpu.registers.a, 0x23);
}

void OpCodesTest::testZeroPageMode() {
  assembler.assemble(LDY, ZeroPage, ZeroPageByteAddress);
  cpu.execute();
  QCOMPARE(cpu.registers.y, ZeroPageByte);
}
