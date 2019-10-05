#include "emulator.h"

Emulator::Emulator(QObject *parent) :
    QObject(parent),
    m_cpu(&Emulator::cpuBusRead, &Emulator::cpuBusWrite)
{

}

void Emulator::checkCpuRegisters()
{
    emit cpuRegistersChanged(m_cpu.registers());
}

uint8_t Emulator::cpuBusRead(uint16_t addr)
{
    return 0;
}

void Emulator::cpuBusWrite(uint16_t addr, uint8_t data)
{

}
