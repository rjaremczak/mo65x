#include "cpu.h"
#include "decodetable.h"

Cpu::Cpu(Memory& memory) : memory_(memory) {
}

void Cpu::prepImpliedMode() {
  // nothing to do
}

void Cpu::prepAccumulatorMode() {
  effectiveOperandPtr_ = &registers.a;
}

void Cpu::prepRelativeMode() {
  // all handled by branch instructions
}

void Cpu::prepIndirectMode() {
  effectiveAddress_ = memory_.read16(operand16());
}

void Cpu::prepImmediateMode() {
  effectiveOperandPtr_ = operandPtr_;
}

void Cpu::prepZeroPageMode() {
  setEffectiveAddressAndOperand(operand8());
}

void Cpu::prepZeroPageXMode() {
  setEffectiveAddressAndOperand(operand8(), registers.x);
}

void Cpu::prepZeroPageYMode() {
  setEffectiveAddressAndOperand(operand8(), registers.y);
}

void Cpu::prepIndexedIndirectXMode() {
  setEffectiveAddressAndOperand(memory_.read16(loByte(operand8() + registers.x)));
}

void Cpu::prepIndirectIndexedYMode() {
  setEffectiveAddressAndOperand(memory_.read16(operand8()), registers.y);
}

void Cpu::prepAbsoluteMode() {
  setEffectiveAddressAndOperand(operand16());
}

void Cpu::prepAbsoluteXMode() {
  setEffectiveAddressAndOperand(operand16(), registers.x);
}

void Cpu::prepAbsoluteYMode() {
  setEffectiveAddressAndOperand(operand16(), registers.y);
}

void Cpu::execLDA() {
  registers.a = *effectiveOperandPtr_;
  applyPageBoundaryCrossingPenalty();
}

void Cpu::execLDX() {
  registers.x = *effectiveOperandPtr_;
  applyPageBoundaryCrossingPenalty();
}

void Cpu::execLDY() {
  registers.y = *effectiveOperandPtr_;
  applyPageBoundaryCrossingPenalty();
}

void Cpu::execSTA() {
  *effectiveOperandPtr_ = registers.a;
}

void Cpu::execSTX() {
  *effectiveOperandPtr_ = registers.x;
}

void Cpu::execSTY() {
  *effectiveOperandPtr_ = registers.y;
}

void Cpu::execADC() {
  const auto op1 = registers.a;
  const auto op2 = *effectiveOperandPtr_;
  const uint16_t result = op1 + op2 + registers.p.c;
  registers.a = loByte(result);
  registers.p.computeNZC(result);
  registers.p.computeV(op1, op2, result);
  applyPageBoundaryCrossingPenalty();
}

void Cpu::execSBC() {
  const auto op1 = registers.a;
  const auto op2 = *effectiveOperandPtr_;
  const uint16_t result = op1 - op2 - registers.p.c;
  registers.a = loByte(result);
  registers.p.computeNZC(result);
  registers.p.computeV(op1, op2, result);
  applyPageBoundaryCrossingPenalty();
}

void Cpu::execINC() {
  registers.p.computeNZ(++(*effectiveOperandPtr_));
}

void Cpu::execINX() {
  registers.p.computeNZ(++registers.x);
}

void Cpu::execINY() {
  registers.p.computeNZ(++registers.y);
}

void Cpu::execDEC() {
  registers.p.computeNZ(--(*effectiveOperandPtr_));
}

void Cpu::execDEX() {
  registers.p.computeNZ(--registers.x);
}

void Cpu::execDEY() {
  registers.p.computeNZ(--registers.y);
}

void Cpu::execASL() {
  auto val = *effectiveOperandPtr_;
  registers.p.c = val & 0x80;
  registers.p.computeNZ(*effectiveOperandPtr_ = static_cast<uint8_t>(val << 1));
}

void Cpu::execLSR() {
  auto val = *effectiveOperandPtr_;
  registers.p.c = val & 0x01;
  registers.p.computeNZ(*effectiveOperandPtr_ = static_cast<uint8_t>(val >> 1));
}

void Cpu::execROL() {
  const uint16_t res = static_cast<uint16_t>(*effectiveOperandPtr_ << 1) | registers.p.c;
  registers.p.c = res & 0x100;
  registers.p.computeNZ(*effectiveOperandPtr_ = loByte(res));
}

void Cpu::execROR() {
  const uint16_t tmp = *effectiveOperandPtr_ | (registers.p.c ? 0x100 : 0x00);
  registers.p.c = tmp & 0x01;
  registers.p.computeNZ(*effectiveOperandPtr_ = loByte(tmp >> 1));
}

void Cpu::execAND() {
  registers.p.computeNZ(registers.a &= *effectiveOperandPtr_);
  applyPageBoundaryCrossingPenalty();
}

void Cpu::execORA() {
  registers.p.computeNZ(registers.a |= *effectiveOperandPtr_);
  applyPageBoundaryCrossingPenalty();
}

void Cpu::execEOR() {
  registers.p.computeNZ(registers.a ^= *effectiveOperandPtr_);
  applyPageBoundaryCrossingPenalty();
}

void Cpu::execCMP() {
  registers.p.computeNZC(registers.a - *effectiveOperandPtr_);
  applyPageBoundaryCrossingPenalty();
}

void Cpu::execCPX() {
  registers.p.computeNZC(registers.x - *effectiveOperandPtr_);
}

void Cpu::execCPY() {
  registers.p.computeNZC(registers.y - *effectiveOperandPtr_);
}

void Cpu::execBIT() {
  registers.p.computeNZ(registers.a & *effectiveOperandPtr_);
}

void Cpu::execSED() {
  registers.p.d = true;
}

void Cpu::execSEI() {
  registers.p.i = true;
}

void Cpu::execSEC() {
  registers.p.c = true;
}

void Cpu::execCLC() {
  registers.p.c = false;
}

void Cpu::execCLD() {
  registers.p.d = false;
}

void Cpu::execCLI() {
  registers.p.i = false;
}

void Cpu::execCLV() {
  registers.p.v = false;
}

void Cpu::execTAX() {
  registers.p.computeNZ(registers.x = registers.a);
}

void Cpu::execTXA() {
  registers.p.computeNZ(registers.a = registers.x);
}

void Cpu::execTAY() {
  registers.p.computeNZ(registers.y = registers.a);
}

void Cpu::execTYA() {
  registers.p.computeNZ(registers.a = registers.y);
}

void Cpu::execTSX() {
  registers.x = registers.sp.toByte();
}

void Cpu::execTXS() {
  registers.sp.fromByte(registers.x);
}

void Cpu::execPHA() {
  push8(registers.a);
}

void Cpu::execPLA() {
  registers.a = pull8();
}

void Cpu::execPHP() {
  push8(registers.p);
}

void Cpu::execPLP() {
  registers.p = pull8();
}

void Cpu::execNOP() {
  // literally do nothing :-)
}

void Cpu::execBCC() {
  if (!registers.p.c) branch();
}

void Cpu::execBCS() {
  if (registers.p.c) branch();
}

void Cpu::execBEQ() {
  if (registers.p.z) branch();
}

void Cpu::execBMI() {
  if (registers.p.n) branch();
}

void Cpu::execBNE() {
  if (!registers.p.z) branch();
}

void Cpu::execBPL() {
  if (!registers.p.n) branch();
}

void Cpu::execBVC() {
  if (!registers.p.v) branch();
}

void Cpu::execBVS() {
  if (registers.p.v) branch();
}

void Cpu::execJMP() {
  registers.pc = effectiveAddress_;
}

void Cpu::execJSR() {
  push16(registers.pc);
  registers.pc = effectiveAddress_;
}

void Cpu::execRTS() {
  registers.pc = pull16();
}

void Cpu::execRTI() {
  registers.p = pull8();
  registers.pc = pull16() + 1;
}

void Cpu::execBRK() {
  registers.pc++;
  push16(registers.pc);
  push8(registers.p.withBreakFlag());
  registers.pc = memory_.read16(VectorIRQ);
}

void Cpu::irq() {
  push16(registers.pc);
  push8(registers.p);
  registers.p.i = true;
  registers.pc = memory_.read16(VectorIRQ);
}

void Cpu::nmi() {
  push16(registers.pc);
  push8(registers.p);
  registers.p.i = true;
  registers.pc = memory_.read16(VectorNMI);
}

void Cpu::reset() {
  registers.pc = memory_.read16(VectorRESET);
  registers.a = 0;
  registers.x = 0;
  registers.y = 0;
  registers.sp = 0xfd;
  registers.p.n = false;
  registers.p.v = false;
  registers.p.d = false;
  registers.p.i = true;
  registers.p.z = false;
  registers.p.c = false;
}

void Cpu::execute(bool continuous) {
  executionStatus = Running;
  while (executionStatus != StopRequested) {
    const auto pc = &memory_[registers.pc];
    operandPtr_ = pc + 1;
    const auto& entry = DecodeTable[*pc];
    const auto ins = entry.instruction;

    if (ins->type == INV) {
      executionStatus = InvalidOpCode;
      return;
    }

    registers.pc += ins->size;
    cycles += ins->cycles;

    (this->*entry.prepareOperands)();
    (this->*entry.executeInstruction)();

    if (!continuous) break;
  }
  executionStatus = Stopped;
}
