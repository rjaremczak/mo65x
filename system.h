#pragma once

#include "cpu.h"
#include "memory.h"
#include <QObject>

class System : public QObject {
  Q_OBJECT

  Memory memory;
  Cpu cpu;

public:
  explicit System(QObject* parent = nullptr);
  const Memory& memoryView() const { return memory; }
  ExecutionStatus executionStatus() const { return cpu.executionStatus; }
  void fillMemory(uint16_t first, uint16_t last, uint8_t value);
  size_t loadMemory(uint16_t first, const QByteArray& data);
  QByteArray saveMemory(uint16_t first, uint16_t last);

signals:
  void cpuStateChanged(Registers, int);
  void memoryContentChanged(uint16_t first, uint16_t last);

public slots:
  void checkCpuState();
  void changePC(uint16_t);
  void executeSingleStep();
};
