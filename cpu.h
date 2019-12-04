#pragma once

#include "cpuinfo.h"
#include "cpustate.h"
#include "instruction.h"
#include "memory.h"
#include "operandptr.h"
#include "registers.h"
#include "runlevel.h"
#include <atomic>
#include <chrono>
#include <commondefs.h>
#include <map>

class Cpu {
public:
  using Handler = void (Cpu::*)();

  friend class InstructionsTest;
  friend constexpr Handler operandsHandler(OperandsFormat);
  friend constexpr Handler instructionHandler(InstructionType);

  Registers regs;

  Cpu(Memory&);
  bool running() const { return state == CpuState::Running; }
  void reset();
  void resetExecutionState();
  void resetStatistics();
  void stopExecution();
  void execute(bool continuous, Duration period = Duration(1000));
  void triggerReset();
  void triggerNmi();
  void triggerIrq();
  CpuInfo info() const;

private:
  CpuRunLevel runLevel = CpuRunLevel::Normal;
  CpuState state = CpuState::Idle;
  long cycles;
  Duration duration;

  Memory& memory;
  OperandPtr operandPtr;
  OperandPtr effectiveOperandPtr;
  uint16_t effectiveAddress;
  bool pageBoundaryCrossed;

  void push(uint8_t b) {
    memory[regs.sp.address()] = b;
    regs.sp.offset--;
  }

  uint8_t pull() {
    regs.sp.offset++;
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

  void execBranch() {
    cycles++;
    calculateEffectiveAddress(regs.pc, static_cast<int8_t>(*operandPtr.lo));
    regs.pc = effectiveAddress;
    if (pageBoundaryCrossed) cycles++;
  }

  void execCompare(uint8_t op1) { regs.p.computeNZC(op1 + (*effectiveOperandPtr.lo ^ 0xff) + uint8_t(1)); }

  void nmi();
  void irq();
  void execKIL();

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
