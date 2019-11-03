#pragma once

#include "instructiontype.h"
#include <QString>
#include <algorithm>
#include <map>

using MnemonicTableType = std::map<InstructionType, const char*>;

extern const MnemonicTableType MnemonicTable;

InstructionType findInstructionType(const QString& mnemonic);
