#include "assemblertest.h"
#include <QTest>

#define TEST_INST(instr) QVERIFY(assembler.processLine(instr) == AssemblyResult::Ok)

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
  QVERIFY(assembler.symbols().empty());
  QCOMPARE(assembler.mode, Assembler::ProcessingMode::EmitCode);
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
  TEST_INST_3("jmp $2000", 0x4c, 0x00, 0x20);
  assembler.symbolTable.put("c", 0xfab0);
  TEST_INST_3("jmp c", 0x4c, 0xb0, 0xfa);
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
  TEST_INST("c:lda dziabaDucha");
  QCOMPARE(assembler.symbolTable.get("TestLabel_01"), 1000);
  QCOMPARE(assembler.written, 0);
  QCOMPARE(assembler.locationCounter, 1004);
}

void AssemblerTest::testAssemblyPass() {
  assembler.locationCounter = 2002;
  TEST_INST("CLI");
  TEST_INST("TestLabel_11:  LDA #$20   ; this is a one weird comment  ");
  QCOMPARE(assembler.symbolTable.get("TestLabel_11"), std::nullopt);
  QCOMPARE(assembler.written, 3);
  QCOMPARE(assembler.locationCounter, 2005);
}

void AssemblerTest::testEmitBytes() {
  TEST_INST(".BYTE 20");
  QCOMPARE(assembler.bytesWritten(), 1);
  QCOMPARE(memory[assembler.lastLocationCounter], 20);
  TEST_INST(".BYTE $20 45 $4a");
  QCOMPARE(assembler.bytesWritten(), 4);
  QCOMPARE(memory[assembler.lastLocationCounter], 0x20);
  QCOMPARE(memory[assembler.lastLocationCounter + 1], 45);
  QCOMPARE(memory[assembler.lastLocationCounter + 2], 0x4a);
  TEST_INST(".BYTE $20, $3f,$4a ,$23 , 123");
  QCOMPARE(assembler.bytesWritten(), 9);
  QCOMPARE(memory[assembler.lastLocationCounter], 0x20);
  QCOMPARE(memory[assembler.lastLocationCounter + 1], 0x3f);
  QCOMPARE(memory[assembler.lastLocationCounter + 2], 0x4a);
  QCOMPARE(memory[assembler.lastLocationCounter + 3], 0x23);
  QCOMPARE(memory[assembler.lastLocationCounter + 4], 123);
}

void AssemblerTest::testEmitWords() {
  TEST_INST(".word $20ff $23af $fab0 ; test comment");
  QCOMPARE(assembler.bytesWritten(), 6);
  QCOMPARE(memory.word(assembler.lastLocationCounter), 0x20ff);
  QCOMPARE(memory.word(assembler.lastLocationCounter + 2), 0x23af);
  QCOMPARE(memory.word(assembler.lastLocationCounter + 4), 0xfab0);
  TEST_INST(".word $3000 $15ad 10230");
  QCOMPARE(assembler.bytesWritten(), 12);
  QCOMPARE(memory.word(assembler.lastLocationCounter), 0x3000);
  QCOMPARE(memory.word(assembler.lastLocationCounter + 2), 0x15ad);
  QCOMPARE(memory.word(assembler.lastLocationCounter + 4), 10230);
}

void AssemblerTest::testLowerCaseInstruction() {
  TEST_INST("cli");
}

void AssemblerTest::testDcb() {
  TEST_INST("dcb 0,0,0,0,0,0,0,0,0,$b,$b,$c,$f,$f,$f,$f");
  QCOMPARE(assembler.bytesWritten(), 16);
  QCOMPARE(memory[assembler.lastLocationCounter], 0);
  QCOMPARE(memory[assembler.lastLocationCounter + 15], 0xf);
}

void AssemblerTest::testLoBytePrefix() {
  TEST_INST_2("LDA #<$1afc", 0xa9, 0xfc);
  assembler.symbolTable.put("label", 0x2afe);
  TEST_INST_2("LDA #<label", 0xa9, 0xfe);
  TEST_INST("dcb <label, 2");
  QCOMPARE(memory[assembler.lastLocationCounter], 0xfe);
  QCOMPARE(memory[assembler.lastLocationCounter + 1], 2);
}

void AssemblerTest::testHiBytePrefix() {
  TEST_INST_2("LDA #>$1afc", 0xa9, 0x1a);
  assembler.symbolTable.put("label", 0x3afe);
  TEST_INST_2("LDA #>label", 0xa9, 0x3a);
  TEST_INST_2("dcb >label, 2", 0x3a, 2);
}

void AssemblerTest::testLoHiBytePrefix() {
  assembler.symbolTable.put("a", 0xfa20);
  assembler.symbolTable.put("b", 0x10a0);
  TEST_INST_2("dcb >a, <b", 0xfa, 0xa0);
  TEST_INST_2("dcb <a, >b", 0x20, 0x10);
}

void AssemblerTest::testSymbolDef() {
  assembler.changeMode(Assembler::ProcessingMode::ScanForSymbols);
  TEST_INST(".org $1000");
  TEST_INST("lda init");
  QCOMPARE(assembler.locationCounter, 0x1003);
}
