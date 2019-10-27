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
  const uint16_t result = op1 + op2 + registers.p.c;
  registers.a = loByte(result);
  registers.p.computeNZC(result);
  registers.p.computeV(op1, op2, result);
}

void Cpu::insSBC() {
  const auto op1 = registers.a;
  const auto op2 = *effectiveOperand;
  const uint16_t result = op1 - op2 - registers.p.c;
  registers.a = loByte(result);
  registers.p.computeNZC(result);
  registers.p.computeV(op1, op2, result);
}

void Cpu::insINC() {
  registers.p.computeNZ(++(*effectiveOperand));
}

void Cpu::insINX() {
  registers.p.computeNZ(++registers.x);
}

void Cpu::insINY() {
  registers.p.computeNZ(++registers.y);
}

void Cpu::insDEC() {
  registers.p.computeNZ(--(*effectiveOperand));
}

void Cpu::insDEX() {
  registers.p.computeNZ(--registers.x);
}

void Cpu::insDEY() {
  registers.p.computeNZ(--registers.y);
}

void Cpu::insASL() {
  auto val = *effectiveOperand;
  registers.p.c = val & 0x80;
  registers.p.computeNZ(*effectiveOperand = static_cast<uint8_t>(val << 1));
}

void Cpu::insLSR() {
  auto val = *effectiveOperand;
  registers.p.c = val & 0x01;
  registers.p.computeNZ(*effectiveOperand = static_cast<uint8_t>(val >> 1));
}

void Cpu::insROL() {
  const uint16_t res = static_cast<uint16_t>(*effectiveOperand << 1) | registers.p.c;
  registers.p.c = res & 0x100;
  registers.p.computeNZ(*effectiveOperand = loByte(res));
}

void Cpu::insROR() {
  const uint16_t tmp = *effectiveOperand | (registers.p.c ? 0x100 : 0x00);
  registers.p.c = tmp & 0x01;
  registers.p.computeNZ(*effectiveOperand = loByte(tmp >> 1));
}

void Cpu::insAND() {
  registers.p.computeNZ(registers.a &= *effectiveOperand);
}

void Cpu::insORA() {
  registers.p.computeNZ(registers.a |= *effectiveOperand);
}

void Cpu::insEOR() {
  registers.p.computeNZ(registers.a ^= *effectiveOperand);
}

void Cpu::insCMP() {
  registers.p.computeNZC(registers.a - *effectiveOperand);
}

void Cpu::insCPX() {
  registers.p.computeNZC(registers.x - *effectiveOperand);
}

void Cpu::insCPY() {
  registers.p.computeNZC(registers.y - *effectiveOperand);
}

void Cpu::insBIT() {
  registers.p.computeNZ(registers.a & *effectiveOperand);
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
    instruction = &memory_[registers.pc];
    operand = const_cast<Memory::iterator>(instruction + 1);
    const auto& decodeEntry = OpCodeLookUpTable[*instruction];
    const auto operation = decodeEntry.operation;

    if (operation->instruction == INV) {
      executionStatus = InvalidOpCode;
      return;
    }

    registers.pc += operation->size;
    cycles += operation->cycles;

    (this->*decodeEntry.prepareOperands)();
    (this->*decodeEntry.executeInstruction)();

    if (!continuous) break;
  }
  executionStatus = Stopped;
}
