#pragma once

#include "addressrange.h"
#include "commons.h"
#include "cpu.h"
#include "emulatorinfo.h"
#include "memory.h"
#include <QObject>

class Emulator : public QObject {
  Q_OBJECT

public:
  explicit Emulator(QObject* parent = nullptr);
  const Memory& memoryView() const { return memory_; }

signals:
  void cpuStateChanged(EmulatorInfo);
  void memoryContentChanged(AddressRange);
  void operationCompleted(const QString& message, bool success);

public slots:
  void executeOneInstruction();
  void startExecution();
  void changeProgramCounter(uint16_t);
  void changeStackPointer(uint16_t);
  void changeAccumulator(uint8_t);
  void changeRegisterX(uint8_t);
  void changeRegisterY(uint8_t);
  void changeMemory(uint16_t, uint8_t);
  void notifyStateChanged();
  void loadMemory(uint16_t first, const Data& data);
  void loadMemoryFromFile(uint16_t start, const QString& fname);
  void saveMemoryToFile(AddressRange range, const QString& fname);

  // to be connected as direct connections

  void triggerIrq();
  void triggerNmi();
  void triggerReset();
  void stopExecution();
  void resetCycleCounter();

private:
  Memory memory_;
  Cpu cpu_;

  void notifyCpuStateChanged();
};
