#include "emulator.h"

Emulator::Emulator(QObject *parent) : QObject(parent)
{

}

void Emulator::checkCpuRegisters()
{
    emit cpuRegistersChanged(m_cpu.registers());
}
