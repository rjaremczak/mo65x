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

public slots:
  void executeSingleStep();
  void changeProgramCounter(uint16_t pc);
  void propagateCurrentState();
  void uploadToMemory(uint16_t first, const Data& data);
  void resetCycleCounter();

private:
  Memory memory_;
  Cpu cpu_;
};
