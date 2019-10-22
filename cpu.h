#pragma once

#include <map>
#include "memory.h"
#include "opcode.h"
#include "executionstatus.h"
#include "cpuregisters.h"
#include "cpuflags.h"

class Cpu
{
public:
    using Handler = void (Cpu::*)();

    static Handler operandsHandler(AddressingMode);
    static Handler instructionHandler(Instruction);

    struct DecodeEntry {
        const Operation* operation;
        Handler prepareOperands;
        Handler executeInstruction;
    };

    using DecodeLookUpTable = std::array<DecodeEntry, OpCodeTable.size()>;

private:
    union Operand {
        uint8_t* bytePtr;
        uint16_t address;
    };

    static const DecodeLookUpTable decodeLookUpTable;

    CpuRegisters m_regs;
    CpuFlags m_flags;
    Memory& m_memory;
    uint64_t m_cycles = 0;
    volatile ExecutionStatus m_executionStatus = Idle;
    uint8_t m_opCode;
    const Operation* m_operation;
    Operand m_operand;

    uint8_t operand8() const { return m_memory[m_regs.pc + 1]; }
    uint16_t operand16() const { return m_memory.read16(m_regs.pc + 1); }

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
    auto cycles() const { return m_cycles; }
    void resetCycles() { m_cycles = 0; }
    void execute(bool continuous = false);
};
