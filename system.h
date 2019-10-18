#pragma once

#include <QObject>
#include "cpu.h"
#include "memory.h"

class System : public QObject
{
    Q_OBJECT

    Memory m_memory;
    Cpu m_cpu;

public:
    explicit System(QObject *parent = nullptr);
    const Memory& memory() const { return m_memory; }
    const Cpu& cpu() const { return m_cpu; }

    void fillMemory(size_t start, size_t size, uint8_t value);
    void loadMemory(size_t start, const QByteArray& data);
    QByteArray saveMemory(size_t start, size_t size);

signals:
    void cpuStateChanged(CpuState);
    void memoryContentChanged(size_t start, size_t size);

public slots:
    void checkCpuState();
    void changePC(uint16_t);
};
