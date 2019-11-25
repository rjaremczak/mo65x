#pragma once

#include "cpu.h"
#include "instructiontable.h"

struct DecodeEntry {
  const Instruction* instruction = nullptr;
  Cpu::Handler prepareOperands = nullptr;
  Cpu::Handler executeInstruction = nullptr;
};

constexpr Cpu::Handler operandsHandler(OperandsFormat mode) {
  switch (mode) {
  case Implied: return &Cpu::prepImpliedMode;
  case Accumulator: return &Cpu::prepAccumulatorMode;
  case Branch: return &Cpu::prepRelativeMode;
  case Immediate: return &Cpu::prepImmediateMode;
  case ZeroPage: return &Cpu::prepZeroPageMode;
  case ZeroPageX: return &Cpu::prepZeroPageXMode;
  case ZeroPageY: return &Cpu::prepZeroPageYMode;
  case IndexedIndirectX: return &Cpu::prepIndexedIndirectXMode;
  case IndirectIndexedY: return &Cpu::prepIndirectIndexedYMode;
  case Indirect: return &Cpu::prepIndirectMode;
  case Absolute: return &Cpu::prepAbsoluteMode;
  case AbsoluteX: return &Cpu::prepAbsoluteXMode;
  case AbsoluteY: return &Cpu::prepAbsoluteYMode;
  case NoOperands: return nullptr;
  }
}

constexpr Cpu::Handler instructionHandler(InstructionType instruction) {
  switch (instruction) {
  case LDA: return &Cpu::execLDA;
  case LDX: return &Cpu::execLDX;
  case LDY: return &Cpu::execLDY;
  case STA: return &Cpu::execSTA;
  case STX: return &Cpu::execSTX;
  case STY: return &Cpu::execSTY;

  case ADC: return &Cpu::execADC;
  case SBC: return &Cpu::execSBC;
  case INC: return &Cpu::execINC;
  case INX: return &Cpu::execINX;
  case INY: return &Cpu::execINY;
  case DEC: return &Cpu::execDEC;
  case DEX: return &Cpu::execDEX;
  case DEY: return &Cpu::execDEY;

  case ASL: return &Cpu::execASL;
  case LSR: return &Cpu::execLSR;
  case ROL: return &Cpu::execROL;
  case ROR: return &Cpu::execROR;

  case AND: return &Cpu::execAND;
  case ORA: return &Cpu::execORA;
  case EOR: return &Cpu::execEOR;

  case CMP: return &Cpu::execCMP;
  case CPX: return &Cpu::execCPX;
  case CPY: return &Cpu::execCPY;
  case BIT: return &Cpu::execBIT;

  case SED: return &Cpu::execSED;
  case SEI: return &Cpu::execSEI;
  case SEC: return &Cpu::execSEC;
  case CLC: return &Cpu::execCLC;
  case CLD: return &Cpu::execCLD;
  case CLI: return &Cpu::execCLI;
  case CLV: return &Cpu::execCLV;

  case TAX: return &Cpu::execTAX;
  case TXA: return &Cpu::execTXA;
  case TAY: return &Cpu::execTAY;
  case TYA: return &Cpu::execTYA;
  case TSX: return &Cpu::execTSX;
  case TXS: return &Cpu::execTXS;

  case PHA: return &Cpu::execPHA;
  case PLA: return &Cpu::execPLA;
  case PHP: return &Cpu::execPHP;
  case PLP: return &Cpu::execPLP;

  case RTS: return &Cpu::execRTS;
  case RTI: return &Cpu::execRTI;
  case BRK: return &Cpu::execBRK;
  case NOP: return &Cpu::execNOP;

  case BCC: return &Cpu::execBCC;
  case BCS: return &Cpu::execBCS;
  case BEQ: return &Cpu::execBEQ;
  case BMI: return &Cpu::execBMI;
  case BNE: return &Cpu::execBNE;
  case BPL: return &Cpu::execBPL;
  case BVC: return &Cpu::execBVC;
  case BVS: return &Cpu::execBVS;
  case JMP: return &Cpu::execJMP;
  case JSR: return &Cpu::execJSR;

  default: return &Cpu::execHalt;
  }
}

using DecodeTableType = std::array<DecodeEntry, Instruction::NumberOfOpCodes>;

constexpr DecodeTableType DecodeTable = [] {
  DecodeTableType dtab;
  for (size_t i = 0; i < InstructionTable.size(); i++) {
    const Instruction* ins = &InstructionTable[i];
    dtab[i] = {ins, operandsHandler(ins->mode), instructionHandler(ins->type)};
  }
  return dtab;
}();
