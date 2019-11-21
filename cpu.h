#pragma once

#include "cpuinfo.h"
#include "executionstate.h"
#include "instruction.h"
#include "memory.h"
#include "operandptr.h"
#include "registers.h"
#include "runlevel.h"
#include <atomic>
#include <chrono>
#include <defs.h>
#include <map>

class Cpu {
public:
  constexpr static uint16_t IoPortConfig = 0x00;
  constexpr static uint16_t IoPortData = 0x01;
  constexpr static uint16_t nmiVector = 0xfffa;
  constexpr static uint16_t resetVector = 0xfffc;
  constexpr static uint16_t irqVector = 0xfffe;

  using Handler = void (Cpu::*)();

  friend class InstructionsTest;
  friend constexpr Handler operandsHandler(OperandsFormat);
  friend constexpr Handler instructionHandler(InstructionType);

  Registers regs;
  long cycles;
  Duration duration;

  Cpu(Memory&);
  bool running() const { return state == CpuState::Running; }
  void reset();
  void resetExecutionState();
  void resetStatistics();
  void stopExecution();
  void execute(bool continuous);
  void triggerReset();
  void triggerNmi();
  void triggerIrq();
  CpuInfo info() const;

private:
  std::atomic<CpuRunLevel> runLevel = CpuRunLevel::Program;
  std::atomic<CpuState> state = CpuState::Idle;

  Memory& memory;
  OperandPtr operandPtr;
  OperandPtr effectiveOperandPtr;
  uint16_t effectiveAddress;
  bool pageBoundaryCrossed;

  void push(uint8_t b) {
    memory[regs.sp.address()] = b;
    regs.sp.decrement();
  }

  uint8_t pull() {
    regs.sp.increment();
    return memory[regs.sp.address()];
  }

  void pushWord(uint16_t word) {
    push(word >> 8);
    push(static_cast<uint8_t>(word));
  }

  uint16_t pullWord() { return uint16_t(pull() | pull() << 8); }

  void calculateZeroPageEffectiveAddress(uint8_t address, uint8_t offset) {
    const uint8_t result = address + offset;
    effectiveAddress = result;
  }

  void calculateEffectiveAddress(uint16_t address, int16_t offset) {
    effectiveAddress = static_cast<uint16_t>(address + offset);
    pageBoundaryCrossed = (address ^ effectiveAddress) & 0xff00;
  }

  void setEffectiveOperandPtrToAddress() { effectiveOperandPtr.lo = &memory[effectiveAddress]; }

  void applyExtraCycleOnPageBoundaryCrossing() {
    if (pageBoundaryCrossed) ++cycles;
  }

  void execBranch() {
    cycles++;
    calculateEffectiveAddress(regs.pc, static_cast<int8_t>(*operandPtr.lo));
    regs.pc = effectiveAddress;
    applyExtraCycleOnPageBoundaryCrossing();
  }

  void execAddWithCarry(uint8_t op2) {
    const uint16_t result = regs.a + op2 + uint8_t(regs.p.carry);
    regs.p.computeNZC(result);
    regs.p.computeV(regs.a, op2, result);
    regs.a = uint8_t(result);
  }

  void execCompare(uint8_t op1) { regs.p.computeNZC(op1 + (*effectiveOperandPtr.lo ^ 0xff) + uint8_t(1)); }

  void nmi();
  void irq();
  void execHalt();

  void prepImpliedMode();
  void prepAccumulatorMode();
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

  void execCompare();
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
