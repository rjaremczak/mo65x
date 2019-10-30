#pragma once

#include "cpu.h"
#include <QObject>

class OpCodesTest : public QObject
{
  Q_OBJECT

public:
  explicit OpCodesTest(QObject *parent = nullptr);

signals:

public slots:

private:
  Memory memory_;
  Cpu cpu_;

private slots:
  // functions executed by QtTest before and after test suite
  void initTestCase();
  // void cleanupTestCase();

  // functions executed by QtTest before and after each test
  void init();
  // void cleanup();

  void testImmediateAddressingMode();
};
