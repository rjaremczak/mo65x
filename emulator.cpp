#include "emulator.h"
#include <pthread.h>

Emulator::Emulator() : m_cpu(m_memory), m_thread(&Emulator::loop, this) {
}

Emulator::~Emulator() {
  m_loop.clear();
  m_stateCondition.notify_all();
  if (m_thread.joinable()) m_thread.join();
}

void Emulator::execute(bool continuous, Frequency clock) {
  const auto period = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(1.0 / clock));
  m_lastCpuStatistics.reset();
  m_cpu.preExecute();
  while (m_cpu.running()) {
    const auto t0 = std::chrono::high_resolution_clock::now();
    const auto cycles = m_cpu.execute();
    const auto dt = period * cycles;
    const auto t1 = t0 + dt;
    while (std::chrono::high_resolution_clock::now() < t1) {}

    m_accCpuStatistics.cycles += cycles;
    m_accCpuStatistics.duration += dt;

    m_lastCpuStatistics.cycles += cycles;
    m_lastCpuStatistics.duration += dt;

    if (!continuous) break;
  }
  m_cpu.postExecute();
}

void Emulator::clearStatistics() {
  m_accCpuStatistics.reset();
  m_lastCpuStatistics.reset();
  m_cpu.resetExecutionState();
}

void Emulator::stopExecution() {
  m_cpu.stopExecution();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void Emulator::loop() noexcept {
  std::generate(m_memory.begin(), m_memory.end(), [] { return std::rand(); });
  clearStatistics();
  pthread_setname_np(pthread_self(), "emulator");

  m_loop.test_and_set();
  while (m_loop.test_and_set()) {
    std::unique_lock lock(m_stateMutex);
    m_stateCondition.wait(lock);
  }
}
