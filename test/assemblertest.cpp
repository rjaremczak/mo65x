#include <gtest/gtest.h>

#define private public
#include "assembler.h"
#undef private

static constexpr auto AsmOrigin = 0x0800;

class AssemblerTest : public ::testing::Test {
protected:
  AssemblerTest();
  SymbolTable symbols;
  Assembler assembler;
  Memory memory;
};

#define TEST_INST(instr) EXPECT_EQ(assembler.processLine(instr), AssemblyResult::Ok)

#define TEST_INST_1(instr, opCode)                                                                                               \
  TEST_INST(instr);                                                                                                              \
  EXPECT_EQ(memory[assembler.m_lastLocationCounter], opCode)

#define TEST_INST_2(instr, opCode, lo)                                                                                           \
  TEST_INST_1(instr, opCode);                                                                                                    \
  if (lo >= 0) EXPECT_EQ(lo, memory[assembler.m_lastLocationCounter + 1])

#define TEST_INST_3(instr, opCode, lo, hi)                                                                                       \
  TEST_INST_2(instr, opCode, lo);                                                                                                \
  if (hi >= 0) EXPECT_EQ(hi, memory[assembler.m_lastLocationCounter + 2])

AssemblerTest::AssemblerTest() : ::testing::Test(), assembler(memory, symbols) {
}

TEST_F(AssemblerTest, init) {
  assembler.init(AsmOrigin);
  EXPECT_TRUE(symbols.empty());
  EXPECT_EQ(assembler.m_mode, Assembler::ProcessingMode::EmitCode);
}

TEST_F(AssemblerTest, testByteOperand) {
  TEST_INST_2("LDY %11010001", 0xa4, 0b11010001);
  TEST_INST_2("LDX #123", 0xa2, 123);
  TEST_INST_2("LDA #%00110101", 0xa9, 0b00110101);
}

TEST_F(AssemblerTest, testWordOperand) {
  TEST_INST_3("ROR %0001001011101101", 0x6e, 0b11101101, 0b00010010);
  TEST_INST_3("LSR 65533,X", 0x5e, 0xfd, 0xff);
  TEST_INST_3("JMP ($ffa0)", 0x6c, 0xa0, 0xff);
}

TEST_F(AssemblerTest, testImpliedMode) {
  TEST_INST_1("SEI", 0x78);
}

TEST_F(AssemblerTest, testAccumulatorMode) {
  TEST_INST_1("ASL", 0x0a);
}

TEST_F(AssemblerTest, testImmediateMode) {
  TEST_INST_2("LDX #$2f", 0xa2, 0x2f);
}

TEST_F(AssemblerTest, testZeroPageMode) {
  TEST_INST_2("LDY $8f", 0xa4, 0x8f);
}

TEST_F(AssemblerTest, testZeroPageXMode) {
  TEST_INST_2("LDA $a0,X", 0xb5, 0xa0);
}

TEST_F(AssemblerTest, testZeroPageYMode) {
  TEST_INST_2("STX $7a,Y", 0x96, 0x7a);
}

TEST_F(AssemblerTest, testAbsoluteMode) {
  TEST_INST_3("ROR $3400", 0x6e, 0x00, 0x34);
  TEST_INST_3("jmp $2000", 0x4c, 0x00, 0x20);
  symbols.put("c", 0xfab0);
  TEST_INST_3("jmp c", 0x4c, 0xb0, 0xfa);
}

TEST_F(AssemblerTest, testAbsoluteXMode) {
  TEST_INST_3("LSR $35f0,X", 0x5e, 0xf0, 0x35);
}

TEST_F(AssemblerTest, testAbsoluteYMode) {
  TEST_INST_3("EOR $f7a0,Y", 0x59, 0xa0, 0xf7);
}

TEST_F(AssemblerTest, testIndirectMode) {
  TEST_INST_3("JMP ($ffa0)", 0x6c, 0xa0, 0xff);
}

TEST_F(AssemblerTest, testIndexedIndirectXMode) {
  TEST_INST_2("LDA ($8c,X)", 0xa1, 0x8c);
}

TEST_F(AssemblerTest, testIndirectIndexedYMode) {
  TEST_INST_2("ORA ($a7),Y", 0x11, 0xa7);
}

TEST_F(AssemblerTest, testRelativeModeMinus) {
  TEST_INST_2("BCC -1", 0x90, -1);
}

TEST_F(AssemblerTest, testRelativeModeLabel) {
  assembler.changeMode(Assembler::ProcessingMode::ScanForSymbols);
  TEST_INST("firstloop:");
  assembler.changeMode(Assembler::ProcessingMode::EmitCode);
  TEST_INST_1("  BNE firstloop ;loop until Y is $10", 0xd0);
}

TEST_F(AssemblerTest, testRelativeModePlus) {
  TEST_INST_2("BVS +8", 0x70, 8);
}

TEST_F(AssemblerTest, testOrg) {
  TEST_INST("  .ORG $3000 ;origin");
  EXPECT_EQ(assembler.m_locationCounter, 0x3000);
  TEST_INST("  .ORG $4000 ;origin");
  EXPECT_EQ(assembler.m_locationCounter, 0x4000);
}

TEST_F(AssemblerTest, testOrgStar) {
  TEST_INST("  *= $5000 ;origin");
  EXPECT_EQ(assembler.m_locationCounter, 0x5000);
}

TEST_F(AssemblerTest, testComment) {
  TEST_INST("  SEI   ;disable interrupts ");
  TEST_INST("; disable interrupts ");
  TEST_INST(" LDA #$20  ;comment");
}

TEST_F(AssemblerTest, testEmptyLineLabel) {
  assembler.changeMode(Assembler::ProcessingMode::ScanForSymbols);
  TEST_INST("Label_001:");
  EXPECT_EQ(symbols.get("Label_001"), assembler.m_locationCounter);
  EXPECT_EQ(symbols.get("dummy"), std::nullopt);
}

TEST_F(AssemblerTest, testSymbolPass) {
  assembler.init(1000);
  assembler.changeMode(Assembler::ProcessingMode::ScanForSymbols);
  TEST_INST("TestLabel_01:  SEI   ; disable interrupts ");
  TEST_INST("c:lda dziabaDucha");
  EXPECT_EQ(symbols.get("TestLabel_01"), 1000);
  EXPECT_EQ(assembler.bytesWritten(), 0);
  EXPECT_EQ(assembler.m_locationCounter, 1004);
}

TEST_F(AssemblerTest, testAssemblyPass) {
  assembler.init(2002);
  TEST_INST("CLI");
  TEST_INST("TestLabel_11:  LDA #$20   ; this is a one weird comment  ");
  EXPECT_EQ(symbols.get("TestLabel_11"), std::nullopt);
  EXPECT_EQ(assembler.bytesWritten(), 3);
  EXPECT_EQ(assembler.m_locationCounter, 2005);
}

TEST_F(AssemblerTest, testEmitBytes) {
  TEST_INST(".BYTE 20");
  EXPECT_EQ(assembler.bytesWritten(), 1);
  EXPECT_EQ(memory[assembler.m_lastLocationCounter], 20);
  TEST_INST(".BYTE $20 45 $4a");
  EXPECT_EQ(assembler.bytesWritten(), 4);
  EXPECT_EQ(memory[assembler.m_lastLocationCounter], 0x20);
  EXPECT_EQ(memory[assembler.m_lastLocationCounter + 1], 45);
  EXPECT_EQ(memory[assembler.m_lastLocationCounter + 2], 0x4a);
  TEST_INST(".BYTE $20, $3f,$4a ,$23 , 123");
  EXPECT_EQ(assembler.bytesWritten(), 9);
  EXPECT_EQ(memory[assembler.m_lastLocationCounter], 0x20);
  EXPECT_EQ(memory[assembler.m_lastLocationCounter + 1], 0x3f);
  EXPECT_EQ(memory[assembler.m_lastLocationCounter + 2], 0x4a);
  EXPECT_EQ(memory[assembler.m_lastLocationCounter + 3], 0x23);
  EXPECT_EQ(memory[assembler.m_lastLocationCounter + 4], 123);
}

TEST_F(AssemblerTest, testEmitWords) {
  TEST_INST(".word $20ff $23af $fab0 ; test comment");
  EXPECT_EQ(assembler.bytesWritten(), 6);
  EXPECT_EQ(memory.word(assembler.m_lastLocationCounter), 0x20ff);
  EXPECT_EQ(memory.word(assembler.m_lastLocationCounter + 2), 0x23af);
  EXPECT_EQ(memory.word(assembler.m_lastLocationCounter + 4), 0xfab0);
  TEST_INST(".word $3000 $15ad 10230");
  EXPECT_EQ(assembler.bytesWritten(), 12);
  EXPECT_EQ(memory.word(assembler.m_lastLocationCounter), 0x3000);
  EXPECT_EQ(memory.word(assembler.m_lastLocationCounter + 2), 0x15ad);
  EXPECT_EQ(memory.word(assembler.m_lastLocationCounter + 4), 10230);
}

TEST_F(AssemblerTest, testLowerCaseInstruction) {
  TEST_INST("cli");
}

TEST_F(AssemblerTest, testDcb) {
  TEST_INST("dcb 0,0,0,0,0,0,0,0,0,$b,$b,$c,$f,$f,$f,$f");
  EXPECT_EQ(assembler.bytesWritten(), 16);
  EXPECT_EQ(memory[assembler.m_lastLocationCounter], 0);
  EXPECT_EQ(memory[assembler.m_lastLocationCounter + 15], 0xf);
}

TEST_F(AssemblerTest, testLoBytePrefix) {
  TEST_INST_2("LDA #<$1afc", 0xa9, 0xfc);
  symbols.put("label", 0x2afe);
  TEST_INST_2("LDA #<label", 0xa9, 0xfe);
  TEST_INST("dcb <label, 2");
  EXPECT_EQ(memory[assembler.m_lastLocationCounter], 0xfe);
  EXPECT_EQ(memory[assembler.m_lastLocationCounter + 1], 2);
}

TEST_F(AssemblerTest, testHiBytePrefix) {
  TEST_INST_2("LDA #>$1afc", 0xa9, 0x1a);
  symbols.put("label", 0x3afe);
  TEST_INST_2("LDA #>label", 0xa9, 0x3a);
  TEST_INST_2("dcb >label, 2", 0x3a, 2);
}

TEST_F(AssemblerTest, testLoHiBytePrefix) {
  symbols.put("a", 0xfa20);
  symbols.put("b", 0x10a0);
  TEST_INST_2("dcb >a, <b", 0xfa, 0xa0);
  TEST_INST_2("dcb <a, >b", 0x20, 0x10);
}

TEST_F(AssemblerTest, testSymbolDef) {
  assembler.changeMode(Assembler::ProcessingMode::ScanForSymbols);
  TEST_INST(".org $1000");
  TEST_INST("lda init");
  EXPECT_EQ(assembler.m_locationCounter, 0x1003);
}
