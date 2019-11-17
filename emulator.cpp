#include "emulator.h"
#include <QFile>
#include <algorithm>

Emulator::Emulator(QObject* parent) : QObject(parent), cpu_(memory_) {
  std::generate(memory_.begin(), memory_.end(), [] { return std::rand(); });
}

void Emulator::loadMemory(uint16_t start, const Data& data) {
  auto size = static_cast<uint16_t>(std::min(static_cast<size_t>(data.size()), memory_.size() - start));
  std::copy_n(data.begin(), size, memory_.begin() + start);
  emit memoryContentChanged({start, static_cast<uint16_t>(start + size)});
}

void Emulator::loadMemoryFromFile(uint16_t start, const QString& fname, std::function<void(qint64)> callback) {
  QFile file(fname);
  qint64 rsize = -1;
  if (file.open(QIODevice::ReadOnly)) {
    const auto size = std::min(static_cast<qint64>(Memory::Size - start), file.size());
    Data buf(static_cast<size_t>(size));
    rsize = file.read(reinterpret_cast<char*>(buf.data()), size);
    if (rsize == size) loadMemory(start, buf);
  }
  callback(rsize);
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
