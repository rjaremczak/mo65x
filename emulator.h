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
    const Memory& memory() const { return m_memory; }
    void fillMemory(size_t start, size_t size, uint8_t value);
    void loadMemory(size_t start, const QByteArray& data);
    QByteArray saveMemory(size_t start, size_t size);

signals:
    void cpuRegistersChanged(CpuRegisters);
    void memoryContentChanged(size_t start, size_t size);

public slots:
    void checkCpuRegisters();
};
