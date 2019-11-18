#pragma once

#include "commons.h"
#include "cpu.h"
#include "cpuinfo.h"
#include "memory.h"
#include <QObject>

class Emulator : public QObject {
  Q_OBJECT

public:
  explicit Emulator(QObject* parent = nullptr);
  const Memory& memoryView() const { return memory_; }
  void triggerIrq();
  void triggerNmi();
  void triggerReset();
  bool stopExecution();
  void resetCycleCounter();

signals:
  void cpuStateChanged(CpuInfo);
  void memoryContentChanged(AddressRange);
  void operationCompleted(const QString& message, bool success);

public slots:
  void executeOneInstruction();
  void startExecution();
  void changeProgramCounter(uint16_t pc);
  void propagateCurrentState();
  void loadMemory(uint16_t first, const Data& data);
  void loadMemoryFromFile(uint16_t start, const QString& fname);
  void saveMemoryToFile(AddressRange range, const QString& fname);

private:
  Memory memory_;
  Cpu cpu_;
};
