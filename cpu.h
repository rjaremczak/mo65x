#pragma once

#include "cpuflags.h"
#include "cpuregisters.h"
#include "executionstatus.h"
#include "instruction.h"
#include "memory.h"
#include "utilities.h"
#include <map>

class Cpu {
public:
  enum : uint16_t { VectorNMI = 0xfffa, VectorRESET = 0xfffc, VectorIRQ = 0xfffe };

  using Handler = void (Cpu::*)();
  static Handler operandsHandler(AddressingMode);
  static Handler instructionHandler(InstructionType);

private:
  volatile ExecutionStatus executionStatus_ = Idle;
  Memory& memory_;
  CpuRegisters registers_;
  CpuFlags flags_;
  uint64_t cycles_ = 0;
  uint8_t* instruction_;
  uint8_t* operand_;
  uint8_t* effectiveOperand_;
  uint16_t effectiveAddress_;
  bool pageBoundaryCrossed_;

  uint8_t operand8() { return *operand_; }

  uint16_t operand16() { return wordOf(operand_[0], operand_[1]); }

  void push(uint8_t v) { memory_[registers_.sp.value--] = v; }

  uint8_t pull() { return memory_[++registers_.sp.value]; }

  void setEffectiveAddressAndOperand(uint16_t address) {
    effectiveAddress_ = address;
    effectiveOperand_ = &memory_[address];
  }

  void setEffectiveAddressAndOperand(uint16_t address, int16_t displacement) {
    const auto result = address + displacement;
    pageBoundaryCrossed_ = (address ^ result) & 0xff00;
    effectiveAddress_ = (address & 0xff00) | (result & 0x00ff);
    effectiveOperand_ = &memory_[effectiveAddress_];
  }

  void applyPageBoundaryCrossingPenalty() {
    if (pageBoundaryCrossed_) cycles_++;
  }

  void branch() {
    cycles_++;
    setEffectiveAddressAndOperand(registers_.pc, static_cast<int8_t>(operand8()));
    registers_.pc = effectiveAddress_;
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

  friend class CpuProbe;

public:
  Cpu(Memory&);
  auto cycles() const { return cycles_; }
  void resetCycles() { cycles_ = 0; }
  void execute(bool continuous = false);
  void nmi();
  void irq();
  void reset();
};
