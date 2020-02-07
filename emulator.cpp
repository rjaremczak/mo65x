#include "emulator.h"

Emulator::Emulator() : m_cpu(m_memory) {
  std::generate(m_memory.begin(), m_memory.end(), [] { return std::rand(); });
  // clearStatistics();
}

Emulator::~Emulator() {
  if (m_thread.joinable()) {
    printf("stopping execution...");
    stopExecution();
    m_thread.join();
  }
}

void Emulator::stopExecution() {
  m_cpu.stopExecution();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
