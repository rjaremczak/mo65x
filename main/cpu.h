#pragma once

#include "cpustate.h"
#include "cpustatistics.h"
#include "instruction.h"
#include "memory.h"
#include "operandptr.h"
#include "registers.h"
#include <chrono>
#include <commondefs.h>
#include <map>

class Cpu {
public:
  using Handler = void (Cpu::*)();

  friend constexpr Handler operandsHandler(OperandsFormat);
  friend constexpr Handler instructionHandler(InstructionType);

  Registers regs;

  Cpu(Memory&);
  bool running() const { return m_state.running(); }
  void reset();
  void resetExecutionState();
  void stopExecution();
  void preRun();
  int stepRun();
  void postRun();
  void triggerReset();
  void triggerNmi();
  void triggerIrq();
  CpuState state() const { return m_state; }

private:
  CpuState m_state;
  Memory& m_memory;
  OperandPtr m_operandPtr;
  OperandPtr m_effectiveOperandPtr;
  uint16_t m_effectiveAddress;
  bool m_pageBoundaryCrossed;
  int m_extraCycles;

  void push(uint8_t b) {
    m_memory[regs.sp.address()] = b;
    regs.sp.offset--;
  }

  uint8_t pull() {
    regs.sp.offset++;
    return m_memory[regs.sp.address()];
  }

  void pushWord(uint16_t word) {
    push(word >> 8);
    push(static_cast<uint8_t>(word));
  }

  uint16_t pullWord() { return uint16_t(pull() | pull() << 8); }

  void calculateZeroPageEffectiveAddress(uint8_t address, uint8_t offset) {
    const uint8_t result = address + offset;
    m_effectiveAddress = result;
  }

  void calculateEffectiveAddress(uint16_t address, int16_t offset) {
    m_effectiveAddress = static_cast<uint16_t>(address + offset);
    m_pageBoundaryCrossed = (address ^ m_effectiveAddress) & 0xff00;
  }

  void setEffectiveOperandPtrToAddress() { m_effectiveOperandPtr.lo = &m_memory[m_effectiveAddress]; }

  void execBranch() {
    m_extraCycles++;
    calculateEffectiveAddress(regs.pc, static_cast<int8_t>(*m_operandPtr.lo));
    regs.pc = m_effectiveAddress;
    if (m_pageBoundaryCrossed) m_extraCycles++;
  }

  void execCompare(uint8_t op1) { regs.p.computeNZC(op1 + (*m_effectiveOperandPtr.lo ^ 0xff) + uint8_t(1)); }

  void nmi();
  void irq();
  void execKIL();

  void prepImpliedOrAccumulatorMode();
  void prepRelativeMode();
  void prepImmediateMode();
  void prepZeroPageMode();
  void prepZeroPageXMode();
  void prepZeroPageYMode();
  void prepIndexedIndirectXMode();
  void prepIndirectIndexedYMode();
  void prepIndirectMode();
  void prepAbsoluteMode();
  void prepAbsoluteXMode();
  void prepAbsoluteYMode();

  void execLDA();
  void execLDX();
  void execLDY();
  void execSTA();
  void execSTX();
  void execSTY();

  void execADC();
  void execSBC();
  void execINC();
  void execINX();
  void execINY();
  void execDEC();
  void execDEX();
  void execDEY();

  void execASL();
  void execLSR();
  void execROL();
  void execROR();

  void execAND();
  void execORA();
  void execEOR();

  void execCMP();
  void execCPX();
  void execCPY();
  void execBIT();

  void execSED();
  void execSEI();
  void execSEC();
  void execCLC();
  void execCLD();
  void execCLI();
  void execCLV();

  void execTAX();
  void execTXA();
  void execTAY();
  void execTYA();
  void execTSX();
  void execTXS();

  void execPHA();
  void execPLA();
  void execPHP();
  void execPLP();

  void execRTS();
  void execRTI();
  void execBRK();
  void execNOP();

  void execBCC();
  void execBCS();
  void execBEQ();
  void execBMI();
  void execBNE();
  void execBPL();
  void execBVC();
  void execBVS();
  void execJMP();
  void execJSR();
};
