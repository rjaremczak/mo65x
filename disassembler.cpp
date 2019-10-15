#include <map>
#include <QStringList>
#include "disassembler.h"
#include "opcodes.h"

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

static QString formatHex8(uint8_t val) {
    return QString("%1").arg(val, 2, 16, QChar('0'));
}

static QString formatHex16(uint16_t val) {
    return QString("%1").arg(val, 4, 16, QChar('0'));
}

Disassembler::Disassembler(const Memory &memory, uint16_t pc) : m_memory(memory)
{
    setAddr(pc);
}

void Disassembler::setAddr(uint16_t addr)
{
    m_addr = addr;
    m_opcode = m_memory[addr];
    m_instruction = OpCodes[m_opcode];
}

void Disassembler::step()
{
    setAddr(m_addr + m_instruction.size);
}

QString Disassembler::dumpBytes(uint16_t n) const
{
    QString str;
    for(uint16_t i=0; i<n; i++) { str.append(formatHex8(m_memory[m_addr + i])).append(" "); }
    return str;
}

QString Disassembler::dumpWords(uint16_t n) const
{
    QString str;
    for(uint16_t i=0; i<n; i+=2) { str.append(formatHex16(m_memory.read16(m_addr + i))).append(" "); }
    return str;
}

QString Disassembler::disassemble() const
{
    QString str = formatHex16(m_addr).append(" ");
    for(uint8_t i=0; i<3; i++) {
        str.append(i < m_instruction.size ? formatHex8(m_memory[m_addr+i]).append(" ") : "   ");
    }
    str.append(Mnemonics.at(m_instruction.mnemonic)).append(" ");
    return str;
}

