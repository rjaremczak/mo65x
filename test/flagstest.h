#pragma once

#include "processorstatus.h"
#include <QObject>

class FlagsTest : public QObject
{
  Q_OBJECT
public:
  explicit FlagsTest(QObject *parent = nullptr);

signals:

public slots:

private:
  ProcessorStatus p;

private slots:
  void init();
  void testBits();
  void testNegative();
  void testZero();
  void testCarry();
  void testOverflow();
};
