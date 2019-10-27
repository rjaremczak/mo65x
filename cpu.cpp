#include "cpu.h"
#include "cpu_lookuptables.h"

Cpu::Cpu(Memory& memory) : memory_(memory) {
}

void Cpu::amImplied() {
  // nothing to do
}

void Cpu::amAccumulator() {
  effectiveOperand_ = &registers_.a;
}

void Cpu::amRelative() {
  // all handled by branch instructions
}

void Cpu::amIndirect() {
  effectiveAddress_ = memory_.read16(operand16());
}

void Cpu::amImmediate() {
  effectiveOperand_ = operand_;
}

void Cpu::amZeroPage() {
  setEffectiveAddressAndOperand(operand8());
}

void Cpu::amZeroPageX() {
  setEffectiveAddressAndOperand(operand8(), registers_.x);
}

void Cpu::amZeroPageY() {
  setEffectiveAddressAndOperand(operand8(), registers_.y);
}

void Cpu::amIndexedIndirectX() {
  setEffectiveAddressAndOperand(loByte(memory_.read16((operand8() + registers_.x))));
}

void Cpu::amIndirectIndexedY() {
  setEffectiveAddressAndOperand(memory_.read16(operand8()), registers_.y);
}

void Cpu::amAbsolute() {
  setEffectiveAddressAndOperand(operand16());
}

void Cpu::amAbsoluteX() {
  setEffectiveAddressAndOperand(operand16(), registers_.x);
}

void Cpu::amAbsoluteY() {
  setEffectiveAddressAndOperand(operand16(), registers_.y);
}

void Cpu::insLDA() {
  registers_.a = *effectiveOperand_;
}

void Cpu::insLDX() {
  registers_.x = *effectiveOperand_;
}

void Cpu::insLDY() {
  registers_.y = *effectiveOperand_;
}

void Cpu::insSTA() {
  *effectiveOperand_ = registers_.a;
}

void Cpu::insSTX() {
  *effectiveOperand_ = registers_.x;
}

void Cpu::insSTY() {
  *effectiveOperand_ = registers_.y;
}

void Cpu::insADC() {
  const auto op1 = registers_.a;
  const auto op2 = *effectiveOperand_;
  const uint16_t result = op1 + op2 + flags_.c;
  registers_.a = loByte(result);
  flags_.computeNZC(result);
  flags_.computeV(op1, op2, result);
}

void Cpu::insSBC() {
  const auto op1 = registers_.a;
  const auto op2 = *effectiveOperand_;
  const uint16_t result = op1 - op2 - flags_.c;
  registers_.a = loByte(result);
  flags_.computeNZ(result);
  flags_.computeC(result);
  flags_.computeV(op1, op2, result);
}

void Cpu::insINC() {
  flags_.computeNZ(++(*effectiveOperand_));
}

void Cpu::insINX() {
  flags_.computeNZ(++registers_.x);
}

void Cpu::insINY() {
  flags_.computeNZ(++registers_.y);
}

void Cpu::insDEC() {
  flags_.computeNZ(--(*effectiveOperand_));
}

void Cpu::insDEX() {
  flags_.computeNZ(--registers_.x);
}

void Cpu::insDEY() {
  flags_.computeNZ(--registers_.y);
}

void Cpu::insASL() {
  auto val = *effectiveOperand_;
  flags_.c = val & 0x80;
  flags_.computeNZ(*effectiveOperand_ = static_cast<uint8_t>(val << 1));
}

void Cpu::insLSR() {
  auto val = *effectiveOperand_;
  flags_.c = val & 0x01;
  flags_.computeNZ(*effectiveOperand_ = static_cast<uint8_t>(val >> 1));
}

void Cpu::insROL() {
  const uint16_t res = static_cast<uint16_t>(*effectiveOperand_ << 1) | flags_.c;
  flags_.c = res & 0x100;
  flags_.computeNZ(*effectiveOperand_ = loByte(res));
}

void Cpu::insROR() {
  const uint16_t tmp = *effectiveOperand_ | (flags_.c ? 0x100 : 0x00);
  flags_.c = tmp & 0x01;
  flags_.computeNZ(*effectiveOperand_ = loByte(tmp >> 1));
}

void Cpu::insAND() {
  flags_.computeNZ(registers_.a &= *effectiveOperand_);
}

void Cpu::insORA() {
  flags_.computeNZ(registers_.a |= *effectiveOperand_);
}

void Cpu::insEOR() {
  flags_.computeNZ(registers_.a ^= *effectiveOperand_);
}

void Cpu::insCMP() {
  flags_.computeNZC(registers_.a - *effectiveOperand_);
}

void Cpu::insCPX() {
  flags_.computeNZC(registers_.x - *effectiveOperand_);
}

void Cpu::insCPY() {
  flags_.computeNZC(registers_.y - *effectiveOperand_);
}

void Cpu::insBIT() {
  flags_.computeNZ(registers_.a & *effectiveOperand_);
}

void Cpu::insSED() {
  flags_.d = true;
}

void Cpu::insSEI() {
  flags_.i = true;
}

void Cpu::insSEC() {
  flags_.c = true;
}

void Cpu::insCLC() {
  flags_.c = false;
}

void Cpu::insCLD() {
  flags_.d = false;
}

void Cpu::insCLI() {
  flags_.i = false;
}

void Cpu::insCLV() {
  flags_.v = false;
}

void Cpu::insTAX() {
  flags_.computeNZ(registers_.x = registers_.a);
}

void Cpu::insTXA() {
  flags_.computeNZ(registers_.a = registers_.x);
}

void Cpu::insTAY() {
  flags_.computeNZ(registers_.y = registers_.a);
}

void Cpu::insTYA() {
  flags_.computeNZ(registers_.a = registers_.y);
}

void Cpu::insTSX() {
  registers_.x = registers_.sp.value;
}

void Cpu::insTXS() {
  registers_.sp.value = registers_.x;
}

void Cpu::insPHA() {
  push(registers_.a);
}

void Cpu::insPLA() {
  registers_.a = pull();
}

void Cpu::insPHP() {
  push(flags_);
}

void Cpu::insPLP() {
  flags_ = pull();
}

void Cpu::insNOP() {
  // literally do nothing :-)
}

void Cpu::insBCC() {
  if (!flags_.c) branch();
}

void Cpu::insBCS() {
  if (flags_.c) branch();
}

void Cpu::insBEQ() {
  if (flags_.z) branch();
}

void Cpu::insBMI() {
  if (flags_.n) branch();
}

void Cpu::insBNE() {
  if (!flags_.z) branch();
}

void Cpu::insBPL() {
  if (!flags_.n) branch();
}

void Cpu::insBVC() {
  if (!flags_.v) branch();
}

void Cpu::insBVS() {
  if (flags_.v) branch();
}

void Cpu::insJMP() {
  registers_.pc = effectiveAddress_;
}

void Cpu::insJSR() {
  push(loByte(registers_.pc));
  push(hiByte(registers_.pc));
  registers_.pc = effectiveAddress_;
}

void Cpu::insRTS() {
  const auto lo = pull();
  const auto hi = pull();
  registers_.pc = wordOf(lo, hi);
}

void Cpu::insRTI() {
  flags_ = pull();
  const auto lo = pull();
  const auto hi = pull();
  registers_.pc = wordOf(lo, hi) + 1;
}

void Cpu::insBRK() {
  registers_.pc++;
  push(hiByte(registers_.pc));
  push(loByte(registers_.pc));
  push(flags_.withBreakFlag());
  registers_.pc = memory_.read16(VectorIRQ);
}

void Cpu::irq() {
  push(hiByte(registers_.pc));
  push(loByte(registers_.pc));
  push(flags_);
  flags_.i = true;
  registers_.pc = memory_.read16(VectorIRQ);
}

void Cpu::nmi() {
  flags_.i = true;
  push(hiByte(registers_.pc));
  push(loByte(registers_.pc));
  push(flags_);
  registers_.pc = memory_.read16(VectorNMI);
}

void Cpu::reset() {
  registers_.pc = memory_.read16(VectorRESET);
}

void Cpu::execute(bool continuous) {
  executionStatus_ = Running;
  while (continuous && executionStatus_ != StopRequested) {
    instruction_ = &memory_[registers_.pc];
    operand_ = const_cast<Memory::iterator>(instruction_ + 1);
    const auto& decodeEntry = OpCodeLookUpTable[*instruction_];
    const auto operation = decodeEntry.operation;

    if (operation->instruction == INV) {
      executionStatus_ = InvalidOpCode;
      return;
    }

    registers_.pc += operation->size;
    cycles_ += operation->cycles;

    (this->*decodeEntry.prepareOperands)();
    (this->*decodeEntry.executeInstruction)();
  }
  executionStatus_ = Idle;
}
