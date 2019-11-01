#include "assemblertest.h"
#include <QTest>

AssemblerTest::AssemblerTest(QObject* parent) : QObject(parent), assembler(memory) {
}

void AssemblerTest::verify(const char* str, uint8_t opcode, int lo, int hi) {
  QVERIFY(assembler.assemble(str));
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
  verify("LDY $8f", 0xa4, 0x8f);
}

void AssemblerTest::testZeroPageXMode() {
  verify("LDA $a0,X", 0xb5, 0xa0);
}

void AssemblerTest::testZeroPageYMode() {
  verify("STX $7a,Y", 0x96, 0x7a);
}

void AssemblerTest::testAbsoluteMode() {
  // verify("STX $7a,Y", 0x96, 0x7a);
  verify("ROR $3400", 0x6e, 0x00, 0x34);
}
