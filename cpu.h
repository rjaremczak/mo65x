#pragma once

#include "cpuflags.h"
#include "cpuregisters.h"
#include "executionstatus.h"
#include "instruction.h"
#include "memory.h"
#include <map>

class Cpu {
public:
  using Handler = void (Cpu::*)();

  static Handler operandsHandler(AddressingMode);
  static Handler instructionHandler(InstructionType);

  struct OpCodeEntry {
    const Instruction *operation;
    Handler prepareOperands;
    Handler executeInstruction;
  };

  using OpCodeLookUpTableType = std::array<OpCodeEntry, OpCodeTable.size()>;

private:
  static const OpCodeLookUpTableType OpCodeLookUpTable;

  volatile ExecutionStatus m_executionStatus = Idle;

  Memory &m_memory;
  CpuRegisters m_regs;
  CpuFlags m_flags;
  uint64_t m_cycles = 0;

  Memory::const_iterator m_instruction;
  Memory::iterator m_operands;
  uint8_t *m_effectiveOperand;

  uint8_t operand8() { return *m_operands; }

  uint16_t operand16() {
    return static_cast<uint16_t>(m_operands[0] | (m_operands[1] << 8));
  }

  void extraCycleOnPageBoundaryCrossing(uint16_t addr, uint8_t index) {
    if ((addr & 0xff) + index > 0xff)
      m_cycles++;
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
  Cpu(Memory &);
  auto cycles() const { return m_cycles; }
  void resetCycles() { m_cycles = 0; }
  void execute(bool continuous = false);
};
