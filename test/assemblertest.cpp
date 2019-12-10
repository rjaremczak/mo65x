#include "assemblertest.h"
#include <QTest>

#define TEST_INST(instr) QCOMPARE(assembler.processLine(instr), AssemblyResult::Ok)

#define TEST_INST_1(instr, opCode)                                                                                               \
  TEST_INST(instr);                                                                                                              \
  QCOMPARE(memory[assembler.lastLocationCounter], opCode)

#define TEST_INST_2(instr, opCode, lo)                                                                                           \
  TEST_INST_1(instr, opCode);                                                                                                    \
  if (lo >= 0) QCOMPARE(lo, memory[assembler.lastLocationCounter + 1])

#define TEST_INST_3(instr, opCode, lo, hi)                                                                                       \
  TEST_INST_2(instr, opCode, lo);                                                                                                \
  if (hi >= 0) QCOMPARE(hi, memory[assembler.lastLocationCounter + 2])

AssemblerTest::AssemblerTest(QObject* parent) : QObject(parent), assembler(memory) {
}

void AssemblerTest::init() {
  assembler.init(AsmOrigin);
  assembler.clearSymbols();
  assembler.changeMode(Assembler::ProcessingMode::EmitCode);
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
  assembler.changeMode(Assembler::ProcessingMode::ScanForSymbols);
  TEST_INST("firstloop:");
  assembler.changeMode(Assembler::ProcessingMode::EmitCode);
  TEST_INST_1("  BNE firstloop ;loop until Y is $10", 0xd0);
}

void AssemblerTest::testRelativeModePlus() {
  TEST_INST_2("BVS +8", 0x70, 8);
}

void AssemblerTest::testOrg() {
  TEST_INST("  .ORG $3000 ;origin");
  QCOMPARE(assembler.locationCounter, 0x3000);
  TEST_INST("  .ORG $4000 ;origin");
  QCOMPARE(assembler.locationCounter, 0x4000);
}

void AssemblerTest::testOrgStar() {
  TEST_INST("  *= $5000 ;origin");
  QCOMPARE(assembler.locationCounter, 0x5000);
}

void AssemblerTest::testComment() {
  TEST_INST("  SEI   ;disable interrupts ");
  TEST_INST("; disable interrupts ");
  TEST_INST(" LDA #$20  ;comment");
}

void AssemblerTest::testEmptyLineLabel() {
  assembler.changeMode(Assembler::ProcessingMode::ScanForSymbols);
  TEST_INST("Label_001:");
  QCOMPARE(assembler.symbolTable.get("Label_001"), assembler.locationCounter);
  QCOMPARE(assembler.symbolTable.get("dummy"), std::nullopt);
}

void AssemblerTest::testSymbolPass() {
  assembler.changeMode(Assembler::ProcessingMode::ScanForSymbols);
  assembler.locationCounter = 1000;
  TEST_INST("TestLabel_01:  SEI   ; disable interrupts ");
  QCOMPARE(assembler.symbolTable.get("TestLabel_01"), 1000);
  QCOMPARE(assembler.written, 0U);
  QCOMPARE(assembler.locationCounter, 1001U);
}

void AssemblerTest::testAssemblyPass() {
  assembler.changeMode(Assembler::ProcessingMode::EmitCode);
  assembler.locationCounter = 2002;
  TEST_INST("CLI");
  TEST_INST("TestLabel_11:  LDA #$20   ; this is a one weird comment  ");
  QCOMPARE(assembler.symbolTable.get("TestLabel_11"), std::nullopt);
  QCOMPARE(assembler.written, 3U);
  QCOMPARE(assembler.locationCounter, 2005);
}

void AssemblerTest::testEmitByte() {
  assembler.changeMode(Assembler::ProcessingMode::EmitCode);
  TEST_INST_1(".BYTE $20", 0x20);
}

void AssemblerTest::testEmitWord() {
  TEST_INST_2(".word $20ff ; test comment", 0xff, 0x20);
  TEST_INST_2(".word $3000 $15ad 10230", 0x00, 0x30);
}

void AssemblerTest::testLowerCaseInstruction() {
  TEST_INST(" lda #$f0  ;comment");
}
