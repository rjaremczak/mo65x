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
  enum class State : uint8_t { Idle, StepExecution, ContinuousExecution };

  Emulator();
  ~Emulator();
  const Memory& memoryView() const { return m_memory; }
  Memory& memoryRef() { return m_memory; }
  State state() const { return m_state; }
  void loadMemory(Address first, std::basic_istream<char>& is);
  void saveMemory(AddressRange range, std::basic_ostream<char>& os);
  void execute(bool continuous, Frequency clock);
  void changeProgramCounter(Address);
  void changeStackPointer(Address);
  void changeAccumulator(uint8_t);
  void changeRegisterX(uint8_t);
  void changeRegisterY(uint8_t);
  void changeMemory(Address, uint8_t);
  void triggerIrq();
  void triggerNmi();
  void triggerReset();
  void clearStatistics();
  void stopExecution();

private:
  void loop() noexcept;

  State m_state = State::Idle;
  Frequency m_frequency;
  Memory m_memory;
  Cpu m_cpu;
  CpuStatistics m_accCpuStatistics;
  CpuStatistics m_lastCpuStatistics;
  std::atomic_flag m_loop;
  std::mutex m_stateMutex;
  std::condition_variable m_stateCondition;
  std::thread m_thread;
};
