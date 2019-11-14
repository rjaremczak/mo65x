#include "system.h"
#include <algorithm>

System::System(QObject* parent) : QObject(parent), cpu_(memory_) {
  std::generate(memory_.begin(), memory_.end(), [] { return std::rand(); });
}

void System::fillMemory(quint16 first, quint16 last, uint8_t value) {
  std::fill(memory_.begin() + first, memory_.begin() + last + 1, value);
  emit memoryContentChanged(first, last);
}

void System::uploadToMemory(Address first, const Bytes& data) {
  auto size = static_cast<Address>(std::min(static_cast<size_t>(data.size()), memory_.size() - first));
  std::copy_n(data.begin(), size, memory_.begin() + first);
  emit memoryContentChanged(first, first + size);
}

void System::saveMemory(Address first, Address last, BytesIterator outIt) {
  std::copy(memory_.cbegin() + first, memory_.cbegin() + last + 1, outIt);
}

void System::executeSingleStep() {
  cpu_.execute();
  emit cpuStateChanged(cpu_.info());
}

void System::changeProgramCounter(quint16 pc) {
  if (cpu_.state != ExecutionState::Running && cpu_.regs.pc != pc) {
    cpu_.regs.pc = pc;
    emit cpuStateChanged(cpu_.info());
  }
}

void System::propagateCurrentState() {
  emit cpuStateChanged(cpu_.info());
}
