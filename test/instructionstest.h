#pragma once

#include "assembler.h"
#include "cpu.h"
#include <QObject>

class OpCodesTest : public QObject{
  Q_OBJECT

public:
  explicit OpCodesTest(QObject* parent = nullptr);

private:
  Memory memory;
  Cpu cpu;
  Assembler assembler;

private slots:
  // functions executed by QtTest before and after test suite
  void initTestCase();
  // void cleanupTestCase();

  // functions executed by QtTest before and after each test
  void init();
  // void cleanup();

  void testReset();
  void testImpliedMode();
  void testAccumulatorMode();
  void testImmediateMode();
  void testZeroPageMode();
  void testZeroPageXMode();
  void testZeroPageYMode();
  void testAbsoluteMode();
  void testAbsoluteXMode();
  void testAbsoluteYMode();
  void testIndirectMode();
  void testIndexedIndirectXMode();
  void testIndirectIndexedYMode();
  void testPageBoundaryCrossingDetection();
  void testBranchForward();
  void testBranchBackward();
  void testBranchWithPageBoundaryCrossed();

  void testADC();
  void testSBC();
  void testAND();
  void testEOR();
  void testASL();
  void testLSR();
  void testORA();
  void testROL();
  void testROR();
  void testDEC();
  void testDEX();
  void testDEY();
  void testINC();
  void testINX();
  void testINY();

  void testLDA();
  void testLDX();
  void testLDY();
  void testSTA();
  void testSTX();
  void testSTY();
  void testTXA();
  void testTAX();
  void testTYA();
  void testTAY();
  void testTSX();
  void testTXS();

  void testBCC_taken();
  void testBCC_notTaken();
  void testBCS_taken();
  void testBCS_notTaken();
  void testBEQ_taken();
  void testBEQ_notTaken();
  void testBNE_taken();
  void testBNE_notTaken();
  void testBMI_taken();
  void testBMI_notTaken();
  void testBPL_taken();
  void testBPL_notTaken();
  void testBVC_taken();
  void testBVC_notTaken();
  void testBVS_taken();
  void testBVS_notTaken();

  void testBIT();
  void testCMP();
  void testCPX();
  void testCPY();

  void testCLC();
  void testSEC();
  void testCLD();
  void testSED();
  void testCLI();
  void testSEI();
};
