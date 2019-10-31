#include "assemblertest.h"
#include <QTest>

AssemblerTest::AssemblerTest(QObject* parent) : QObject(parent), assembler(memory) {
}

void AssemblerTest::verify(const char* str, uint8_t opcode, int lo, int hi) {
  QVERIFY(assembler.enter(str));
  QCOMPARE(memory[AsmOrigin], opcode);
  if (lo >= 0) QCOMPARE(lo, memory[AsmOrigin + 1]);
  if (hi >= 0) QCOMPARE(hi, memory[AsmOrigin + 2]);
}

void AssemblerTest::init() {
  assembler.setOrigin(AsmOrigin);
}

void AssemblerTest::testImpliedMode() {
  verify("SEI", 0x78);
}

void AssemblerTest::testAccumulatorMode() {
  verify("ASL", 0x0a);
}

void AssemblerTest::testImmediateMode() {
  verify("LDX #$2f", 0xa2, 0x2f);
}

void AssemblerTest::testZeroPageMode() {
}
