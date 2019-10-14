#include "emulator.h"

Emulator::Emulator(QObject *parent) : QObject(parent),
    m_memory(),
    m_cpu(m_memory)
{
}

void Emulator::checkCpuRegisters()
{
    emit cpuRegistersChanged(m_cpu.registers());
}
