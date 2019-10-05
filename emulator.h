#pragma once

#include <QObject>

#include "cpu.h"

class Emulator : public QObject
{
    Q_OBJECT

public:
    explicit Emulator(QObject *parent = nullptr);

signals:
    void cpuStateChanged(CpuRegisters);

public slots:
    void checkCpuState();

private:
    Cpu m_cpu;
};
