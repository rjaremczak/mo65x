#pragma once

#include "assembler.h"
#include "cpu.h"
#include <QObject>

class OpCodesTest : public QObject{
  Q_OBJECT

public:
  explicit OpCodesTest(QObject* parent = nullptr);

  Memory memory;
  Cpu cpu;
  Assembler assembler;

signals:

public slots:

private:
private slots:
  // functions executed by QtTest before and after test suite
  void initTestCase();
  // void cleanupTestCase();

  // functions executed by QtTest before and after each test
  void init();
  // void cleanup();

  void testAccumulatorMode();
  void testImmediateMode();
  void testZeroPageMode();
};
