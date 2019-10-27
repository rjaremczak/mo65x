#include "cpu_lookuptables.h"

const OpCodeLookUpTableType OpCodeLookUpTable = [] {
  OpCodeLookUpTableType lut;
  for (size_t i = 0; i < OpCodeTable.size(); i++) {
    const Instruction* opCode = &OpCodeTable[i];
    lut[i] = {opCode, Cpu::operandsHandler(opCode->addressing), Cpu::instructionHandler(opCode->instruction)};
  }
  return lut;
}();

Cpu::Handler Cpu::operandsHandler(AddressingMode addressingMode)
{
    switch(addressingMode) {
        case Implied : return &Cpu::amImplied;
        case Accumulator : return &Cpu::amAccumulator;
        case Relative : return &Cpu::amRelative;
        case Immediate : return &Cpu::amImmediate;
        case ZeroPage : return &Cpu::amZeroPage;
        case ZeroPageX : return &Cpu::amZeroPageX;
        case ZeroPageY : return &Cpu::amZeroPageY;
        case IndexedIndirectX : return &Cpu::amIndexedIndirectX;
        case IndirectIndexedY : return &Cpu::amIndirectIndexedY;
        case Indirect : return &Cpu::amIndirect;
        case Absolute : return &Cpu::amAbsolute;
        case AbsoluteX : return &Cpu::amAbsoluteX;
        case AbsoluteY : return &Cpu::amAbsoluteY;
    }
}

Cpu::Handler Cpu::instructionHandler(InstructionType instruction)
{
    switch (instruction) {
    case LDA : return &Cpu::insLDA;
    case LDX : return &Cpu::insLDX;
    case LDY : return &Cpu::insLDY;
    case STA : return &Cpu::insSTA;
    case STX : return &Cpu::insSTX;
    case STY : return &Cpu::insSTY;

    case ADC : return &Cpu::insADC;
    case SBC : return &Cpu::insSBC;
    case INC : return &Cpu::insINC;
    case INX : return &Cpu::insINX;
    case INY : return &Cpu::insINY;
    case DEC : return &Cpu::insDEC;
    case DEX : return &Cpu::insDEX;
    case DEY : return &Cpu::insDEY;

    case ASL : return &Cpu::insASL;
    case LSR : return &Cpu::insLSR;
    case ROL : return &Cpu::insROL;
    case ROR : return &Cpu::insROR;

    case AND : return &Cpu::insAND;
    case ORA : return &Cpu::insORA;
    case EOR : return &Cpu::insEOR;

    case CMP : return &Cpu::insCMP;
    case CPX : return &Cpu::insCPX;
    case CPY : return &Cpu::insCPY;
    case BIT : return &Cpu::insBIT;

    case SED : return &Cpu::insSED;
    case SEI : return &Cpu::insSEI;
    case SEC : return &Cpu::insSEC;
    case CLC : return &Cpu::insCLC;
    case CLD : return &Cpu::insCLD;
    case CLI : return &Cpu::insCLI;
    case CLV : return &Cpu::insCLV;

    case TAX : return &Cpu::insTAX;
    case TXA : return &Cpu::insTXA;
    case TAY : return &Cpu::insTAY;
    case TYA : return &Cpu::insTYA;
    case TSX : return &Cpu::insTSX;
    case TXS : return &Cpu::insTXS;

    case PHA : return &Cpu::insPHA;
    case PLA : return &Cpu::insPLA;
    case PHP : return &Cpu::insPHP;
    case PLP : return &Cpu::insPLP;

    case RTS : return &Cpu::insRTS;
    case RTI : return &Cpu::insRTI;
    case BRK : return &Cpu::insBRK;
    case NOP : return &Cpu::insNOP;

    case BCC : return &Cpu::insBCC;
    case BCS : return &Cpu::insBCS;
    case BEQ : return &Cpu::insBEQ;
    case BMI : return &Cpu::insBMI;
    case BNE : return &Cpu::insBNE;
    case BPL : return &Cpu::insBPL;
    case BVC : return &Cpu::insBVC;
    case BVS : return &Cpu::insBVS;
    case JMP : return &Cpu::insJMP;
    case JSR : return &Cpu::insJSR;

    case INV : return nullptr;
    }
}
