#include <map>
#include <QStringList>
#include "disassembler.h"
#include "instructions.h"

static const std::map<Mnemonic, const char*> Mnemonics {
    { ADC, "ADC"}, { SBC, "SBC"}, { AND, "AND"}, { ORA, "ORA"}, { ASL, "ASL"}, { LSR, "LSR"}, { EOR, "EOR"}, { ROL, "ROL"}, { ROR, "ROR"},
    { BIT, "BIT"}, { CMP, "CMP"}, { CPX, "CPX"}, { CPY, "CPY"},
    { INC, "INC"}, { INX, "INX"}, { INY, "INY"},
    { DEC, "DEC"}, { DEX, "DEX"}, { DEY, "DEY"},
    { BCC, "BCC"}, { BCS, "BCS"}, { BEQ, "BEQ"}, { BMI, "BMI"}, { BNE, "BNE"}, { BPL, "BPL"}, { BVC, "BVC"}, { BVS, "BVS"},
    { CLC, "CLC"}, { CLD, "CLD"}, { CLI, "CLI"}, { CLV, "CLV"},
    { SEC, "SEC"}, { SED, "SED"}, { SEI, "SEI"},
    { JMP, "JMP"}, { JSR, "JSR"}, { BRK, "BRK"}, { RTI, "RTI"}, { RTS, "RTS"},
    { LDA, "LDA"}, { LDX, "LDX"}, { LDY, "LDY"},
    { STA, "STA"}, { STX, "STX"}, { STY, "STY"},
    { TAX, "TAX"}, { TAY, "TAY"}, { TSX, "TSX"}, { TXA, "TXA"}, { TYA, "TYA"}, { TXS, "TXS"},
    { PHA, "PHA"}, { PHP, "PHP"}, { PLA, "PLA"}, { PLP, "PLP"},
    { NOP, "NOP"}, { Invalid, "???"}
};

static QString formatHex8(uint16_t val) {
    return QString("%1").arg(val, 2, 16, QChar('0'));
}

static QString formatHex16(uint16_t val) {
    return QString("%1").arg(val, 4, 16, QChar('0'));
}

Disassembler::Disassembler(const Memory &memory, uint16_t pc) : m_memory(memory)
{
    setAddr(pc);
}

QString Disassembler::disassemble() const
{
    const auto opcode = m_memory[m_addr];
    auto str = formatHex16(m_addr).append(" ").append(formatHex8(opcode)).append(" ");
    const auto& instruction = Instructions[opcode];


    return QString("%1").arg(m_addr, 4, 16, QChar('0'));
}

