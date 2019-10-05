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
};
