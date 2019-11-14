#include "system.h"
#include <algorithm>

System::System(QObject* parent) : QObject(parent), cpu_(memory_) {
  std::generate(memory_.begin(), memory_.end(), [] { return std::rand(); });
}

void System::uploadToMemory(quint16 start, const Data& data) {
  auto size = static_cast<quint16>(std::min(static_cast<size_t>(data.size()), memory_.size() - start));
  std::copy_n(data.begin(), size, memory_.begin() + start);
  emit memoryContentChanged(start, start + size);
}

void System::resetCycleCounter() {
  cpu_.cycles = 0;
}

void System::requestIRQ() {
  cpu_.requestIRQ();
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
