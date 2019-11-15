#include "assemblertest.h"
#include <QTest>

#define TEST_INST_1(instr, opCode)                                                                                               \
  QCOMPARE(assembler.process(instr), Assembler::Result::Ok);                                                                     \
  QCOMPARE(assembler.lastInstructionByte(0), opCode)

#define TEST_INST_2(instr, opCode, lo)                                                                                           \
  TEST_INST_1(instr, opCode);                                                                                                    \
  if (lo >= 0) QCOMPARE(lo, assembler.lastInstructionByte(1))

#define TEST_INST_3(instr, opCode, lo, hi)                                                                                       \
  TEST_INST_2(instr, opCode, lo);                                                                                                \
  if (hi >= 0) QCOMPARE(hi, assembler.lastInstructionByte(2))

AssemblerTest::AssemblerTest(QObject* parent) : QObject(parent) {
}

void AssemblerTest::init() {
  assembler.reset();
  assembler.setOrigin(AsmOrigin);
}

void AssemblerTest::testByteOperand() {
  TEST_INST_2("LDY %11010001", 0xa4, 0b11010001);
  TEST_INST_2("LDX #123", 0xa2, 123);
  TEST_INST_2("LDA #%00110101", 0xa9, 0b00110101);
}

void AssemblerTest::testWordOperand() {
  TEST_INST_3("ROR %0001001011101101", 0x6e, 0b11101101, 0b00010010);
  TEST_INST_3("LSR 65533,X", 0x5e, 0xfd, 0xff);
  TEST_INST_3("JMP ($ffa0)", 0x6c, 0xa0, 0xff);
}

void AssemblerTest::testImpliedMode() {
  TEST_INST_1("SEI", 0x78);
}

void AssemblerTest::testAccumulatorMode() {
  TEST_INST_1("ASL", 0x0a);
}

void AssemblerTest::testImmediateMode() {
  TEST_INST_2("LDX #$2f", 0xa2, 0x2f);
}

void AssemblerTest::testZeroPageMode() {
  TEST_INST_2("LDY $8f", 0xa4, 0x8f);
}

void AssemblerTest::testZeroPageXMode() {
  TEST_INST_2("LDA $a0,X", 0xb5, 0xa0);
}

void AssemblerTest::testZeroPageYMode() {
  TEST_INST_2("STX $7a,Y", 0x96, 0x7a);
}

void AssemblerTest::testAbsoluteMode() {
  TEST_INST_3("ROR $3400", 0x6e, 0x00, 0x34);
}

void AssemblerTest::testAbsoluteXMode() {
  TEST_INST_3("LSR $35f0,X", 0x5e, 0xf0, 0x35);
}

void AssemblerTest::testAbsoluteYMode() {
  TEST_INST_3("EOR $f7a0,Y", 0x59, 0xa0, 0xf7);
}

void AssemblerTest::testIndirectMode() {
  TEST_INST_3("JMP ($ffa0)", 0x6c, 0xa0, 0xff);
}

void AssemblerTest::testIndexedIndirectXMode() {
  TEST_INST_2("LDA ($8c,X)", 0xa1, 0x8c);
}

void AssemblerTest::testIndirectIndexedYMode() {
  TEST_INST_2("ORA ($a7),Y", 0x11, 0xa7);
}

void AssemblerTest::testRelativeModeMinus() {
  TEST_INST_2("BCC -1", 0x90, -1);
}

void AssemblerTest::testRelativeModeLabel() {
  assembler.reset(Assembler::Pass::ScanForSymbols);
  QCOMPARE(assembler.process("firstloop:"), Assembler::Result::Ok);
  assembler.reset(Assembler::Pass::Assembly);
  TEST_INST_1("  BNE firstloop ;loop until Y is $10", 0xd0);
}

void AssemblerTest::testRelativeModePlus() {
  TEST_INST_2("BVS +8", 0x70, 8);
}

void AssemblerTest::testOrg() {
  assembler.reset();
  QCOMPARE(assembler.process("  ORG $3000 ;origin"), Assembler::Result::Ok);
  QCOMPARE(assembler.locationCounter(), 0x3000);
  QCOMPARE(assembler.process("  ORG $4000 ;origin"), Assembler::Result::OriginAlreadyDefined);
  QCOMPARE(assembler.locationCounter(), 0x3000);

  assembler.reset();
  QCOMPARE(assembler.process("  .org $5000 ;origin"), Assembler::Result::Ok);
  QCOMPARE(assembler.locationCounter(), 0x5000);

  assembler.reset();
  QCOMPARE(assembler.process("  *= $5000 ;origin defined"), Assembler::Result::Ok);
  QCOMPARE(assembler.locationCounter(), 0x5000);
}

void AssemblerTest::testComment() {
  QCOMPARE(assembler.process("  SEI   ;disable interrupts "), Assembler::Result::Ok);
  QCOMPARE(assembler.process("; disable interrupts "), Assembler::Result::Ok);
  QCOMPARE(assembler.process(" LDA #$20  ;comment"), Assembler::Result::Ok);
}

void AssemblerTest::testEmptyLineLabel() {
  assembler.reset(Assembler::Pass::ScanForSymbols);
  QCOMPARE(assembler.process("Label_001:"), Assembler::Result::Ok);
  QCOMPARE(assembler.symbol("Label_001"), assembler.locationCounter());
  QCOMPARE(assembler.symbol("dummy"), std::nullopt);
}

void AssemblerTest::testSymbolPass() {
  assembler.reset(Assembler::Pass::ScanForSymbols);
  assembler.setOrigin(1000);
  QCOMPARE(assembler.process("TestLabel_01:  SEI   ; disable interrupts "), Assembler::Result::Ok);
  QCOMPARE(assembler.symbol("TestLabel_01"), 1000);
  QCOMPARE(assembler.code().size(), 0U);
  QCOMPARE(assembler.locationCounter(), 1001U);
}

void AssemblerTest::testAssemblyPass() {
  assembler.reset(Assembler::Pass::Assembly);
  assembler.setOrigin(2002);
  QCOMPARE(assembler.process("CLI"), Assembler::Result::Ok);
  QCOMPARE(assembler.process("TestLabel_11:  LDA #$20   ; this is a one weird comment  "), Assembler::Result::Ok);
  QCOMPARE(assembler.symbol("TestLabel_11"), std::nullopt);
  QCOMPARE(assembler.code().size(), 3U);
  QCOMPARE(assembler.locationCounter(), 2005);
}

void AssemblerTest::testLabelDefAndUse() {
}
