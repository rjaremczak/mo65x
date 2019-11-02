#pragma once

#include "instructiontype.h"
#include <algorithm>
#include <map>
#include <string>

using MnemonicTableType = std::map<InstructionType, const char*>;

extern const MnemonicTableType MnemonicTable;

InstructionType findInstructionType(const std::string& mnemonic);
