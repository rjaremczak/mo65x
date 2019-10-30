#include "assembler.h"

Assembler::Assembler(Memory& memory, uint16_t origin) : memory_(memory), address_(origin) {
}

void Assembler::ins(InstructionType type, AddressingMode addressingMode, uint16_t operand) {
}
