#include "emulator.h"

Emulator::Emulator(QObject *parent) : QObject(parent)
{

}

void Emulator::checkCpuState()
{
    emit cpuStateChanged(m_cpu.state());
}
