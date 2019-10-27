#pragma once

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

  CpuRegisters registers;
  int cycles;
  volatile ExecutionStatus executionStatus = Stopped;

private:
  Memory& memory_;
  uint8_t* instruction;
  uint8_t* operand;
  uint8_t* effectiveOperand;
  uint16_t effectiveAddress;
  bool pageBoundaryCrossed;

  uint8_t operand8() { return *operand; }

  uint16_t operand16() { return wordOf(operand[0], operand[1]); }

  void push(uint8_t v) { memory_[registers.sp.value--] = v; }

  uint8_t pull() { return memory_[++registers.sp.value]; }

  void setEffectiveAddressAndOperand(uint16_t address) {
    effectiveAddress = address;
    effectiveOperand = &memory_[address];
  }

  void setEffectiveAddressAndOperand(uint16_t address, int16_t displacement) {
    const auto result = address + displacement;
    pageBoundaryCrossed = (address ^ result) & 0xff00;
    effectiveAddress = (address & 0xff00) | (result & 0x00ff);
    effectiveOperand = &memory_[effectiveAddress];
  }

  void applyPageBoundaryCrossingPenalty() {
    if (pageBoundaryCrossed) cycles++;
  }

  void branch() {
    cycles++;
    setEffectiveAddressAndOperand(registers.pc, static_cast<int8_t>(operand8()));
    registers.pc = effectiveAddress;
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
  void execute(bool continuous = false);
  void nmi();
  void irq();
  void reset();
};
