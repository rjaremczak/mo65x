#include "cpu.h"

void Cpu::amImplied() {
  // nothing to do
}

void Cpu::amAccumulator() {
  m_effectiveOperand = &m_regs.a;
}

void Cpu::amRelative() {
  // all handled by branch instructions
}

void Cpu::amIndirect() {
  // handled by jump instruction
}

void Cpu::amImmediate() {
  m_effectiveOperand = m_operands;
}

void Cpu::amZeroPage() {
  m_effectiveOperand = &m_memory[operand8()];
}

void Cpu::amZeroPageX() {
  m_effectiveOperand = &m_memory[static_cast<uint8_t>(operand8() + m_regs.x)];
}

void Cpu::amZeroPageY() {
  m_effectiveOperand = &m_memory[static_cast<uint8_t>(operand8() + m_regs.y)];
}

void Cpu::amIndexedIndirectX() {
  const auto addr = m_memory.read16(static_cast<uint8_t>(operand8() + m_regs.x));
  m_effectiveOperand = &m_memory[addr];
}

void Cpu::amIndirectIndexedY() {
  const auto addr = m_memory.read16(operand8());
  m_effectiveOperand = &m_memory[calculateAddressWithCyclePenalty(addr, m_regs.y)];
}

void Cpu::amAbsolute() {
  m_effectiveOperand = &m_memory[operand16()];
}

void Cpu::amAbsoluteX() {
  const auto addr = operand16();
  m_effectiveOperand = &m_memory[calculateAddressWithCyclePenalty(addr, m_regs.x)];
}

void Cpu::amAbsoluteY() {
  const auto addr = operand16();
  m_effectiveOperand = &m_memory[calculateAddressWithCyclePenalty(addr, m_regs.y)];
}

void Cpu::insLDA() {
  m_regs.a = *m_effectiveOperand;
}

void Cpu::insLDX() {
  m_regs.x = *m_effectiveOperand;
}

void Cpu::insLDY() {
  m_regs.y = *m_effectiveOperand;
}

void Cpu::insSTA() {
  *m_effectiveOperand = m_regs.a;
}

void Cpu::insSTX() {
  *m_effectiveOperand = m_regs.x;
}

void Cpu::insSTY() {
  *m_effectiveOperand = m_regs.y;
}

void Cpu::insADC() {
  const auto op1 = m_regs.a;
  const auto op2 = *m_effectiveOperand;
  const uint16_t result = op1 + op2 + m_flags.c;
  m_regs.a = result & 0xff;
  m_flags.computeNZC(result);
  m_flags.computeV(op1, op2, result);
}

void Cpu::insSBC() {
  const auto op1 = m_regs.a;
  const auto op2 = *m_effectiveOperand;
  const uint16_t result = op1 - op2 - m_flags.c;
  m_regs.a = result & 0xff;
  m_flags.computeNZ(result);
  m_flags.computeC(result);
  m_flags.computeV(op1, op2, result);
}

void Cpu::insINC() {
  m_flags.computeNZ(++(*m_effectiveOperand));
}

void Cpu::insINX() {
  m_flags.computeNZ(++m_regs.x);
}

void Cpu::insINY() {
  m_flags.computeNZ(++m_regs.y);
}

void Cpu::insDEC() {
  m_flags.computeNZ(--(*m_effectiveOperand));
}

void Cpu::insDEX() {
  m_flags.computeNZ(--m_regs.x);
}

void Cpu::insDEY() {
  m_flags.computeNZ(--m_regs.y);
}

void Cpu::insASL() {
  auto val = *m_effectiveOperand;
  m_flags.c = val & 0x80;
  m_flags.computeNZ(*m_effectiveOperand = static_cast<uint8_t>(val << 1));
}

void Cpu::insLSR() {
  auto val = *m_effectiveOperand;
  m_flags.c = val & 0x01;
  m_flags.computeNZ(*m_effectiveOperand = static_cast<uint8_t>(val >> 1));
}

void Cpu::insROL() {
  const auto res = static_cast<uint16_t>(*m_effectiveOperand << 1) | m_flags.c;
  m_flags.c = res & 0x100;
  m_flags.computeNZ(*m_effectiveOperand = res & 0xff);
}

void Cpu::insROR() {
  uint16_t tmp = *m_effectiveOperand | (m_flags.c ? 0x100 : 0x00);
  m_flags.c = tmp & 0x01;
  m_flags.computeNZ(*m_effectiveOperand = (tmp >> 1) & 0xff);
}

void Cpu::insAND() {
  m_flags.computeNZ(m_regs.a &= *m_effectiveOperand);
}

void Cpu::insORA() {
  m_flags.computeNZ(m_regs.a |= *m_effectiveOperand);
}

void Cpu::insEOR() {
  m_flags.computeNZ(m_regs.a ^= *m_effectiveOperand);
}

void Cpu::insCMP() {
  m_flags.computeNZC(m_regs.a - *m_effectiveOperand);
}

void Cpu::insCPX() {
  m_flags.computeNZC(m_regs.x - *m_effectiveOperand);
}

void Cpu::insCPY() {
  m_flags.computeNZC(m_regs.y - *m_effectiveOperand);
}

void Cpu::insBIT() {
  m_flags.computeNZ(m_regs.a & *m_effectiveOperand);
}

void Cpu::insSED() {
  m_flags.d = true;
}

void Cpu::insSEI() {
  m_flags.i = true;
}

void Cpu::insSEC() {
  m_flags.c = true;
}

void Cpu::insCLC() {
  m_flags.c = false;
}

void Cpu::insCLD() {
  m_flags.d = false;
}

void Cpu::insCLI() {
  m_flags.i = false;
}

void Cpu::insCLV() {
  m_flags.v = false;
}

void Cpu::insTAX() {
  m_flags.computeNZ(m_regs.x = m_regs.a);
}

void Cpu::insTXA() {
  m_flags.computeNZ(m_regs.a = m_regs.x);
}

void Cpu::insTAY() {
  m_flags.computeNZ(m_regs.y = m_regs.a);
}

void Cpu::insTYA() {
  m_flags.computeNZ(m_regs.a = m_regs.y);
}

void Cpu::insTSX() {
  m_regs.x = m_regs.sp.value;
}

void Cpu::insTXS() {
  m_regs.sp.value = m_regs.x;
}

void Cpu::insPHA() {
  push(m_regs.a);
}

void Cpu::insPLA() {
  m_regs.a = pull();
}

void Cpu::insPHP() {
  push(m_flags);
}

void Cpu::insPLP() {
  m_flags = pull();
}

void Cpu::insRTS() {
  const auto lsb = pull();
  const auto msb = pull();
  m_regs.pc = static_cast<uint16_t>(lsb + (msb << 8) + 1);
}

void Cpu::insRTI() {
  m_flags = pull();
  insRTS();
}

void Cpu::insBRK() {
  m_flags.b = true;
  m_regs.pc++;
  irq();
}

void Cpu::insNOP() {
}

void Cpu::insBCC() {
  if(!m_flags.c) branch();
}

void Cpu::insBCS() {
}

void Cpu::insBEQ() {
}

void Cpu::insBMI() {
}

void Cpu::insBNE() {
}

void Cpu::insBPL() {
}

void Cpu::insBVC() {
}

void Cpu::insBVS() {
}

void Cpu::insJMP() {
}

void Cpu::insJSR() {
}

Cpu::Cpu(Memory &memory) : m_memory(memory) {
}

void Cpu::execute(bool continuous) {
  m_executionStatus = Running;
  while (continuous && m_executionStatus != StopRequested) {
    m_instruction = &m_memory[m_regs.pc];
    m_operands = const_cast<Memory::iterator>(m_instruction + 1);
    const auto &decodeEntry = OpCodeLookUpTable[*m_instruction];
    const auto operation = decodeEntry.operation;

    if (operation->instruction == Invalid) {
      m_executionStatus = InvalidOpCode;
      return;
    }

    m_regs.pc += operation->size;
    m_cycles += operation->cycles;

    (this->*decodeEntry.prepareOperands)();
    (this->*decodeEntry.executeInstruction)();
  }
  m_executionStatus = Idle;
}

void Cpu::nmi()
{

}

void Cpu::irq()
{

}

void Cpu::reset()
{

}
