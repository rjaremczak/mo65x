#include "emulator.h"
#include <pthread.h>

Emulator::Emulator() : m_cpu(m_memory), m_thread(&Emulator::loop, this) {
  const auto str = "emulator";
//  std::generate(m_memory.begin(), m_memory.end(), [] { return std::rand(); });
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

void Emulator::readMemory(AddressRange range, Data& buf) const {
  std::copy_n(&m_memory[range.first], range.size(), buf.begin());
}

void Emulator::writeMemory(AddressRange range, const Data& buf) {
  std::copy_n(buf.begin(), range.size(), &m_memory[range.first]);
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

void Emulator::setRegisterPC(Address pc) {
  std::unique_lock lock(m_runningMutex, std::try_to_lock);
  if (lock.owns_lock() && !m_running) {
    m_cpu.regs.pc = pc;
    m_cpu.resetExecutionState();
  }
}

void Emulator::setRegisterSP(Address sp) {
  std::unique_lock lock(m_runningMutex, std::try_to_lock);
  if (lock.owns_lock() && !m_running) { m_cpu.regs.sp.offset = static_cast<uint8_t>(sp); }
}

void Emulator::setRegisterA(uint8_t a) {
  std::unique_lock lock(m_runningMutex, std::try_to_lock);
  if (lock.owns_lock() && !m_running) { m_cpu.regs.a = a; }
}

void Emulator::setRegisterX(uint8_t x) {
  std::unique_lock lock(m_runningMutex, std::try_to_lock);
  if (lock.owns_lock() && !m_running) { m_cpu.regs.x = x; }
}

void Emulator::setRegisterY(uint8_t y) {
  std::unique_lock lock(m_runningMutex, std::try_to_lock);
  if (lock.owns_lock() && !m_running) { m_cpu.regs.a = y; }
}

void Emulator::triggerIrq() {
  m_cpu.triggerIrq();
}

void Emulator::triggerNmi() {
  m_cpu.triggerNmi();
}

void Emulator::triggerReset() {
  m_cpu.triggerReset();
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

void Emulator::waitForStop() {
  while (m_running) { std::lock_guard lock(m_runningMutex); }
}

void Emulator::loop() noexcept {
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
