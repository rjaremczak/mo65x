#include "assemblertest.h"
#include <QTest>

#define TEST_INST(instr) QCOMPARE(assembler.process(instr), Assembler::Result::Ok)

#define TEST_INST_1(instr, opCode)                                                                                               \
  TEST_INST(instr);                                                                                                              \
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
  assembler.resetOrigin(AsmOrigin);
  assembler.clearCode();
  assembler.clearSymbols();
  assembler.changeMode(Assembler::Mode::EmitCode);
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
  assembler.changeMode(Assembler::Mode::ScanForSymbols);
  TEST_INST("firstloop:");
  assembler.changeMode(Assembler::Mode::EmitCode);
  TEST_INST_1("  BNE firstloop ;loop until Y is $10", 0xd0);
}

void AssemblerTest::testRelativeModePlus() {
  TEST_INST_2("BVS +8", 0x70, 8);
}

void AssemblerTest::testOrg() {
  TEST_INST("  .ORG $3000 ;origin");
  QCOMPARE(assembler.locationCounter(), 0x3000);
  QCOMPARE(assembler.process("  .org $4000 ;origin"), Assembler::Result::OriginAlreadyDefined);
  QCOMPARE(assembler.locationCounter(), 0x3000);
}

void AssemblerTest::testOrgStar() {
  TEST_INST("  *= $5000 ;origin");
  QCOMPARE(assembler.locationCounter(), 0x5000);
}

void AssemblerTest::testComment() {
  TEST_INST("  SEI   ;disable interrupts ");
  TEST_INST("; disable interrupts ");
  TEST_INST(" LDA #$20  ;comment");
}

void AssemblerTest::testEmptyLineLabel() {
  assembler.changeMode(Assembler::Mode::ScanForSymbols);
  TEST_INST("Label_001:");
  QCOMPARE(assembler.symbol("Label_001"), assembler.locationCounter());
  QCOMPARE(assembler.symbol("dummy"), std::nullopt);
}

void AssemblerTest::testSymbolPass() {
  assembler.changeMode(Assembler::Mode::ScanForSymbols);
  assembler.defineOrigin(1000);
  TEST_INST("TestLabel_01:  SEI   ; disable interrupts ");
  QCOMPARE(assembler.symbol("TestLabel_01"), 1000);
  QCOMPARE(assembler.code().size(), 0U);
  QCOMPARE(assembler.locationCounter(), 1001U);
}

void AssemblerTest::testAssemblyPass() {
  assembler.changeMode(Assembler::Mode::EmitCode);
  assembler.defineOrigin(2002);
  TEST_INST("CLI");
  TEST_INST("TestLabel_11:  LDA #$20   ; this is a one weird comment  ");
  QCOMPARE(assembler.symbol("TestLabel_11"), std::nullopt);
  QCOMPARE(assembler.code().size(), 3U);
  QCOMPARE(assembler.locationCounter(), 2005);
}

void AssemblerTest::testEmitByte() {
  TEST_INST(".BYTE $20");
}
