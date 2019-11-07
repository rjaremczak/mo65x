#pragma once

#include "cpu.h"
#include "memory.h"
#include <QObject>

class System : public QObject {
  Q_OBJECT

public:
  explicit System(QObject* parent = nullptr);
  const Memory& memory() const { return memory_; }
  const Cpu& cpu() const { return cpu_; }
  void fillMemory(uint16_t first, uint16_t last, uint8_t value);
  size_t loadMemory(uint16_t first, const QByteArray& data);
  QByteArray saveMemory(uint16_t first, uint16_t last);

signals:
  void cpuStateChanged();
  void memoryContentChanged(uint16_t first, uint16_t last);

public slots:
  void changeExecutionAddress(uint16_t);
  void executeSingleStep();

private:
  Memory memory_;
  Cpu cpu_;
};
