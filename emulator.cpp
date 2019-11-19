#include "emulator.h"
#include <QFile>
#include <QThread>
#include <algorithm>

Emulator::Emulator(QObject* parent) : QObject(parent), cpu_(memory_) {
  std::generate(memory_.begin(), memory_.end(), [] { return std::rand(); });
}

void Emulator::loadMemory(uint16_t start, const Data& data) {
  auto size = static_cast<uint16_t>(std::min(static_cast<size_t>(data.size()), memory_.size() - start));
  std::copy_n(data.begin(), size, memory_.begin() + start);
  emit memoryContentChanged({start, static_cast<uint16_t>(start + size - 1)});
}

void Emulator::loadMemoryFromFile(uint16_t start, const QString& fname) {
  QFile file(fname);
  qint64 rsize = -1;
  if (file.open(QIODevice::ReadOnly)) {
    const auto size = std::min(static_cast<qint64>(Memory::Size - start), file.size());
    Data buf(static_cast<size_t>(size));
    rsize = file.read(reinterpret_cast<char*>(buf.data()), size);
    if (rsize == size) loadMemory(start, buf);
  }
  emit operationCompleted(rsize > 0 ? tr("loaded %1 B\nfrom file %2").arg(rsize).arg(fname) : "load error", rsize > 0);
}

void Emulator::saveMemoryToFile(AddressRange range, const QString& fname) {
  QFile file(fname);
  qint64 rsize = -1;
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    Data buf(range.size());
    std::copy_n(&memory_[range.first], range.size(), buf.begin());
    rsize = file.write(reinterpret_cast<char*>(buf.data()), static_cast<int>(buf.size()));
  }
  emit operationCompleted(rsize > 0 ? tr("saved %1 B\nto file %2").arg(rsize).arg(fname) : "save error", rsize > 0);
}

void Emulator::resetCycleCounter() {
  cpu_.cycles = 0;
  cpu_.duration = std::chrono::microseconds::zero();
}

void Emulator::notifyCpuStateChanged() {
  emit cpuStateChanged({cpu_.state, cpu_.regs, cpu_.cycles, cpu_.duration});
}

void Emulator::triggerIrq() {
  cpu_.triggerIrq();
}

void Emulator::triggerNmi() {
  cpu_.triggerNmi();
}

void Emulator::triggerReset() {
  cpu_.triggerReset();
}

void Emulator::stopExecution() {
  cpu_.state = ExecutionState::Stopping;
  QThread::msleep(100);
}

void Emulator::executeOneInstruction() {
  cpu_.execute(false);
  notifyCpuStateChanged();
}

void Emulator::startExecution() {
  cpu_.execute(true);
  notifyCpuStateChanged();
}

void Emulator::changeProgramCounter(uint16_t pc) {
  if (cpu_.state != ExecutionState::Running && cpu_.regs.pc != pc) {
    cpu_.regs.pc = pc;
    if (cpu_.state == ExecutionState::Halted || cpu_.state == ExecutionState::Stopped) cpu_.state = ExecutionState::Idle;
    notifyCpuStateChanged();
  }
}

void Emulator::changeStackPointer(uint16_t sp) {
  const auto offset = static_cast<uint8_t>(sp);
  if (cpu_.regs.sp.offset != offset) {
    cpu_.regs.sp.offset = offset;
    notifyCpuStateChanged();
  }
}

void Emulator::changeAccumulator(uint8_t a) {
  if (cpu_.regs.a != a) {
    cpu_.regs.a = a;
    notifyCpuStateChanged();
  }
}

void Emulator::changeRegisterX(uint8_t x) {
  if (cpu_.regs.x != x) {
    cpu_.regs.x = x;
    notifyCpuStateChanged();
  }
}

void Emulator::changeRegisterY(uint8_t y) {
  if (cpu_.regs.y != y) {
    cpu_.regs.y = y;
    notifyCpuStateChanged();
  }
}

void Emulator::changeMemory(uint16_t addr, uint8_t b) {
  memory_[addr] = b;
  emit memoryContentChanged(addr);
}

void Emulator::notifyStateChanged() {
  notifyCpuStateChanged();
}
