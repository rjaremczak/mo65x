#pragma once

#include <QObject>
#include "cpu.h"

class Emulator : public QObject
{
    Q_OBJECT

public:
    explicit Emulator(QObject *parent = nullptr);

signals:
    void cpuRegistersChanged(CpuRegisters);

public slots:
    void checkCpuRegisters();

private:
    Cpu m_cpu;


    uint8_t cpuBusRead(uint16_t addr);
    void cpuBusWrite(uint16_t addr, uint8_t data);
};
