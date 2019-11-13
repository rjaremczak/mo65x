#pragma once

#include "assembler.h"
#include <QObject>

static constexpr auto AsmOrigin = 0x0800;

class AssemblerTest : public QObject {
  Q_OBJECT

public:
  Assembler assembler;
  const Assembler::MachineCode& buffer;

  explicit AssemblerTest(QObject* parent = nullptr);

private:
  void verify(const char* str, uint8_t opcode, int lo = -1, int hi = -1);

private slots:
  // functions executed by QtTest before and after test suite
  // void initTestCase();
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
  void testRelativeModePlus();
  void testRelativeModeMinus();
  void testRelativeModeLabel();
  void testOrg();
  void testComment();
  void testEmptyLineLabel();
  void testSymbolPass();
  void testAssemblyPass();
};
