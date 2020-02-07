#pragma once

#include "addressrange.h"
#include "cpu.h"
#include "cpustatistics.h"
#include "emulatorstate.h"
#include "memory.h"
#include <istream>
#include <thread>

class Emulator
{
public:
  Emulator();
  ~Emulator();
  const Memory& memoryView() const { return m_memory; }
  Memory& memoryRef() { return m_memory; }
  EmulatorState state() const;
  void loadMemory(Address first, std::basic_istream<char>& is);
  void saveMemory(AddressRange range, std::basic_ostream<char>& os);
  void execute(bool continuous, Frequency clock);
  void changeProgramCounter(Address);
  void changeStackPointer(Address);
  void changeAccumulator(uint8_t);
  void changeRegisterX(uint8_t);
  void changeRegisterY(uint8_t);
  void changeMemory(Address, uint8_t);

  // to be connected as direct connections

  void triggerIrq();
  void triggerNmi();
  void triggerReset();
  void stopExecution();
  void clearStatistics();

private:
  std::thread m_thread;
  Memory m_memory;
  Cpu m_cpu;
  CpuStatistics m_accCpuStatistics;
  CpuStatistics m_lastCpuStatistics;
};
