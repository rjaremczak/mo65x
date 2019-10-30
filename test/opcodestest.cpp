#include "opcodestest.h"
#include <QTest>
#include <algorithm>

OpCodesTest::OpCodesTest(QObject* parent) : QObject(parent), cpu_(memory_) {
}

void OpCodesTest::initTestCase() {
  QVERIFY(&cpu_.memory_ == &memory_);
  std::fill(memory_.begin(), memory_.end(), 0);
  QVERIFY(std::accumulate(memory_.begin(), memory_.end(), 0) == 0);
}

void OpCodesTest::init() {
  cpu_.registers.pc = 0x0000;
  memory_.write16(Cpu::VectorRESET, 0xFCE2);
  cpu_.reset();
  QVERIFY(cpu_.registers.pc == 0xFCE2);
}

void OpCodesTest::testImmediateAddressingMode() {
  cpu_.operandPtr_ = nullptr;
  cpu_.operandPtr_ = &memory_[1234];
  cpu_.amImmediate();
  QVERIFY(cpu_.effectiveOperandPtr_ == &memory_[1234]);
}
