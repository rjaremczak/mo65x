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

  uint8_t operand8() const { return *operandPtr_; }
  uint16_t operand16() const { return wordOf(operandPtr_[0], operandPtr_[1]); }

  void push8(uint8_t b) { memory_[registers.sp--] = b; }
  uint8_t pull8() { return memory_[++registers.sp]; }

  void push16(uint16_t w) {
    push8(hiByte(w));
    push8(loByte(w));
  }

  uint16_t pull16() { return wordOf(pull8(), pull8()); }

  void setEffectiveAddressAndOperand(uint16_t address) {
    effectiveAddress_ = address;
    effectiveOperandPtr_ = &memory_[address];
  }

  void setEffectiveAddressAndOperand(uint16_t address, int16_t displacement) {
    const auto result = address + displacement;
    pageBoundaryCrossed_ = (address ^ result) & 0xff00;
    effectiveAddress_ = (address & 0xff00) | (result & 0x00ff);
    effectiveOperandPtr_ = &memory_[effectiveAddress_];
  }

  void applyPageBoundaryCrossingPenalty() {
    if (pageBoundaryCrossed_) cycles++;
  }

  void branch() {
    cycles++;
    setEffectiveAddressAndOperand(registers.pc, static_cast<int8_t>(operand8()));
    registers.pc = effectiveAddress_;
  }

  void amImplied();
  void amAccumulator();
  void amRelative();
  void amImmediate();
  void amZeroPage();
  void amZeroPageX();
  void amZeroPageY();
  void amIndexedIndirectX();
  void amIndirectIndexedY();
  void amIndirect();
  void amAbsolute();
  void amAbsoluteX();
  void amAbsoluteY();

  void insLDA();
  void insLDX();
  void insLDY();
  void insSTA();
  void insSTX();
  void insSTY();

  void insADC();
  void insSBC();
  void insINC();
  void insINX();
  void insINY();
  void insDEC();
  void insDEX();
  void insDEY();

  void insASL();
  void insLSR();
  void insROL();
  void insROR();

  void insAND();
  void insORA();
  void insEOR();

  void insCMP();
  void insCPX();
  void insCPY();
  void insBIT();

  void insSED();
  void insSEI();
  void insSEC();
  void insCLC();
  void insCLD();
  void insCLI();
  void insCLV();

  void insTAX();
  void insTXA();
  void insTAY();
  void insTYA();
  void insTSX();
  void insTXS();

  void insPHA();
  void insPLA();
  void insPHP();
  void insPLP();

  void insRTS();
  void insRTI();
  void insBRK();
  void insNOP();

  void insBCC();
  void insBCS();
  void insBEQ();
  void insBMI();
  void insBNE();
  void insBPL();
  void insBVC();
  void insBVS();
  void insJMP();
  void insJSR();
};
