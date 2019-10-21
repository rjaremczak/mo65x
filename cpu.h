#pragma once

#include <map>
#include "cpustate.h"
#include "memory.h"
#include "opcode.h"

class Cpu
{
public:
    enum ExecutionStatus { Idle, InvalidOpCode, Running, StopRequested };

    using Handler = void (Cpu::*)();

    static Handler operandsProvider(AddressingMode);
    static Handler instructionExecutor(Instruction);

    struct DecodeEntry {
        const Operation* operation;
        Handler operandsProvider;
        Handler instructionExecutor;
    };

    using DecodeLookUpTable = std::array<DecodeEntry, OpCodeTable.size()>;

private:
    union Operands {
        uint8_t* bytePtr;
        uint16_t address;
    };

    static const DecodeLookUpTable decodeLookUpTable;

    Memory& m_memory;
    CpuState m_state;
    uint64_t m_cycles = 0;
    volatile ExecutionStatus m_executionStatus = Idle;

    uint8_t m_opCode;
    const Operation* m_operation;
    Operands m_operands;

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

public:
    Cpu(Memory&);
    auto state() const { return m_state; }
    auto pc() const { return m_state.pc; }
    auto cycles() const { return m_cycles; }
    auto instruction() const { return m_operation; }
    void resetCycles() { m_cycles = 0; }
    void setPC(uint16_t pc) { m_state.pc = pc; }
    void execute(bool continuous = false);
};
