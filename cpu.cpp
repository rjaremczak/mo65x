#include "cpu.h"
#include "cpu_lookuptables.h"

Cpu::Cpu(Memory& memory) : memory_(memory) {
}

void Cpu::amImplied() {
  // nothing to do
}

void Cpu::amAccumulator() {
  effectiveOperand = &registers.a;
}

void Cpu::amRelative() {
  // all handled by branch instructions
}

void Cpu::amIndirect() {
  effectiveAddress = memory_.read16(operand16());
}

void Cpu::amImmediate() {
  effectiveOperand = operand;
}

void Cpu::amZeroPage() {
  setEffectiveAddressAndOperand(operand8());
}

void Cpu::amZeroPageX() {
  setEffectiveAddressAndOperand(operand8(), registers.x);
}

void Cpu::amZeroPageY() {
  setEffectiveAddressAndOperand(operand8(), registers.y);
}

void Cpu::amIndexedIndirectX() {
  setEffectiveAddressAndOperand(loByte(memory_.read16((operand8() + registers.x))));
}

void Cpu::amIndirectIndexedY() {
  setEffectiveAddressAndOperand(memory_.read16(operand8()), registers.y);
}

void Cpu::amAbsolute() {
  setEffectiveAddressAndOperand(operand16());
}

void Cpu::amAbsoluteX() {
  setEffectiveAddressAndOperand(operand16(), registers.x);
}

void Cpu::amAbsoluteY() {
  setEffectiveAddressAndOperand(operand16(), registers.y);
}

void Cpu::insLDA() {
  registers.a = *effectiveOperand;
}

void Cpu::insLDX() {
  registers.x = *effectiveOperand;
}

void Cpu::insLDY() {
  registers.y = *effectiveOperand;
}

void Cpu::insSTA() {
  *effectiveOperand = registers.a;
}

void Cpu::insSTX() {
  *effectiveOperand = registers.x;
}

void Cpu::insSTY() {
  *effectiveOperand = registers.y;
}

void Cpu::insADC() {
  const auto op1 = registers.a;
  const auto op2 = *effectiveOperand;
  const uint16_t result = op1 + op2 + flags.c;
  registers.a = loByte(result);
  flags.computeNZC(result);
  flags.computeV(op1, op2, result);
}

void Cpu::insSBC() {
  const auto op1 = registers.a;
  const auto op2 = *effectiveOperand;
  const uint16_t result = op1 - op2 - flags.c;
  registers.a = loByte(result);
  flags.computeNZ(result);
  flags.computeC(result);
  flags.computeV(op1, op2, result);
}

void Cpu::insINC() {
  flags.computeNZ(++(*effectiveOperand));
}

void Cpu::insINX() {
  flags.computeNZ(++registers.x);
}

void Cpu::insINY() {
  flags.computeNZ(++registers.y);
}

void Cpu::insDEC() {
  flags.computeNZ(--(*effectiveOperand));
}

void Cpu::insDEX() {
  flags.computeNZ(--registers.x);
}

void Cpu::insDEY() {
  flags.computeNZ(--registers.y);
}

void Cpu::insASL() {
  auto val = *effectiveOperand;
  flags.c = val & 0x80;
  flags.computeNZ(*effectiveOperand = static_cast<uint8_t>(val << 1));
}

void Cpu::insLSR() {
  auto val = *effectiveOperand;
  flags.c = val & 0x01;
  flags.computeNZ(*effectiveOperand = static_cast<uint8_t>(val >> 1));
}

void Cpu::insROL() {
  const uint16_t res = static_cast<uint16_t>(*effectiveOperand << 1) | flags.c;
  flags.c = res & 0x100;
  flags.computeNZ(*effectiveOperand = loByte(res));
}

void Cpu::insROR() {
  const uint16_t tmp = *effectiveOperand | (flags.c ? 0x100 : 0x00);
  flags.c = tmp & 0x01;
  flags.computeNZ(*effectiveOperand = loByte(tmp >> 1));
}

void Cpu::insAND() {
  flags.computeNZ(registers.a &= *effectiveOperand);
}

void Cpu::insORA() {
  flags.computeNZ(registers.a |= *effectiveOperand);
}

void Cpu::insEOR() {
  flags.computeNZ(registers.a ^= *effectiveOperand);
}

void Cpu::insCMP() {
  flags.computeNZC(registers.a - *effectiveOperand);
}

void Cpu::insCPX() {
  flags.computeNZC(registers.x - *effectiveOperand);
}

void Cpu::insCPY() {
  flags.computeNZC(registers.y - *effectiveOperand);
}

void Cpu::insBIT() {
  flags.computeNZ(registers.a & *effectiveOperand);
}

void Cpu::insSED() {
  flags.d = true;
}

void Cpu::insSEI() {
  flags.i = true;
}

void Cpu::insSEC() {
  flags.c = true;
}

void Cpu::insCLC() {
  flags.c = false;
}

void Cpu::insCLD() {
  flags.d = false;
}

void Cpu::insCLI() {
  flags.i = false;
}

void Cpu::insCLV() {
  flags.v = false;
}

void Cpu::insTAX() {
  flags.computeNZ(registers.x = registers.a);
}

void Cpu::insTXA() {
  flags.computeNZ(registers.a = registers.x);
}

void Cpu::insTAY() {
  flags.computeNZ(registers.y = registers.a);
}

void Cpu::insTYA() {
  flags.computeNZ(registers.a = registers.y);
}

void Cpu::insTSX() {
  registers.x = registers.sp.value;
}

void Cpu::insTXS() {
  registers.sp.value = registers.x;
}

void Cpu::insPHA() {
  push(registers.a);
}

void Cpu::insPLA() {
  registers.a = pull();
}

void Cpu::insPHP() {
  push(flags);
}

void Cpu::insPLP() {
  flags = pull();
}

void Cpu::insNOP() {
  // literally do nothing :-)
}

void Cpu::insBCC() {
  if (!flags.c) branch();
}

void Cpu::insBCS() {
  if (flags.c) branch();
}

void Cpu::insBEQ() {
  if (flags.z) branch();
}

void Cpu::insBMI() {
  if (flags.n) branch();
}

void Cpu::insBNE() {
  if (!flags.z) branch();
}

void Cpu::insBPL() {
  if (!flags.n) branch();
}

void Cpu::insBVC() {
  if (!flags.v) branch();
}

void Cpu::insBVS() {
  if (flags.v) branch();
}

void Cpu::insJMP() {
  registers.pc = effectiveAddress;
}

void Cpu::insJSR() {
  push(loByte(registers.pc));
  push(hiByte(registers.pc));
  registers.pc = effectiveAddress;
}

void Cpu::insRTS() {
  const auto lo = pull();
  const auto hi = pull();
  registers.pc = wordOf(lo, hi);
}

void Cpu::insRTI() {
  flags = pull();
  const auto lo = pull();
  const auto hi = pull();
  registers.pc = wordOf(lo, hi) + 1;
}

void Cpu::insBRK() {
  registers.pc++;
  push(hiByte(registers.pc));
  push(loByte(registers.pc));
  push(flags.withBreakFlag());
  registers.pc = memory_.read16(VectorIRQ);
}

void Cpu::irq() {
  push(hiByte(registers.pc));
  push(loByte(registers.pc));
  push(flags);
  flags.i = true;
  registers.pc = memory_.read16(VectorIRQ);
}

void Cpu::nmi() {
  flags.i = true;
  push(hiByte(registers.pc));
  push(loByte(registers.pc));
  push(flags);
  registers.pc = memory_.read16(VectorNMI);
}

void Cpu::reset() {
  registers.pc = memory_.read16(VectorRESET);
}

void Cpu::execute(bool continuous) {
  executionStatus_ = Running;
  while (continuous && executionStatus_ != StopRequested) {
    instruction = &memory_[registers.pc];
    operand = const_cast<Memory::iterator>(instruction + 1);
    const auto& decodeEntry = OpCodeLookUpTable[*instruction];
    const auto operation = decodeEntry.operation;

    if (operation->instruction == INV) {
      executionStatus_ = InvalidOpCode;
      return;
    }

    registers.pc += operation->size;
    cycles += operation->cycles;

    (this->*decodeEntry.prepareOperands)();
    (this->*decodeEntry.executeInstruction)();
  }
  executionStatus_ = Idle;
}
