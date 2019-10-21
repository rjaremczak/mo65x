#include "cpu.h"

void Cpu::amImplied()
{

}

void Cpu::amAccumulator()
{

}

void Cpu::amRelative()
{

}

void Cpu::amImmediate()
{

}

void Cpu::amZeroPage()
{

}

void Cpu::amZeroPageX()
{

}

void Cpu::amZeroPageY()
{

}

void Cpu::amIndexedIndirectX()
{

}

void Cpu::amIndirectIndexedY()
{

}

void Cpu::amIndirect()
{

}

void Cpu::amAbsolute()
{

}

void Cpu::amAbsoluteX()
{

}

void Cpu::amAbsoluteY()
{

}

void Cpu::insLDA()
{

}

void Cpu::insLDX()
{

}

void Cpu::insLDY()
{

}

void Cpu::insSTA()
{

}

void Cpu::insSTX()
{

}

void Cpu::insSTY()
{

}

void Cpu::insADC()
{

}

void Cpu::insSBC()
{

}

void Cpu::insINC()
{

}

void Cpu::insINX()
{

}

void Cpu::insINY()
{

}

void Cpu::insDEC()
{

}

void Cpu::insDEX()
{

}

void Cpu::insDEY()
{

}

void Cpu::insASL()
{

}

void Cpu::insLSR()
{

}

void Cpu::insROL()
{

}

void Cpu::insROR()
{

}

void Cpu::insAND()
{

}

void Cpu::insORA()
{

}

void Cpu::insEOR()
{

}

void Cpu::insCMP()
{

}

void Cpu::insCPX()
{

}

void Cpu::insCPY()
{

}

void Cpu::insBIT()
{

}

void Cpu::insSED()
{

}

void Cpu::insSEI()
{

}

void Cpu::insSEC()
{

}

void Cpu::insCLC()
{

}

void Cpu::insCLD()
{

}

void Cpu::insCLI()
{

}

void Cpu::insCLV()
{

}

void Cpu::insTAX()
{

}

void Cpu::insTXA()
{

}

void Cpu::insTAY()
{

}

void Cpu::insTYA()
{

}

void Cpu::insTSX()
{

}

void Cpu::insTXS()
{

}

void Cpu::insPHA()
{

}

void Cpu::insPLA()
{

}

void Cpu::insPHP()
{

}

void Cpu::insPLP()
{

}

void Cpu::insRTS()
{

}

void Cpu::insRTI()
{

}

void Cpu::insBRK()
{

}

void Cpu::insNOP()
{

}

void Cpu::insBCC()
{

}

void Cpu::insBCS()
{

}

void Cpu::insBEQ()
{

}

void Cpu::insBMI()
{

}

void Cpu::insBNE()
{

}

void Cpu::insBPL()
{

}

void Cpu::insBVC()
{

}

void Cpu::insBVS()
{

}

void Cpu::insJMP()
{

}

void Cpu::insJSR()
{

}

Cpu::Cpu(Memory& memory) : m_memory(memory)
{

}

void Cpu::execute(bool continuous)
{
    while (continuous) {
        m_opCode = m_memory[m_state.pc];
        const auto& decodeEntry = decodeLookUpTable[m_memory[m_state.pc]];
        m_operation = decodeEntry.operation;
        if(m_operation->instruction == Invalid) {
            m_executionStatus = InvalidOpCode;
            break;
        }
        (this->*decodeEntry.operandsProvider)();
        (this->*decodeEntry.instructionExecutor)();
    }
}
