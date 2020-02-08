#include "emulator.h"
#include <pthread.h>

Emulator::Emulator() : m_cpu(m_memory), m_thread(&Emulator::loop, this) {
  const auto str = "emulator";
#ifdef __APPLE__
  pthread_setname_np(str);
#else
  pthread_setname_np(pthread_self(), str);
#endif
}

Emulator::~Emulator() {
  m_loop.clear();
  m_runningCondition.notify_all();
  if (m_thread.joinable()) m_thread.join();
}

void Emulator::execute(bool continuous, Frequency clock) {
  std::unique_lock lock(m_runningMutex, std::try_to_lock);
  if (lock.owns_lock() && !m_running) {
    m_running = true;
    m_continuous = continuous;
    m_period = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(1.0 / clock));
    lock.unlock();
    m_runningCondition.notify_all();
  }
}

void Emulator::clearStatistics() {
  m_accCpuStatistics.reset();
  m_lastCpuStatistics.reset();
  m_cpu.resetExecutionState();
}

bool Emulator::stopExecution() {
  m_cpu.stopExecution();
  std::lock_guard lock(m_runningMutex);
  return !m_running && !m_cpu.running();
}

void Emulator::loop() noexcept {
  std::generate(m_memory.begin(), m_memory.end(), [] { return std::rand(); });
  clearStatistics();
  m_loop.test_and_set();
  while (m_loop.test_and_set()) {
    std::unique_lock lock(m_runningMutex);
    m_runningCondition.wait(lock);
    if (m_running) {
      m_lastCpuStatistics.reset();
      m_cpu.preRun();
      while (m_cpu.running()) {
        const auto t0 = std::chrono::high_resolution_clock::now();
        const auto cycles = m_cpu.stepRun();
        const auto dt = m_period * cycles;
        const auto t1 = t0 + dt;
        while (std::chrono::high_resolution_clock::now() < t1) {}

        m_accCpuStatistics.cycles += cycles;
        m_accCpuStatistics.duration += dt;

        m_lastCpuStatistics.cycles += cycles;
        m_lastCpuStatistics.duration += dt;

        if (!m_continuous) break;
      }
      m_cpu.postRun();
      m_running = false;
    }
  }
}
