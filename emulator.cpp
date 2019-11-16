#include "emulator.h"
#include <algorithm>

Emulator::Emulator(QObject* parent) : QObject(parent), cpu_(memory_) {
  std::generate(memory_.begin(), memory_.end(), [] { return std::rand(); });
}

void Emulator::uploadToMemory(quint16 start, const Data& data) {
  auto size = static_cast<quint16>(std::min(static_cast<size_t>(data.size()), memory_.size() - start));
  std::copy_n(data.begin(), size, memory_.begin() + start);
  emit memoryContentChanged(start, start + size);
}

void Emulator::resetCycleCounter() {
  cpu_.cycles = 0;
}

void Emulator::requestIrq() {
  cpu_.requestIRQ();
}

void Emulator::executeSingleStep() {
  cpu_.execute();
  emit cpuStateChanged(cpu_.info());
}

void Emulator::changeProgramCounter(uint16_t pc) {
  if (cpu_.state != ExecutionState::Running && cpu_.regs.pc != pc) {
    cpu_.regs.pc = pc;
    emit cpuStateChanged(cpu_.info());
  }
}

void Emulator::propagateCurrentState() {
  emit cpuStateChanged(cpu_.info());
}
