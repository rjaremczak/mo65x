#include "board.h"

Board::Board(QObject *parent) :
    QObject(parent),
    m_cpu({&Board::cpuBusRead, &Board::cpuBusWrite})
{
}

void Board::checkCpuRegisters()
{
    emit cpuRegistersChanged(m_cpu.registers());
}

uint8_t Board::cpuBusRead(uint16_t addr)
{
    return 0;
}

void Board::cpuBusWrite(uint16_t addr, uint8_t data)
{

}
