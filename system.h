#pragma once

#include "commondefs.h"
#include "cpu.h"
#include "cpuinfo.h"
#include "memory.h"
#include <QObject>

class System : public QObject {
  Q_OBJECT

public:
  explicit System(QObject* parent = nullptr);
  const Memory& memoryView() const { return memory_; }

signals:
  void cpuStateChanged(CpuInfo);
  void memoryContentChanged(uint16_t first, uint16_t last);

public slots:
  void executeSingleStep();
  void changeProgramCounter(uint16_t pc);
  void propagateCurrentState();
  void uploadToMemory(Address first, const Bytes& data);
  void saveMemory(Address first, Address last, BytesIterator outIt);
  void fillMemory(Address first, Address last, Byte value);

private:
  Memory memory_;
  Cpu cpu_;
};
