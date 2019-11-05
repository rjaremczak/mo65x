#pragma once

#include "executionstatus.h"
#include "instruction.h"
#include "memory.h"
#include "registers.h"
#include "utilities.h"
#include <map>

class Cpu {
public:
  enum : uint16_t { VectorNMI = 0xfffa, VectorRESET = 0xfffc, VectorIRQ = 0xfffe };
  using Handler = void (Cpu::*)();

  friend constexpr Handler operandsHandler(AddressingMode);
  friend constexpr Handler instructionHandler(InstructionType);

  Registers registers;
  int cycles;
  volatile ExecutionStatus executionStatus = Stopped;

  Cpu(Memory&);
  void execute(bool continuous = false);
  void nmi();
  void irq();
  void reset();

private:
  friend class OpCodesTest;

  Memory& memory_;
  uint8_t* operandPtr_;
  uint8_t* effectiveOperandPtr_;
  uint16_t effectiveAddress_;
  bool pageBoundaryCrossed_;

  uint16_t operand16() const { return wordOf(operandPtr_[0], operandPtr_[1]); }

  void push8(uint8_t b) { memory_[registers.sp.value--] = b; }
  uint8_t pull8() { return memory_[++registers.sp.value]; }

  void push16(uint16_t w) {
    push8(hiByte(w));
    push8(loByte(w));
  }

  uint16_t pull16() { return wordOf(pull8(), pull8()); }

  void calculateZeroPageEffectiveAddress(uint8_t address, uint8_t offset) {
    const uint8_t result = address + offset;
    effectiveAddress_ = result;
  }

  void calculateEffectiveAddress(uint16_t address, int16_t offset) {
    effectiveAddress_ = static_cast<uint16_t>(address + offset);
    pageBoundaryCrossed_ = (address ^ effectiveAddress_) & 0xff00;
  }

  void setEffectiveOperandPtrToAddress() { effectiveOperandPtr_ = &memory_[effectiveAddress_]; }

  void applyPageBoundaryCrossingPenalty() {
    if (pageBoundaryCrossed_) cycles++;
  }

  void execBranch() {
    cycles++;
    calculateEffectiveAddress(registers.pc, static_cast<int8_t>(*operandPtr_));
    registers.pc = effectiveAddress_;
  }

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
