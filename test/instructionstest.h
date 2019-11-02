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
  void test(const char*);
  void test(InstructionType type, AddressingMode mode = NoOperands, int operand = 0);

  // functions executed by QtTest before and after test suite
  void initTestCase();
  // void cleanupTestCase();

  // functions executed by QtTest before and after each test
  void init();
  // void cleanup();

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
  // void testRelativeModePlus();
  // void testRelativeModeMinus();

  void testASL();
};
