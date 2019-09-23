#include "cpu.h"

Cpu::Cpu(QObject *parent) : QObject(parent)
{

}

void Cpu::checkState()
{
    m_state.regPC++;
    emit stateChanged(m_state);
}
