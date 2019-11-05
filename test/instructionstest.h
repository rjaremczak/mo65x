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
  void exec(const char*, int cycles = -1);
  void verifyANZCV(uint8_t acc, bool n, bool z, bool c, bool v);

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
  void testAND();
  void testASL();
  void testBCC();
  void testBCCNotTaken();
};
