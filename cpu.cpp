#include "cpu.h"
#include "decodetable.h"
#include <chrono>

Cpu::Cpu(Memory& memory) : m_memory(memory) {
}

void Cpu::prepImpliedOrAccumulatorMode() {
  m_effectiveOperandPtr.lo = &regs.a;
}

void Cpu::prepRelativeMode() {
  // all handled by branch instructions
}

void Cpu::prepIndirectMode() {
  m_effectiveAddress = m_memory.word(m_operandPtr.word());
}

void Cpu::prepImmediateMode() {
  m_effectiveOperandPtr.lo = m_operandPtr.lo;
}

void Cpu::prepZeroPageMode() {
  m_effectiveAddress = *m_operandPtr.lo;
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepZeroPageXMode() {
  calculateZeroPageEffectiveAddress(*m_operandPtr.lo, regs.x);
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepZeroPageYMode() {
  calculateZeroPageEffectiveAddress(*m_operandPtr.lo, regs.y);
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepIndexedIndirectXMode() {
  m_effectiveAddress = m_memory.word(static_cast<uint8_t>((*m_operandPtr.lo + regs.x)));
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepIndirectIndexedYMode() {
  calculateEffectiveAddress(m_memory.word(*m_operandPtr.lo), regs.y);
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepAbsoluteMode() {
  m_effectiveAddress = m_operandPtr.word();
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepAbsoluteXMode() {
  calculateEffectiveAddress(m_operandPtr.word(), regs.x);
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepAbsoluteYMode() {
  calculateEffectiveAddress(m_operandPtr.word(), regs.y);
  setEffectiveOperandPtrToAddress();
}

void Cpu::execLDA() {
  regs.p.computeNZ(regs.a = *m_effectiveOperandPtr.lo);
  if (m_pageBoundaryCrossed) m_extraCycles++;
}

void Cpu::execLDX() {
  regs.p.computeNZ(regs.x = *m_effectiveOperandPtr.lo);
  if (m_pageBoundaryCrossed) m_extraCycles++;
}

void Cpu::execLDY() {
  regs.p.computeNZ(regs.y = *m_effectiveOperandPtr.lo);
  if (m_pageBoundaryCrossed) m_extraCycles++;
}

void Cpu::execSTA() {
  *m_effectiveOperandPtr.lo = regs.a;
}

void Cpu::execSTX() {
  *m_effectiveOperandPtr.lo = regs.x;
}

void Cpu::execSTY() {
  *m_effectiveOperandPtr.lo = regs.y;
}

static bool decimalCorrectionAndCarry(uint16_t& result) {
  if ((result & 0x0f) > 0x09) result += 0x06;
  if ((result & 0xf0) > 0x90) {
    result += 0x60;
    return true;
  }
  return false;
}

void Cpu::execADC() {
  const uint8_t op2 = *m_effectiveOperandPtr.lo;
  uint16_t result = regs.a + op2 + static_cast<uint8_t>(regs.p.carry);
  if (regs.p.decimal) {
    regs.p.carry = decimalCorrectionAndCarry(result);
    regs.p.computeNZ(result);
  } else {
    regs.p.computeNZC(result);
  }
  regs.p.computeV(regs.a, op2, result);
  regs.a = static_cast<uint8_t>(result);
  if (m_pageBoundaryCrossed) m_extraCycles++;
}

void Cpu::execSBC() {
  const uint8_t op2 = *m_effectiveOperandPtr.lo ^ 0xff;
  uint16_t result = regs.a + op2 + static_cast<uint8_t>(regs.p.carry);
  if (regs.p.decimal) {
    result -= 0x66;
    regs.p.carry = decimalCorrectionAndCarry(result);
    regs.p.computeNZ(result);
  } else {
    regs.p.computeNZC(result);
  }
  regs.p.computeV(regs.a, op2, result);
  regs.a = static_cast<uint8_t>(result);
  if (m_pageBoundaryCrossed) m_extraCycles++;
}

void Cpu::execINC() {
  regs.p.computeNZ(++(*m_effectiveOperandPtr.lo));
}

void Cpu::execINX() {
  regs.p.computeNZ(++regs.x);
}

void Cpu::execINY() {
  regs.p.computeNZ(++regs.y);
}

void Cpu::execDEC() {
  regs.p.computeNZ(--(*m_effectiveOperandPtr.lo));
}

void Cpu::execDEX() {
  regs.p.computeNZ(--regs.x);
}

void Cpu::execDEY() {
  regs.p.computeNZ(--regs.y);
}

void Cpu::execASL() {
  auto val = *m_effectiveOperandPtr.lo;
  regs.p.carry = val & 0x80;
  regs.p.computeNZ(*m_effectiveOperandPtr.lo = static_cast<uint8_t>(val << 1));
}

void Cpu::execLSR() {
  auto val = *m_effectiveOperandPtr.lo;
  regs.p.carry = val & 0x01;
  regs.p.computeNZ(*m_effectiveOperandPtr.lo = static_cast<uint8_t>(val >> 1));
}

void Cpu::execROL() {
  const uint16_t res = static_cast<uint16_t>(*m_effectiveOperandPtr.lo << 1) | regs.p.carry;
  regs.p.carry = res & 0x100;
  regs.p.computeNZ(*m_effectiveOperandPtr.lo = static_cast<uint8_t>(res));
}

void Cpu::execROR() {
  const uint16_t tmp = *m_effectiveOperandPtr.lo | (regs.p.carry ? 0x100 : 0x00);
  regs.p.carry = tmp & 0x01;
  regs.p.computeNZ(*m_effectiveOperandPtr.lo = static_cast<uint8_t>(tmp >> 1));
}

void Cpu::execAND() {
  regs.p.computeNZ(regs.a &= *m_effectiveOperandPtr.lo);
  if (m_pageBoundaryCrossed) m_extraCycles++;
}

void Cpu::execORA() {
  regs.p.computeNZ(regs.a |= *m_effectiveOperandPtr.lo);
  if (m_pageBoundaryCrossed) m_extraCycles++;
}

void Cpu::execEOR() {
  regs.p.computeNZ(regs.a ^= *m_effectiveOperandPtr.lo);
  if (m_pageBoundaryCrossed) m_extraCycles++;
}

void Cpu::execCMP() {
  execCompare(regs.a);
  if (m_pageBoundaryCrossed) m_extraCycles++;
}

void Cpu::execCPX() {
  execCompare(regs.x);
}

void Cpu::execCPY() {
  execCompare(regs.y);
}

void Cpu::execBIT() {
  const auto operand = *m_effectiveOperandPtr.lo;
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
  regs.pc = m_effectiveAddress;
}

void Cpu::execJSR() {
  pushWord(regs.pc - 1);
  regs.pc = m_effectiveAddress;
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
  regs.pc = m_memory.word(CpuAddress::IrqVector);
}

void Cpu::irq() {
  pushWord(regs.pc);
  push(regs.p);
  regs.p.interrupt = true;
  regs.pc = m_memory.word(CpuAddress::IrqVector);
  m_state.mode = CpuState::Mode::Normal;
}

void Cpu::nmi() {
  pushWord(regs.pc);
  push(regs.p);
  regs.p.interrupt = true;
  regs.pc = m_memory.word(CpuAddress::NmiVector);
  m_state.mode = CpuState::Mode::Normal;
}

void Cpu::reset() {
  regs.pc = m_memory.word(CpuAddress::ResetVector);
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
  m_pageBoundaryCrossed = false;
  m_extraCycles = 0;
  m_state.mode = CpuState::Mode::Normal;
}

void Cpu::resetExecutionState() {
  if (m_state.execution == CpuState::Execution::Halted || m_state.execution == CpuState::Execution::Stopped)
    m_state.execution = CpuState::Execution::Idle;
}

void Cpu::stopExecution() {
  if (m_state.execution == CpuState::Execution::Running) m_state.execution = CpuState::Execution::Stopping;
}

void Cpu::preExecute() {
  m_state.execution = CpuState::Execution::Running;
}

int Cpu::execute() {
  m_extraCycles = 0;
  m_pageBoundaryCrossed = false;
  const auto pcPtr = &m_memory[regs.pc];
  m_operandPtr.lo = &m_memory[regs.pc + 1];
  m_operandPtr.hi = &m_memory[regs.pc + 2];
  const auto& entry = DecodeTable[*pcPtr];

  regs.pc += entry.instruction->size;

  (this->*entry.prepareOperands)();
  (this->*entry.executeInstruction)();

  switch (m_state.mode) {
  case CpuState::Mode::Normal: break;
  case CpuState::Mode::PendingReset: reset(); break;
  case CpuState::Mode::PendingNmi: nmi(); break;
  case CpuState::Mode::PendingIrq: irq(); break;
  }
  return entry.instruction->cycles + m_extraCycles;
}

void Cpu::postExecute() {
  switch (m_state.execution) {
  case CpuState::Execution::Running: m_state.execution = CpuState::Execution::Idle; break;
  case CpuState::Execution::Stopping: m_state.execution = CpuState::Execution::Stopped; break;
  default: break;
  }
}

void Cpu::execKIL() {
  regs.pc--;
  m_state.execution = CpuState::Execution::Halted;
}

void Cpu::triggerReset() {
  if (m_state.mode < CpuState::Mode::PendingReset) {
    if (running()) {
      m_state.mode = CpuState::Mode::PendingReset;
    } else
      reset();
  }
}

void Cpu::triggerNmi() {
  if (m_state.mode < CpuState::Mode::PendingNmi) {
    if (running()) {
      m_state.mode = CpuState::Mode::PendingNmi;
    } else {
      nmi();
    }
  }
}

void Cpu::triggerIrq() {
  if (m_state.mode < CpuState::Mode::PendingIrq && !regs.p.interrupt) {
    if (running()) {
      m_state.mode = CpuState::Mode::PendingIrq;
    } else {
      irq();
    }
  }
}
