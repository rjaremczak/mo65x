#include "emulatorqt.h"
#include <QFile>
#include <QThread>
#include <algorithm>

EmulatorQt::EmulatorQt(QObject* parent) : QObject(parent), m_cpu(m_memory) {
  std::generate(m_memory.begin(), m_memory.end(), [] { return std::rand(); });
  clearStatistics();
}

void EmulatorQt::loadMemory(Address start, const Data& data) {
  auto size = static_cast<uint16_t>(std::min(static_cast<size_t>(data.size()), m_memory.size() - start));
  std::copy_n(data.begin(), size, m_memory.begin() + start);
  emit memoryContentChanged({start, static_cast<Address>(start + size - 1)});
}

void EmulatorQt::loadMemoryFromFile(uint16_t start, const QString& fname) {
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

void EmulatorQt::saveMemoryToFile(AddressRange range, const QString& fname) {
  QFile file(fname);
  qint64 rsize = -1;
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    Data buf(range.size());
    std::copy_n(&m_memory[range.first], range.size(), buf.begin());
    rsize = file.write(reinterpret_cast<char*>(buf.data()), static_cast<int>(buf.size()));
  }
  emit operationCompleted(rsize > 0 ? tr("saved %1 B\nto file %2").arg(rsize).arg(fname) : "save error", rsize > 0);
}

void EmulatorQt::clearStatistics() {
  m_accCpuStatistics.reset();
  m_lastCpuStatistics.reset();
  m_cpu.resetExecutionState();
}

EmulatorState EmulatorQt::state() const {
  return {m_cpu.state(), m_cpu.regs, m_accCpuStatistics, m_lastCpuStatistics};
}

void EmulatorQt::triggerIrq() {
  m_cpu.triggerIrq();
}

void EmulatorQt::triggerNmi() {
  m_cpu.triggerNmi();
}

void EmulatorQt::triggerReset() {
  m_cpu.triggerReset();
}

void EmulatorQt::stopExecution() {
  m_cpu.stopExecution();
  QThread::msleep(100);
}

void EmulatorQt::execute(bool continuous, Frequency clock) {
  QSignalBlocker sb(this);

  const auto period = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(1.0 / clock));
  m_lastCpuStatistics.reset();
  m_cpu.preRun();
  while (m_cpu.running()) {
    const auto t0 = std::chrono::high_resolution_clock::now();
    const auto cycles = m_cpu.stepRun();
    const auto dt = period * cycles;
    const auto t1 = t0 + dt;
    while (std::chrono::high_resolution_clock::now() < t1) {}

    m_accCpuStatistics.cycles += cycles;
    m_accCpuStatistics.duration += dt;

    m_lastCpuStatistics.cycles += cycles;
    m_lastCpuStatistics.duration += dt;

    if (!continuous) break;
  }
  m_cpu.postRun();
  sb.unblock();
  emit stateChanged(state());
  emit memoryContentChanged(AddressRange::Max);
}

void EmulatorQt::changeProgramCounter(Address pc) {
  if (!m_cpu.running() && m_cpu.regs.pc != pc) {
    m_cpu.regs.pc = pc;
    m_cpu.resetExecutionState();
    emit stateChanged(state());
  }
}

void EmulatorQt::changeStackPointer(Address sp) {
  const auto offset = static_cast<uint8_t>(sp);
  if (m_cpu.regs.sp.offset != offset) {
    m_cpu.regs.sp.offset = offset;
    emit stateChanged(state());
  }
}

void EmulatorQt::changeAccumulator(uint8_t a) {
  if (m_cpu.regs.a != a) {
    m_cpu.regs.a = a;
    emit stateChanged(state());
  }
}

void EmulatorQt::changeRegisterX(uint8_t x) {
  if (m_cpu.regs.x != x) {
    m_cpu.regs.x = x;
    emit stateChanged(state());
  }
}

void EmulatorQt::changeRegisterY(uint8_t y) {
  if (m_cpu.regs.y != y) {
    m_cpu.regs.y = y;
    emit stateChanged(state());
  }
}

void EmulatorQt::changeMemory(Address addr, uint8_t b) {
  m_memory[addr] = b;
  emit memoryContentChanged(addr);
}
