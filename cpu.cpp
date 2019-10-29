#include "cpu.h"
#include "decodetable.h"

Cpu::Cpu(Memory& memory) : memory_(memory) {
}

void Cpu::amImplied() {
  // nothing to do
}

void Cpu::amAccumulator() {
  effectiveOperandPtr_ = &registers.a;
}

void Cpu::amRelative() {
  // all handled by branch instructions
}

void Cpu::amIndirect() {
  effectiveAddress_ = memory_.read16(operand16());
}

void Cpu::amImmediate() {
  effectiveOperandPtr_ = operandPtr_;
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
  registers.a = *effectiveOperandPtr_;
}

void Cpu::insLDX() {
  registers.x = *effectiveOperandPtr_;
}

void Cpu::insLDY() {
  registers.y = *effectiveOperandPtr_;
}

void Cpu::insSTA() {
  *effectiveOperandPtr_ = registers.a;
}

void Cpu::insSTX() {
  *effectiveOperandPtr_ = registers.x;
}

void Cpu::insSTY() {
  *effectiveOperandPtr_ = registers.y;
}

void Cpu::insADC() {
  const auto op1 = registers.a;
  const auto op2 = *effectiveOperandPtr_;
  const uint16_t result = op1 + op2 + registers.p.c;
  registers.a = loByte(result);
  registers.p.computeNZC(result);
  registers.p.computeV(op1, op2, result);
}

void Cpu::insSBC() {
  const auto op1 = registers.a;
  const auto op2 = *effectiveOperandPtr_;
  const uint16_t result = op1 - op2 - registers.p.c;
  registers.a = loByte(result);
  registers.p.computeNZC(result);
  registers.p.computeV(op1, op2, result);
}

void Cpu::insINC() {
  registers.p.computeNZ(++(*effectiveOperandPtr_));
}

void Cpu::insINX() {
  registers.p.computeNZ(++registers.x);
}

void Cpu::insINY() {
  registers.p.computeNZ(++registers.y);
}

void Cpu::insDEC() {
  registers.p.computeNZ(--(*effectiveOperandPtr_));
}

void Cpu::insDEX() {
  registers.p.computeNZ(--registers.x);
}

void Cpu::insDEY() {
  registers.p.computeNZ(--registers.y);
}

void Cpu::insASL() {
  auto val = *effectiveOperandPtr_;
  registers.p.c = val & 0x80;
  registers.p.computeNZ(*effectiveOperandPtr_ = static_cast<uint8_t>(val << 1));
}

void Cpu::insLSR() {
  auto val = *effectiveOperandPtr_;
  registers.p.c = val & 0x01;
  registers.p.computeNZ(*effectiveOperandPtr_ = static_cast<uint8_t>(val >> 1));
}

void Cpu::insROL() {
  const uint16_t res = static_cast<uint16_t>(*effectiveOperandPtr_ << 1) | registers.p.c;
  registers.p.c = res & 0x100;
  registers.p.computeNZ(*effectiveOperandPtr_ = loByte(res));
}

void Cpu::insROR() {
  const uint16_t tmp = *effectiveOperandPtr_ | (registers.p.c ? 0x100 : 0x00);
  registers.p.c = tmp & 0x01;
  registers.p.computeNZ(*effectiveOperandPtr_ = loByte(tmp >> 1));
}

void Cpu::insAND() {
  registers.p.computeNZ(registers.a &= *effectiveOperandPtr_);
}

void Cpu::insORA() {
  registers.p.computeNZ(registers.a |= *effectiveOperandPtr_);
}

void Cpu::insEOR() {
  registers.p.computeNZ(registers.a ^= *effectiveOperandPtr_);
}

void Cpu::insCMP() {
  registers.p.computeNZC(registers.a - *effectiveOperandPtr_);
}

void Cpu::insCPX() {
  registers.p.computeNZC(registers.x - *effectiveOperandPtr_);
}

void Cpu::insCPY() {
  registers.p.computeNZC(registers.y - *effectiveOperandPtr_);
}

void Cpu::insBIT() {
  registers.p.computeNZ(registers.a & *effectiveOperandPtr_);
}

void Cpu::insSED() {
  registers.p.d = true;
}

void Cpu::insSEI() {
  registers.p.i = true;
}

void Cpu::insSEC() {
  registers.p.c = true;
}

void Cpu::insCLC() {
  registers.p.c = false;
}

void Cpu::insCLD() {
  registers.p.d = false;
}

void Cpu::insCLI() {
  registers.p.i = false;
}

void Cpu::insCLV() {
  registers.p.v = false;
}

void Cpu::insTAX() {
  registers.p.computeNZ(registers.x = registers.a);
}

void Cpu::insTXA() {
  registers.p.computeNZ(registers.a = registers.x);
}

void Cpu::insTAY() {
  registers.p.computeNZ(registers.y = registers.a);
}

void Cpu::insTYA() {
  registers.p.computeNZ(registers.a = registers.y);
}

void Cpu::insTSX() {
  registers.x = registers.sp.toByte();
}

void Cpu::insTXS() {
  registers.sp.fromByte(registers.x);
}

void Cpu::insPHA() {
  push(registers.a);
}

void Cpu::insPLA() {
  registers.a = pull();
}

void Cpu::insPHP() {
  push(registers.p);
}

void Cpu::insPLP() {
  registers.p = pull();
}

void Cpu::insNOP() {
  // literally do nothing :-)
}

void Cpu::insBCC() {
  if (!registers.p.c) branch();
}

void Cpu::insBCS() {
  if (registers.p.c) branch();
}

void Cpu::insBEQ() {
  if (registers.p.z) branch();
}

void Cpu::insBMI() {
  if (registers.p.n) branch();
}

void Cpu::insBNE() {
  if (!registers.p.z) branch();
}

void Cpu::insBPL() {
  if (!registers.p.n) branch();
}

void Cpu::insBVC() {
  if (!registers.p.v) branch();
}

void Cpu::insBVS() {
  if (registers.p.v) branch();
}

void Cpu::insJMP() {
  registers.pc = effectiveAddress_;
}

void Cpu::insJSR() {
  push(loByte(registers.pc));
  push(hiByte(registers.pc));
  registers.pc = effectiveAddress_;
}

void Cpu::insRTS() {
  const auto lo = pull();
  const auto hi = pull();
  registers.pc = wordOf(lo, hi);
}

void Cpu::insRTI() {
  registers.p = pull();
  const auto lo = pull();
  const auto hi = pull();
  registers.pc = wordOf(lo, hi) + 1;
}

void Cpu::insBRK() {
  registers.pc++;
  push(hiByte(registers.pc));
  push(loByte(registers.pc));
  push(registers.p.withBreakFlag());
  registers.pc = memory_.read16(VectorIRQ);
}

void Cpu::irq() {
  push(hiByte(registers.pc));
  push(loByte(registers.pc));
  push(registers.p);
  registers.p.i = true;
  registers.pc = memory_.read16(VectorIRQ);
}

void Cpu::nmi() {
  registers.p.i = true;
  push(hiByte(registers.pc));
  push(loByte(registers.pc));
  push(registers.p);
  registers.pc = memory_.read16(VectorNMI);
}

void Cpu::reset() {
  registers.pc = memory_.read16(VectorRESET);
}

void Cpu::execute(bool continuous) {
  executionStatus = Running;
  while (executionStatus != StopRequested) {
    const auto pc = &memory_[registers.pc];
    operandPtr_ = pc + 1;
    const auto& entry = DecodeTable[*pc];
    const auto insdef = entry.instruction;

    if (insdef->type == INV) {
      executionStatus = InvalidOpCode;
      return;
    }

    registers.pc += insdef->size;
    cycles += insdef->cycles;

    (this->*entry.prepareOperands)();
    (this->*entry.executeInstruction)();

    if (!continuous) break;
  }
  executionStatus = Stopped;
}
