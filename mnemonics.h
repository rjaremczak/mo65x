#pragma once

#include "instructiontype.h"
#include <algorithm>
#include <map>

using MnemonicTableType = std::map<InstructionType, const char*>;

extern const MnemonicTableType MnemonicTable;
