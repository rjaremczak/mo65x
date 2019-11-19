#include "cpu.h"
#include "decodetable.h"

Cpu::Cpu(Memory& memory) : memory_(memory) {
}

void Cpu::prepImpliedMode() {
  // nothing to do
}

void Cpu::prepAccumulatorMode() {
  effectiveOperandPtr_.lo = &regs.a;
}

void Cpu::prepRelativeMode() {
  // all handled by branch instructions
}

void Cpu::prepIndirectMode() {
  effectiveAddress_ = memory_.read16(operandPtr_.word());
}

void Cpu::prepImmediateMode() {
  effectiveOperandPtr_.lo = operandPtr_.lo;
}

void Cpu::prepZeroPageMode() {
  effectiveAddress_ = *operandPtr_.lo;
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepZeroPageXMode() {
  calculateZeroPageEffectiveAddress(*operandPtr_.lo, regs.x);
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepZeroPageYMode() {
  calculateZeroPageEffectiveAddress(*operandPtr_.lo, regs.y);
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepIndexedIndirectXMode() {
  effectiveAddress_ = memory_.read16(static_cast<uint8_t>((*operandPtr_.lo + regs.x)));
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepIndirectIndexedYMode() {
  calculateEffectiveAddress(memory_.read16(*operandPtr_.lo), regs.y);
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepAbsoluteMode() {
  effectiveAddress_ = operandPtr_.word();
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepAbsoluteXMode() {
  calculateEffectiveAddress(operandPtr_.word(), regs.x);
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepAbsoluteYMode() {
  calculateEffectiveAddress(operandPtr_.word(), regs.y);
  setEffectiveOperandPtrToAddress();
}

void Cpu::execLDA() {
  regs.p.computeNZ(regs.a = *effectiveOperandPtr_.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execLDX() {
  regs.p.computeNZ(regs.x = *effectiveOperandPtr_.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execLDY() {
  regs.p.computeNZ(regs.y = *effectiveOperandPtr_.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execSTA() {
  *effectiveOperandPtr_.lo = regs.a;
}

void Cpu::execSTX() {
  *effectiveOperandPtr_.lo = regs.x;
}

void Cpu::execSTY() {
  *effectiveOperandPtr_.lo = regs.y;
}

void Cpu::execADC() {
  execAddWithCarry(*effectiveOperandPtr_.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execSBC() {
  execAddWithCarry(*effectiveOperandPtr_.lo ^ 0xff);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execINC() {
  regs.p.computeNZ(++(*effectiveOperandPtr_.lo));
}

void Cpu::execINX() {
  regs.p.computeNZ(++regs.x);
}

void Cpu::execINY() {
  regs.p.computeNZ(++regs.y);
}

void Cpu::execDEC() {
  regs.p.computeNZ(--(*effectiveOperandPtr_.lo));
}

void Cpu::execDEX() {
  regs.p.computeNZ(--regs.x);
}

void Cpu::execDEY() {
  regs.p.computeNZ(--regs.y);
}

void Cpu::execASL() {
  auto val = *effectiveOperandPtr_.lo;
  regs.p.carry = val & 0x80;
  regs.p.computeNZ(*effectiveOperandPtr_.lo = static_cast<uint8_t>(val << 1));
}

void Cpu::execLSR() {
  auto val = *effectiveOperandPtr_.lo;
  regs.p.carry = val & 0x01;
  regs.p.computeNZ(*effectiveOperandPtr_.lo = static_cast<uint8_t>(val >> 1));
}

void Cpu::execROL() {
  const uint16_t res = static_cast<uint16_t>(*effectiveOperandPtr_.lo << 1) | regs.p.carry;
  regs.p.carry = res & 0x100;
  regs.p.computeNZ(*effectiveOperandPtr_.lo = static_cast<uint8_t>(res));
}

void Cpu::execROR() {
  const uint16_t tmp = *effectiveOperandPtr_.lo | (regs.p.carry ? 0x100 : 0x00);
  regs.p.carry = tmp & 0x01;
  regs.p.computeNZ(*effectiveOperandPtr_.lo = static_cast<uint8_t>(tmp >> 1));
}

void Cpu::execAND() {
  regs.p.computeNZ(regs.a &= *effectiveOperandPtr_.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execORA() {
  regs.p.computeNZ(regs.a |= *effectiveOperandPtr_.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execEOR() {
  regs.p.computeNZ(regs.a ^= *effectiveOperandPtr_.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execCompare() {
  execCompare(regs.a);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execCPX() {
  execCompare(regs.x);
}

void Cpu::execCPY() {
  execCompare(regs.y);
}

void Cpu::execBIT() {
  const auto operand = *effectiveOperandPtr_.lo;
  regs.p.zero = !(regs.a & operand);
  regs.p.negative = operand & 0x80;
  regs.p.overflow = operand & 0x40;
}

void Cpu::execSED() {
  regs.p.decimal = true;
}

void Cpu::execSEI() {
  regs.p.interrupt = true;
}

void Cpu::execSEC() {
  regs.p.carry = true;
}

void Cpu::execCLC() {
  regs.p.carry = false;
}

void Cpu::execCLD() {
  regs.p.decimal = false;
}

void Cpu::execCLI() {
  regs.p.interrupt = false;
}

void Cpu::execCLV() {
  regs.p.overflow = false;
}

void Cpu::execTAX() {
  regs.p.computeNZ(regs.x = regs.a);
}

void Cpu::execTXA() {
  regs.p.computeNZ(regs.a = regs.x);
}

void Cpu::execTAY() {
  regs.p.computeNZ(regs.y = regs.a);
}

void Cpu::execTYA() {
  regs.p.computeNZ(regs.a = regs.y);
}

void Cpu::execTSX() {
  regs.p.computeNZ(regs.x = regs.sp.offset);
}

void Cpu::execTXS() {
  regs.sp.offset = regs.x;
}

void Cpu::execPHA() {
  push(regs.a);
}

void Cpu::execPLA() {
  regs.p.computeNZ(regs.a = pull());
}

void Cpu::execPHP() {
  push(regs.p);
}

void Cpu::execPLP() {
  regs.p = pull();
}

void Cpu::execNOP() {
  // literally do nothing :-)
}

void Cpu::execBCC() {
  if (!regs.p.carry) execBranch();
}

void Cpu::execBCS() {
  if (regs.p.carry) execBranch();
}

void Cpu::execBEQ() {
  if (regs.p.zero) execBranch();
}

void Cpu::execBMI() {
  if (regs.p.negative) execBranch();
}

void Cpu::execBNE() {
  if (!regs.p.zero) execBranch();
}

void Cpu::execBPL() {
  if (!regs.p.negative) execBranch();
}

void Cpu::execBVC() {
  if (!regs.p.overflow) execBranch();
}

void Cpu::execBVS() {
  if (regs.p.overflow) execBranch();
}

void Cpu::execJMP() {
  regs.pc = effectiveAddress_;
}

void Cpu::execJSR() {
  pushWord(regs.pc - 1);
  regs.pc = effectiveAddress_;
}

void Cpu::execRTS() {
  regs.pc = pullWord() + 1;
}

void Cpu::execRTI() {
  regs.p = pull();
  regs.pc = pullWord();
  regs.p.interrupt = false;
}

void Cpu::execBRK() {
  pushWord(regs.pc + 1);
  push(regs.p | ProcessorStatus::BreakBitMask);
  regs.p.interrupt = true;
  regs.pc = memory_.read16(irqVector);
}

void Cpu::irq() {
  pushWord(regs.pc);
  push(regs.p);
  regs.p.interrupt = true;
  regs.pc = memory_.read16(irqVector);
}

void Cpu::nmi() {
  pushWord(regs.pc);
  push(regs.p);
  regs.p.interrupt = true;
  regs.pc = memory_.read16(nmiVector);
}

void Cpu::reset() {
  regs.pc = memory_.read16(resetVector);
  regs.a = 0;
  regs.x = 0;
  regs.y = 0;
  regs.sp.offset = 0xfd;
  regs.p.negative = false;
  regs.p.overflow = false;
  regs.p.decimal = false;
  regs.p.interrupt = true;
  regs.p.zero = false;
  regs.p.carry = false;
}

void Cpu::execHalt() {
  regs.pc--;
  state = ExecutionState::Halted;
}

void Cpu::execute(bool continuous) {
  state = ExecutionState::Running;
  while (state == ExecutionState::Running) {
    pageBoundaryCrossed_ = false;
    const auto pcPtr = &memory_[regs.pc];
    operandPtr_.lo = &memory_[regs.pc + 1];
    operandPtr_.hi = &memory_[regs.pc + 2];
    const auto& entry = DecodeTable[*pcPtr];
    const auto ins = entry.instruction;

    regs.pc += ins->size;
    cycles += ins->cycles;

    (this->*entry.prepareOperands)();
    (this->*entry.executeInstruction)();

    if (!continuous) break;
  }
  switch (state) {
  case ExecutionState::Running: state = ExecutionState::Idle; break;
  case ExecutionState::Stopping: state = ExecutionState::Stopped; break;
  case ExecutionState::Halting: state = ExecutionState::Halted; break;
  default: break;
  }
}

void Cpu::triggerReset() {
  if (running()) {
    pendingReset_ = true;
  } else {
    reset();
  }
}

void Cpu::triggerNmi() {
  if (running()) {
    pendingNmi_ = true;
  } else {
    nmi();
  }
}

void Cpu::triggerIrq() {
  if (regs.p.interrupt) return;
  if (!running()) {
    pendingIrq_ = true;
  } else {
    irq();
  }
}
