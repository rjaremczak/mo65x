#include "emulator.h"
#include <QFile>
#include <QThread>
#include <algorithm>

Emulator::Emulator(QObject* parent) : QObject(parent), cpu(memory) {
  std::generate(memory.begin(), memory.end(), [] { return std::rand(); });
  clearStatistics();
}

void Emulator::loadMemory(uint16_t start, const Data& data) {
  auto size = static_cast<uint16_t>(std::min(static_cast<size_t>(data.size()), memory.size() - start));
  std::copy_n(data.begin(), size, memory.begin() + start);
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
    std::copy_n(&memory[range.first], range.size(), buf.begin());
    rsize = file.write(reinterpret_cast<char*>(buf.data()), static_cast<int>(buf.size()));
  }
  emit operationCompleted(rsize > 0 ? tr("saved %1 B\nto file %2").arg(rsize).arg(fname) : "save error", rsize > 0);
}

void Emulator::clearStatistics() {
  cpu.resetStatistics();
  cpu.resetExecutionState();
  emit stateChanged(state());
}

const EmulatorState Emulator::state(ExecutionStatistics lastRun) {
  const auto info = cpu.info();
  return {info.executionState, info.runLevel, cpu.regs, {cpu.cycles, cpu.duration}, lastRun};
}

void Emulator::triggerIrq() {
  cpu.triggerIrq();
  emit stateChanged(state());
}

void Emulator::triggerNmi() {
  cpu.triggerNmi();
  emit stateChanged(state());
}

void Emulator::triggerReset() {
  cpu.triggerReset();
  emit stateChanged(state());
}

void Emulator::stopExecution() {
  cpu.stopExecution();
  QThread::msleep(100);
  emit stateChanged(state());
}

ExecutionStatistics Emulator::execute(bool continuous) {
  const Duration d0 = cpu.duration;
  const long c0 = cpu.cycles;
  cpu.execute(continuous);
  const Duration d1 = cpu.duration;
  const long c1 = cpu.cycles;
  return {c1 - c0, d1 - d0};
}

void Emulator::executeSingleStep() {
  const auto ex = execute(false);
  emit stateChanged(state(ex));
}

void Emulator::startContinuousExecution() {
  const auto ex = execute(true);
  emit stateChanged(state(ex));
}

void Emulator::changeProgramCounter(uint16_t pc) {
  if (!cpu.running() && cpu.regs.pc != pc) {
    cpu.regs.pc = pc;
    cpu.resetExecutionState();
    emit stateChanged(state());
  }
}

void Emulator::changeStackPointer(uint16_t sp) {
  const auto offset = static_cast<uint8_t>(sp);
  if (cpu.regs.sp.offset != offset) {
    cpu.regs.sp.offset = offset;
    emit stateChanged(state());
  }
}

void Emulator::changeAccumulator(uint8_t a) {
  if (cpu.regs.a != a) {
    cpu.regs.a = a;
    emit stateChanged(state());
  }
}

void Emulator::changeRegisterX(uint8_t x) {
  if (cpu.regs.x != x) {
    cpu.regs.x = x;
    emit stateChanged(state());
  }
}

void Emulator::changeRegisterY(uint8_t y) {
  if (cpu.regs.y != y) {
    cpu.regs.y = y;
    emit stateChanged(state());
  }
}

void Emulator::changeMemory(uint16_t addr, uint8_t b) {
  memory[addr] = b;
  emit memoryContentChanged(addr);
}
