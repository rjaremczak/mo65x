#include "cpu.h"

Cpu::Cpu()
{
    m_registers.flags.v = false;
    m_registers.flags.z = true;
}
