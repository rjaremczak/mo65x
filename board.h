#pragma once

#include <QObject>
#include "cpu.h"
#include "memoryblock.h"

class Board : public QObject
{
    Q_OBJECT

public:
    explicit Board(QObject *parent = nullptr);
signals:
    void cpuRegistersChanged(CpuRegisters);

public slots:
    void checkCpuRegisters();

private:
    MemoryBlock<0x0000, 0xffff> m_ram;
    Cpu m_cpu;

    uint8_t cpuBusRead(uint16_t addr);
    void cpuBusWrite(uint16_t addr, uint8_t data);
};
