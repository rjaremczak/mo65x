#pragma once

#include "cpuinfo.h"
#include "executionstate.h"
#include "instruction.h"
#include "memory.h"
#include "operandptr.h"
#include "registers.h"
#include <atomic>
#include <map>

class Cpu {
public:
  constexpr static uint16_t IOPortConfig = 0x00;
  constexpr static uint16_t IOPortData = 0x01;
  constexpr static uint16_t VectorNMI = 0xfffa;
  constexpr static uint16_t VectorRESET = 0xfffc;
  constexpr static uint16_t VectorIRQ = 0xfffe;

  using Handler = void (Cpu::*)();

  friend constexpr Handler operandsHandler(AddressingMode);
  friend constexpr Handler instructionHandler(InstructionType);

  ExecutionState state = ExecutionState::Stopped;
  Registers regs;
  int cycles = 0;

  Cpu(Memory&);
  void execute(bool continuous = false);
  CpuInfo info() const { return {state, regs, cycles}; }
  bool running() const { return state == ExecutionState::Running; }
  void requestIRQ();
  void requestNMI();
  void requestRESET();

private:
  friend class OpCodesTest;

  std::atomic_bool pendingIRQ_ = false;
  std::atomic_bool pendingNMI_ = false;
  std::atomic_bool pendingRESET_ = false;

  Memory& memory_;
  OperandPtr operandPtr_;
  OperandPtr effectiveOperandPtr_;
  uint16_t effectiveAddress_;
  bool pageBoundaryCrossed_;

  void push(uint8_t b) {
    memory_[regs.sp.address()] = b;
    regs.sp.decrement();
  }

  uint8_t pull() {
    regs.sp.increment();
    return memory_[regs.sp.address()];
  }

  void pushWord(uint16_t word) {
    push(word >> 8);
    push(static_cast<uint8_t>(word));
  }

  uint16_t pullWord() { return uint16_t(pull() | pull() << 8); }

  void calculateZeroPageEffectiveAddress(uint8_t address, uint8_t offset) {
    const uint8_t result = address + offset;
    effectiveAddress_ = result;
  }

  void calculateEffectiveAddress(uint16_t address, int16_t offset) {
    effectiveAddress_ = static_cast<uint16_t>(address + offset);
    pageBoundaryCrossed_ = (address ^ effectiveAddress_) & 0xff00;
  }

  void setEffectiveOperandPtrToAddress() { effectiveOperandPtr_.lo = &memory_[effectiveAddress_]; }

  void applyExtraCycleOnPageBoundaryCrossing() {
    if (pageBoundaryCrossed_) ++cycles;
  }

  void execBranch() {
    cycles++;
    calculateEffectiveAddress(regs.pc, static_cast<int8_t>(*operandPtr_.lo));
    regs.pc = effectiveAddress_;
    applyExtraCycleOnPageBoundaryCrossing();
  }

  void execAddWithCarry(uint8_t op2) {
    const uint16_t result = regs.a + op2 + uint8_t(regs.p.carry);
    regs.p.computeNZC(result);
    regs.p.computeV(regs.a, op2, result);
    regs.a = uint8_t(result);
  }

  void execCompare(uint8_t op1) { regs.p.computeNZC(op1 + (*effectiveOperandPtr_.lo ^ 0xff) + uint8_t(1)); }

  void nmi();
  void irq();
  void reset();

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
