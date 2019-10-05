#include "cpu.h"


Cpu::Cpu(const Cpu::BusRead busRead, const Cpu::BusWrite busWrite) :
    m_busRead(busRead), m_busWrite(busWrite)
{

}
