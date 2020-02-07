#pragma once

#include "addressrange.h"
#include "commondefs.h"
#include "cpu.h"
#include "emulatorstate.h"
#include "memory.h"
#include <QObject>

class Emulator : public QObject {
  Q_OBJECT

public:
  explicit Emulator(QObject* parent = nullptr);
  const Memory& memoryView() const { return m_memory; }
  Memory& memoryRef() { return m_memory; }
  EmulatorState state() const;

signals:
  void stateChanged(EmulatorState);
  void memoryContentChanged(AddressRange);
  void operationCompleted(const QString& message, bool success);

public slots:
  void execute(bool continuous, Frequency clock);
  void changeProgramCounter(Address);
  void changeStackPointer(Address);
  void changeAccumulator(uint8_t);
  void changeRegisterX(uint8_t);
  void changeRegisterY(uint8_t);
  void changeMemory(Address, uint8_t);
  void loadMemory(Address first, const Data& data);
  void loadMemoryFromFile(Address start, const QString& fname);
  void saveMemoryToFile(AddressRange range, const QString& fname);

  // to be connected as direct connections

  void triggerIrq();
  void triggerNmi();
  void triggerReset();
  void stopExecution();
  void clearStatistics();

private:
  Memory m_memory;
  Cpu m_cpu;
  CpuStatistics m_accCpuStatistics;
  CpuStatistics m_lastCpuStatistics;
};
