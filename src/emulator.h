#pragma once

#include "addressrange.h"
#include "cpu.h"
#include "cpustatistics.h"
#include "emulatorstate.h"
#include "memory.h"
#include <condition_variable>
#include <istream>
#include <mutex>
#include <thread>

class Emulator
{
public:
  Emulator();
  ~Emulator();
  const Memory& memoryView() const { return m_memory; }
  Memory& memoryRef() { return m_memory; }
  bool running() const { return m_running; }
  EmulatorState state() const { return {m_cpu.state(), m_cpu.regs, m_accCpuStatistics, m_lastCpuStatistics}; }

  void readMemory(AddressRange range, Data& buf) const;
  void writeMemory(AddressRange range, const Data& buf);
  void execute(bool continuous, Frequency clock);
  void setRegisterPC(Address);
  void setRegisterSP(Address);
  void setRegisterA(uint8_t);
  void setRegisterX(uint8_t);
  void setRegisterY(uint8_t);
  void triggerIrq();
  void triggerNmi();
  void triggerReset();
  void clearStatistics();
  bool stopExecution();
  void waitForStop();

private:
  void loop() noexcept;

  Memory m_memory;
  Cpu m_cpu;
  CpuStatistics m_accCpuStatistics;
  CpuStatistics m_lastCpuStatistics;
  bool m_running = false;
  bool m_continuous = false;
  std::chrono::nanoseconds m_period;
  std::atomic_flag m_loop;
  std::mutex m_runningMutex;
  std::condition_variable m_runningCondition;
  std::thread m_thread;
};
