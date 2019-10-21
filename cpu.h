#pragma once

#include <map>
#include "cpustate.h"
#include "memory.h"
#include "opcode.h"

class Cpu
{
    union Operands {
        uint8_t* bytePtr;
        uint16_t address;
    };

    static const std::map<AddressingMode, void (Cpu::*)()> addressingModeHandlers;
    static const std::map<Instruction, void (Cpu::*)()> instructionHandlers;

    Memory& m_memory;
    CpuState m_state;
    OpCode m_instruction;
    Operands m_operands;
    uint64_t m_cycles = 0;

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

    void LDA();
    void LDX();
    void LDY();
    void STA();
    void STX();
    void STY();

    void ADC();
    void SBC();
    void INC();
    void INX();
    void INY();
    void DEC();
    void DEX();
    void DEY();

    void ASL();
    void LSR();
    void ROL();
    void ROR();

    void AND();
    void ORA();
    void EOR();

    void CMP();
    void CPX();
    void CPY();
    void BIT();

    void SED();
    void SEI();
    void CLC();
    void CLD();
    void CLI();
    void CLV();

    void TAX();
    void TXA();
    void TAY();
    void TYA();
    void TSX();
    void TXS();

    void PHA();
    void PLA();
    void PHP();
    void PLP();

    void RTS();
    void RTI();
    void BRK();
    void NOP();

    void BCC();
    void BCS();
    void BEQ();
    void BMI();
    void BNE();
    void BPL();
    void BVC();
    void BVS();
    void JMP();
    void JSR();

public:
    enum ExecutionStatus { Ok, InvalidOpCode };

    Cpu(Memory&);
    auto state() const { return m_state; }
    auto pc() const { return m_state.pc; }
    auto cycles() const { return m_cycles; }
    auto instruction() const { return m_instruction; }
    void resetCycles() { m_cycles = 0; }

    void setPC(uint16_t pc) { m_state.pc = pc; }
    ExecutionStatus execute(bool continuous = false);
};
