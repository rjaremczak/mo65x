#pragma once

#include <functional>
#include <chrono>
#include "cpuregisters.h"

class Board;

class Cpu
{
public:
    struct BoardConnector
    {
        uint8_t(Board::*read)(uint16_t);
        void(Board::*write)(uint16_t, uint8_t);
    };

private:
    const BoardConnector m_board;
    CpuRegisters m_registers;

public:
    Cpu(const BoardConnector);

    CpuRegisters registers() const { return m_registers; }
};
