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
  void requestIrq();
  void requestNmi();
  void requestReset();

signals:
  void cpuStateChanged(CpuInfo);
  void memoryContentChanged(AddressRange);
  void memoryLoaded(quint16, qint64);

public slots:
  void executeSingleStep();
  void changeProgramCounter(uint16_t pc);
  void propagateCurrentState();
  void loadMemory(uint16_t first, const Data& data);
  void loadMemoryFromFile(uint16_t start, const QString& fname);
  void resetCycleCounter();

private:
  Memory memory_;
  Cpu cpu_;
};
