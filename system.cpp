#include "system.h"
#include <algorithm>

System::System(QObject* parent) : QObject(parent), cpu(memory) {
  std::generate(memory.begin(), memory.end(), [] { return std::rand(); });
}

void System::fillMemory(uint16_t first, uint16_t last, uint8_t value) {
  std::fill(memory.begin() + first, memory.begin() + last + 1, value);
  emit memoryContentChanged(first, last);
}

size_t System::loadMemory(uint16_t first, const QByteArray& data) {
  auto size = static_cast<uint16_t>(std::min(static_cast<size_t>(data.size()), memory.size() - first));
  std::copy_n(data.begin(), size, memory.begin() + first);
  emit memoryContentChanged(first, first + size);
  return size;
}

QByteArray System::saveMemory(uint16_t first, uint16_t last) {
  QByteArray data;
  std::copy(memory.cbegin() + first, memory.cbegin() + last + 1, std::back_inserter(data));
  return data;
}

void System::checkCpuState() {
  emit cpuStateChanged(cpu.registers, cpu.flags);
}

void System::changePC(uint16_t pc) {
  if (cpu.registers.pc != pc) {
    cpu.registers.pc = pc;
    emit cpuStateChanged(cpu.registers, cpu.flags);
  }
}

void System::stepExecution() {
  cpu.execute();
}
