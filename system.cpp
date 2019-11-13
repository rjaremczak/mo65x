#include "system.h"
#include <algorithm>

System::System(QObject* parent) : QObject(parent), cpu_(memory_) {
  std::generate(memory_.begin(), memory_.end(), [] { return std::rand(); });
}

void System::fillMemory(uint16_t first, uint16_t last, uint8_t value) {
  std::fill(memory_.begin() + first, memory_.begin() + last + 1, value);
  emit memoryContentChanged(first, last);
}

void System::loadMemory(uint16_t first, const std::vector<uint8_t>& data) {
  auto size = static_cast<uint16_t>(std::min(static_cast<size_t>(data.size()), memory_.size() - first));
  std::copy_n(data.begin(), size, memory_.begin() + first);
  emit memoryContentChanged(first, first + size);
}

QByteArray System::saveMemory(uint16_t first, uint16_t last) {
  QByteArray data;
  std::copy(memory_.cbegin() + first, memory_.cbegin() + last + 1, std::back_inserter(data));
  return data;
}

void System::executeSingleStep() {
  cpu_.execute();
  emit cpuStateChanged(cpu_.info());
}

void System::changeProgramCounter(uint16_t pc) {
  if (cpu_.state != ExecutionState::Running && cpu_.regs.pc != pc) {
    cpu_.regs.pc = pc;
    emit cpuStateChanged(cpu_.info());
  }
}

void System::propagateCurrentState() {
  emit cpuStateChanged(cpu_.info());
}
