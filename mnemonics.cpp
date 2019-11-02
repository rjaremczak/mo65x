#include "mnemonics.h"

const MnemonicTableType MnemonicTable{
    {ADC, "ADC"}, {SBC, "SBC"}, {AND, "AND"}, {ORA, "ORA"}, {ASL, "ASL"}, {LSR, "LSR"}, {EOR, "EOR"}, {ROL, "ROL"}, {ROR, "ROR"},
    {BIT, "BIT"}, {CMP, "CMP"}, {CPX, "CPX"}, {CPY, "CPY"}, {INC, "INC"}, {INX, "INX"}, {INY, "INY"}, {DEC, "DEC"}, {DEX, "DEX"},
    {DEY, "DEY"}, {BCC, "BCC"}, {BCS, "BCS"}, {BEQ, "BEQ"}, {BMI, "BMI"}, {BNE, "BNE"}, {BPL, "BPL"}, {BVC, "BVC"}, {BVS, "BVS"},
    {CLC, "CLC"}, {CLD, "CLD"}, {CLI, "CLI"}, {CLV, "CLV"}, {SEC, "SEC"}, {SED, "SED"}, {SEI, "SEI"}, {JMP, "JMP"}, {JSR, "JSR"},
    {BRK, "BRK"}, {RTI, "RTI"}, {RTS, "RTS"}, {LDA, "LDA"}, {LDX, "LDX"}, {LDY, "LDY"}, {STA, "STA"}, {STX, "STX"}, {STY, "STY"},
    {TAX, "TAX"}, {TAY, "TAY"}, {TSX, "TSX"}, {TXA, "TXA"}, {TYA, "TYA"}, {TXS, "TXS"}, {PHA, "PHA"}, {PHP, "PHP"}, {PLA, "PLA"},
    {PLP, "PLP"}, {NOP, "NOP"}, {INV, "???"}};

InstructionType findInstructionType(const std::string& mnemonic) {
  const auto it =
      std::find_if(MnemonicTable.begin(), MnemonicTable.end(), [=](const auto& kv) { return mnemonic == kv.second; });
  return it != MnemonicTable.end() ? it->first : InstructionType::INV;
}
