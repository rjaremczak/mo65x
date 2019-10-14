#pragma once

#include <QObject>
#include "cpu.h"
#include "memory.h"

class Emulator : public QObject
{
    Q_OBJECT

    Memory m_memory;
    Cpu m_cpu;

public:
    explicit Emulator(QObject *parent = nullptr);

signals:
    void cpuRegistersChanged(CpuRegisters);

public slots:
    void checkCpuRegisters();

};
