#include "cpu.h"

void Cpu::amImplied()
{
    // nothing to do
}

void Cpu::amAccumulator()
{
    m_operand.bytePtr = &m_regs.a;
}

void Cpu::amRelative()
{
    m_operand.address = static_cast<uint16_t>(m_regs.pc + m_operation->size + static_cast<int8_t>(operand8()));
}

void Cpu::amImmediate()
{
    m_operand.bytePtr = &m_memory[m_regs.pc + 1];
}

void Cpu::amZeroPage()
{
    m_operand.bytePtr = &m_memory[operand8()];
}

void Cpu::amZeroPageX()
{
    m_operand.bytePtr = &m_memory[(operand8() + m_regs.x) & 0xff];
}

void Cpu::amZeroPageY()
{
    m_operand.bytePtr = &m_memory[(operand8() + m_regs.y) & 0xff];
}

void Cpu::amIndexedIndirectX()
{
    const auto addr = m_memory.read16((operand8() + m_regs.x) & 0xf);
    m_operand.bytePtr = &m_memory[addr];
}

void Cpu::amIndirectIndexedY()
{
    const auto addr = m_memory.read16(operand8());
    extraCycleOnPageBoundaryCrossing(addr, m_regs.y);
    m_operand.bytePtr = &m_memory[addr + m_regs.y];
}

void Cpu::amIndirect()
{
    m_operand.address = m_memory.read16(operand16());
}

void Cpu::amAbsolute()
{
    m_operand.bytePtr = &m_memory[operand16()];
}

void Cpu::amAbsoluteX()
{
    const auto addr = operand16();
    extraCycleOnPageBoundaryCrossing(addr, m_regs.x);
    m_operand.bytePtr = &m_memory[addr + m_regs.x];
}

void Cpu::amAbsoluteY()
{
    const auto addr = operand16();
    extraCycleOnPageBoundaryCrossing(addr, m_regs.y);
    m_operand.bytePtr = &m_memory[addr + m_regs.y];
}

void Cpu::insLDA()
{
    m_regs.a = *m_operand.bytePtr;
}

void Cpu::insLDX()
{
    m_regs.x = *m_operand.bytePtr;
}

void Cpu::insLDY()
{
    m_regs.y = *m_operand.bytePtr;
}

void Cpu::insSTA()
{
    *m_operand.bytePtr = m_regs.a;
}

void Cpu::insSTX()
{
    *m_operand.bytePtr = m_regs.x;
}

void Cpu::insSTY()
{
    *m_operand.bytePtr = m_regs.y;
}

void Cpu::insADC()
{
    const uint8_t op1 = m_regs.a;
    const uint8_t op2 = *m_operand.bytePtr;
    const uint16_t result = op1 + op2 + m_flags.c;
    m_regs.a = result & 0xff;
    m_flags.computeNZ(result);
    m_flags.computeC(result);
    m_flags.computeV(op1, op2, result);
}

void Cpu::insSBC()
{
    const uint8_t op1 = m_regs.a;
    const uint8_t op2 = *m_operand.bytePtr;
    const uint16_t result = op1 - op2 - m_flags.c;
    m_regs.a = result & 0xff;
    m_flags.computeNZ(result);
    m_flags.computeC(result);
    m_flags.computeV(op1, op2, result);
}

void Cpu::insINC()
{
    m_flags.computeNZ(++(*m_operand.bytePtr));
}

void Cpu::insINX()
{
    m_flags.computeNZ(++m_regs.x);
}

void Cpu::insINY()
{
    m_flags.computeNZ(++m_regs.y);
}

void Cpu::insDEC()
{
    m_flags.computeNZ(--(*m_operand.bytePtr));
}

void Cpu::insDEX()
{
    m_flags.computeNZ(--m_regs.x);
}

void Cpu::insDEY()
{
    m_flags.computeNZ(--m_regs.y);
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
    m_executionStatus = Running;
    while (continuous && m_executionStatus != StopRequested) {
        m_opCode = m_memory[m_regs.pc];
        const auto& decodeEntry = decodeLookUpTable[m_memory[m_regs.pc]];
        m_operation = decodeEntry.operation;
        if(m_operation->instruction == Invalid) {
            m_executionStatus = InvalidOpCode;
            return;
        }

        m_cycles += m_operation->cycles;
        (this->*decodeEntry.prepareOperands)();
        (this->*decodeEntry.executeInstruction)();
    }
    m_executionStatus = Idle;
}
