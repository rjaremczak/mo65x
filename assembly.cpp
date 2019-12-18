#include "assembly.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include <QRegularExpression>

static constexpr auto LabelGroup = 1;
static constexpr auto OperationGroup = 2;
static constexpr auto FirstOperandGroup = 3;

static constexpr QChar HexPrefix('$');
static constexpr QChar BinPrefix('%');

static const QString Comment("(?:;.*)?$");
static const QString HexNum("\\$[\\d|a-h]{1,4}");
static const QString DecNum("\\d{1,5}");
static const QString BinNum("\\%[01]{1,16}");
static const QString Symbol("[a-z]\\w+");
static const QString Mnemonic("([a-z]{3})\\s*");
static const QString OrgCmd("((?:\\.ORG\\s+)|(?:\\*\\s*\\=\\s*))");
static const QString ByteCmd("(\\.(?:BYTE))\\s+");
static const QString WordCmd("(\\.(?:WORD))\\s+");

static const QString Operand("((?:" + HexNum + ")|(?:" + DecNum + ")|(?:" + BinNum + ")|(?:" + Symbol + "))\\s*");
static const QString Label("^(?:(" + Symbol + "):)?\\s*");
static const QString BranchTarget("((?:[+|-]?\\d{1,3})|(?:" + Symbol + "))\\s*");

enum class LineFormat {
  Invalid,
  Empty,
  CmdOrg,
  CmdByte,
  CmdWord,
  InsNoOperands,
  InsImmediate,
  InsAbsolute,
  InsAbsoluteIndexedX,
  InsAbsoluteIndexedY,
  InsIndirect,
  InsIndexedIndirectX,
  InsIndirectIndexedY,
  InsBranch
};

enum class OperandType { Invalid, Symbol, Literal };

struct LinePattern {
  const QRegularExpression regex;
  const LineFormat format;

  LinePattern(const QString& pattern, LineFormat format)
      : regex(Label + pattern + "\\s*" + Comment, QRegularExpression::CaseInsensitiveOption), format(format) {}
};

static const LinePattern ValidLinePatterns[]{{"", LineFormat::Empty},
                                             {OrgCmd + Operand, LineFormat::CmdOrg},
                                             {ByteCmd + "(?:[\\s\\,]" + Operand + ")+", LineFormat::CmdByte},
                                             {WordCmd, LineFormat::CmdWord},
                                             {Mnemonic, LineFormat::InsNoOperands},
                                             {Mnemonic + "#" + Operand, LineFormat::InsImmediate},
                                             {Mnemonic + Operand, LineFormat::InsAbsolute},
                                             {Mnemonic + Operand + ",x", LineFormat::InsAbsoluteIndexedX},
                                             {Mnemonic + Operand + ",y", LineFormat::InsAbsoluteIndexedY},
                                             {Mnemonic + "\\(" + Operand + "\\)", LineFormat::InsIndirect},
                                             {Mnemonic + "\\(" + Operand + ",x\\)", LineFormat::InsIndexedIndirectX},
                                             {Mnemonic + "\\(" + Operand + "\\),y", LineFormat::InsIndirectIndexedY},
                                             {Mnemonic + BranchTarget, LineFormat::InsBranch}};

static bool isByte(int n) {
  return n >= 0 && n <= 255;
}

static bool isWord(int n) {
  return n >= 0 && n <= 65535;
}

static bool isNumber(const QString& str) {
  const auto front = str.front();
  return front == HexPrefix || front == BinPrefix || front.isDigit() || front == '+' || front == '-';
}

static int parseNumber(const QString& str) {
  const auto op = str;
  const auto front = op.front();
  if (front == HexPrefix) return op.right(op.length() - 1).toInt(nullptr, 16);
  if (front == BinPrefix) return op.right(op.length() - 1).toInt(nullptr, 2);
  return op.toInt(nullptr, 10);
}

static int computeBranchDisplacement(const QString& str, const SymbolTable& symbols, Address lc) {
  if (isNumber(str)) return parseNumber(str);
  if (const auto& opt = symbols.get(str)) {
    const auto d = *opt - lc - 2;
    // if (d < -128 || d > 127) throw AssemblyResult::BranchTooFar;
    return d;
  }
  throw AssemblyResult::SymbolNotDefined;
}

static int resolveOperandValue(const QString& str, const SymbolTable& symbols) {
  if (isNumber(str)) return parseNumber(str);
  if (const auto& opt = symbols.get(str)) return *opt;
  throw AssemblyResult::SymbolNotDefined;
}

static InstructionType resolveInstructionType(const QString& str) {
  const auto it = std::find_if(MnemonicTable.begin(), MnemonicTable.end(), [=](const auto& kv) { return str == kv.second; });
  if (it == MnemonicTable.end()) throw AssemblyResult::InvalidMnemonic;
  return it->first;
}

static const Instruction* resolveInstruction(const QString& str, OperandsFormat mode0, OperandsFormat mode1) {
  const auto type = resolveInstructionType(str);
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(), [=](const Instruction& ins) {
    return ins.type == type && (ins.mode == mode0 || ins.mode == mode1);
  });
  if (it == InstructionTable.end()) throw AssemblyResult::InvalidInstructionFormat;
  return it;
}

static const Instruction* resolveInstruction(const QString& str, OperandsFormat mode) {
  return resolveInstruction(str, mode, mode);
}

uint8_t Assembly::opCode() const {
  return static_cast<uint8_t>(std::distance(InstructionTable.begin(), const_cast<const Instruction* const>(instruction)));
}

Assembly Assembly::process(const QString& line, const SymbolTable& symbols, Address lc) {
  Assembly assembly;
  for (auto& linePattern : ValidLinePatterns) {
    if (auto match = linePattern.regex.match(line); match.hasMatch()) {
      assembly.label = match.captured(LabelGroup);
      const auto instr = match.captured(OperationGroup);
      const auto opstr = match.captured(FirstOperandGroup);
      switch (linePattern.format) {
      case LineFormat::Invalid: throw AssemblyResult::SyntaxError;
      case LineFormat::Empty: break;
      case LineFormat::InsNoOperands: {
        assembly.command = Command::Instruction;
        assembly.instruction = resolveInstruction(instr, OperandsFormat::ImpliedOrAccumulator);
        break;
      }
      case LineFormat::InsBranch: {
        assembly.command = Command::Instruction;
        assembly.instruction = resolveInstruction(instr, OperandsFormat::Branch);
        assembly.operands.push_back(computeBranchDisplacement(opstr, symbols, lc));
        break;
      }
      case LineFormat::InsAbsolute: {
        assembly.command = Command::Instruction;
        assembly.instruction =
            resolveInstruction(instr, isByte(assembly.operands[0]) ? OperandsFormat::ZeroPage : OperandsFormat::Absolute);
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::InsIndirect: {
        assembly.command = Command::Instruction;
        assembly.instruction = resolveInstruction(instr, OperandsFormat::Indirect);
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::InsImmediate: {
        assembly.command = Command::Instruction;
        assembly.instruction = resolveInstruction(instr, OperandsFormat::Immediate);
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::InsAbsoluteIndexedX: {
        assembly.command = Command::Instruction;
        assembly.instruction =
            resolveInstruction(instr, isByte(assembly.operands[0]) ? OperandsFormat::ZeroPageX : OperandsFormat::AbsoluteX);
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::InsAbsoluteIndexedY: {
        assembly.command = Command::Instruction;
        assembly.instruction =
            resolveInstruction(instr, isByte(assembly.operands[0]) ? OperandsFormat::ZeroPageY : OperandsFormat::AbsoluteY);
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::InsIndexedIndirectX: {
        assembly.command = Command::Instruction;
        assembly.instruction = resolveInstruction(instr, OperandsFormat::IndexedIndirectX);
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::InsIndirectIndexedY: {
        assembly.command = Command::Instruction;
        assembly.instruction = resolveInstruction(instr, OperandsFormat::IndirectIndexedY);
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::CmdOrg: {
        assembly.command = Command::Origin;
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::CmdByte: {
        assembly.command = Command::Byte;
        for (int i = FirstOperandGroup; i <= match.lastCapturedIndex(); i++) {
          const auto v = resolveOperandValue(match.captured(i), symbols);
          if (!isByte(v)) throw AssemblyResult::ValueOutOfRange;
          assembly.operands.push_back(v);
        }
        break;
      }
      case LineFormat::CmdWord: {
        assembly.command = Command::Word;
        for (int i = FirstOperandGroup; i <= match.lastCapturedIndex(); i++) {
          const auto v = resolveOperandValue(match.captured(i), symbols);
          if (!isWord(v)) throw AssemblyResult::ValueOutOfRange;
          assembly.operands.push_back(v);
        }
        break;
      }
      }
      return assembly;
    }
  }
  throw AssemblyResult::SyntaxError;
}
