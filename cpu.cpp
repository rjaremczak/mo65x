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

void Cpu::LDA()
{

}

void Cpu::LDX()
{

}

void Cpu::LDY()
{

}

void Cpu::STA()
{

}

void Cpu::STX()
{

}

void Cpu::STY()
{

}

void Cpu::ADC()
{

}

void Cpu::SBC()
{

}

void Cpu::INC()
{

}

void Cpu::INX()
{

}

void Cpu::INY()
{

}

void Cpu::DEC()
{

}

void Cpu::DEX()
{

}

void Cpu::DEY()
{

}

void Cpu::ASL()
{

}

void Cpu::LSR()
{

}

void Cpu::ROL()
{

}

void Cpu::ROR()
{

}

void Cpu::AND()
{

}

void Cpu::ORA()
{

}

void Cpu::EOR()
{

}

void Cpu::CMP()
{

}

void Cpu::CPX()
{

}

void Cpu::CPY()
{

}

void Cpu::BIT()
{

}

void Cpu::SED()
{

}

void Cpu::SEI()
{

}

void Cpu::CLC()
{

}

void Cpu::CLD()
{

}

void Cpu::CLI()
{

}

void Cpu::CLV()
{

}

void Cpu::TAX()
{

}

void Cpu::TXA()
{

}

void Cpu::TAY()
{

}

void Cpu::TYA()
{

}

void Cpu::TSX()
{

}

void Cpu::TXS()
{

}

void Cpu::PHA()
{

}

void Cpu::PLA()
{

}

void Cpu::PHP()
{

}

void Cpu::PLP()
{

}

void Cpu::RTS()
{

}

void Cpu::RTI()
{

}

void Cpu::BRK()
{

}

void Cpu::NOP()
{

}

void Cpu::BCC()
{

}

void Cpu::BCS()
{

}

void Cpu::BEQ()
{

}

void Cpu::BMI()
{

}

void Cpu::BNE()
{

}

void Cpu::BPL()
{

}

void Cpu::BVC()
{

}

void Cpu::BVS()
{

}

void Cpu::JMP()
{

}

void Cpu::JSR()
{

}

Cpu::Cpu(Memory& memory) : m_memory(memory)
{

}

Cpu::ExecutionStatus Cpu::execute(bool continuous)
{
    while (continuous) {
        m_instruction = OpCodes[m_memory[m_state.pc]];
        if(m_instruction.instruction == Invalid) return InvalidOpCode;

        switch (m_instruction.addressing) {
        case Implied: break;
            // exec
        case Absolute:
            // exec(absAddressing
            break;
        }

    }

    return Ok;
}
