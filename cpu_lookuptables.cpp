#include "cpu.h"
#include <map>

const std::map<AddressingMode, void (Cpu::*)()> Cpu::addressingModeHandlers = {
   { AddressingMode::Implied, &Cpu::amImplied },
   { AddressingMode::Accumulator, &Cpu::amAccumulator },
   { AddressingMode::Relative, &Cpu::amRelative },
   { AddressingMode::Immediate, &Cpu::amImmediate },
   { AddressingMode::ZeroPage, &Cpu::amZeroPage },
   { AddressingMode::ZeroPageX, &Cpu::amZeroPageX },
   { AddressingMode::ZeroPageY, &Cpu::amZeroPageY },
   { AddressingMode::IndexedIndirectX, &Cpu::amIndexedIndirectX },
   { AddressingMode::IndirectIndexedY, &Cpu::amIndirectIndexedY },
   { AddressingMode::Indirect, &Cpu::amIndirect },
   { AddressingMode::Absolute, &Cpu::amAbsolute },
   { AddressingMode::AbsoluteX, &Cpu::amAbsoluteX },
   { AddressingMode::AbsoluteY, &Cpu::amAbsoluteY }
};

const std::map<Instruction, void (Cpu::*)()> Cpu::instructionHandlers = {
    { Instruction::LDA, &Cpu::LDA },
    { Instruction::LDX, &Cpu::LDX },
    { Instruction::LDY, &Cpu::LDY },
    { Instruction::STA, &Cpu::STA },
    { Instruction::STX, &Cpu::STX },
    { Instruction::STY, &Cpu::STY },

    { Instruction::ADC, &Cpu::ADC },
    { Instruction::SBC, &Cpu::SBC },
    { Instruction::INC, &Cpu::INC },
    { Instruction::INX, &Cpu::INX },
    { Instruction::INY, &Cpu::INY },
    { Instruction::DEC, &Cpu::DEC },
    { Instruction::DEX, &Cpu::DEX },
    { Instruction::DEY, &Cpu::DEY },

    { Instruction::ASL, &Cpu::ASL },
    { Instruction::LSR, &Cpu::LSR },
    { Instruction::ROL, &Cpu::ROL },
    { Instruction::ROR, &Cpu::ROR },

    { Instruction::AND, &Cpu::AND },
    { Instruction::ORA, &Cpu::ORA },
    { Instruction::EOR, &Cpu::EOR },

    { Instruction::CMP, &Cpu::CMP },
    { Instruction::CPX, &Cpu::CPX },
    { Instruction::CPY, &Cpu::CPY },
    { Instruction::BIT, &Cpu::BIT },

    { Instruction::SED, &Cpu::SED },
    { Instruction::SEI, &Cpu::SEI },
    { Instruction::CLC, &Cpu::CLC },
    { Instruction::CLD, &Cpu::CLD },
    { Instruction::CLI, &Cpu::CLI },
    { Instruction::CLV, &Cpu::CLV },

    { Instruction::TAX, &Cpu::TAX },
    { Instruction::TXA, &Cpu::TXA },
    { Instruction::TAY, &Cpu::TAY },
    { Instruction::TYA, &Cpu::TYA },
    { Instruction::TSX, &Cpu::TSX },
    { Instruction::TXS, &Cpu::TXS },

    { Instruction::PHA, &Cpu::PHA },
    { Instruction::PLA, &Cpu::PLA },
    { Instruction::PHP, &Cpu::PHP },
    { Instruction::PLP, &Cpu::PLP },

    { Instruction::RTS, &Cpu::RTS },
    { Instruction::RTI, &Cpu::RTI },
    { Instruction::BRK, &Cpu::BRK },
    { Instruction::NOP, &Cpu::NOP },

    { Instruction::BCC, &Cpu::BCC },
    { Instruction::BCS, &Cpu::BCS },
    { Instruction::BEQ, &Cpu::BEQ },
    { Instruction::BMI, &Cpu::BMI },
    { Instruction::BNE, &Cpu::BNE },
    { Instruction::BPL, &Cpu::BPL },
    { Instruction::BVC, &Cpu::BVC },
    { Instruction::BVS, &Cpu::BVS },
    { Instruction::JMP, &Cpu::JMP },
    { Instruction::JSR, &Cpu::JSR },
};

