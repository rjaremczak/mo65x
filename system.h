#pragma once

#include "cpu.h"
#include "cpuprobe.h"
#include "memory.h"
#include <QObject>

class System : public QObject {
  Q_OBJECT

  Memory memory_;
  Cpu cpu_;
  CpuProbe cpuProbe_;

public:
  explicit System(QObject* parent = nullptr);
  const Memory& memory() const { return memory_; }
  void fillMemory(uint16_t first, uint16_t last, uint8_t value);
  size_t loadMemory(uint16_t first, const QByteArray& data);
  QByteArray saveMemory(uint16_t first, uint16_t last);

signals:
  void cpuStateChanged(CpuRegisters, CpuFlags);
  void memoryContentChanged(uint16_t first, uint16_t last);

public slots:
  void checkCpuState();
  void changePC(uint16_t);
  void stepExecution();
};
