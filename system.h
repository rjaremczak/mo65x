#pragma once

#include "cpu.h"
#include "cpuinfo.h"
#include "memory.h"
#include <QObject>

class System : public QObject {
  Q_OBJECT

public:
  explicit System(QObject* parent = nullptr);
  const Memory& memoryView() const { return memory_; }
  void fillMemory(uint16_t first, uint16_t last, uint8_t value);
  QByteArray saveMemory(uint16_t first, uint16_t last);

signals:
  void cpuStateChanged(CpuInfo);
  void memoryContentChanged(uint16_t first, uint16_t last);

public slots:
  void executeSingleStep();
  void changeProgramCounter(uint16_t pc);
  void propagateCurrentState();
  void loadMemory(uint16_t first, const std::vector<uint8_t>& data);

private:
  Memory memory_;
  Cpu cpu_;
};
