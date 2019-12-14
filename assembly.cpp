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
    if (d < -128 || d > 127) throw AssemblyResult::BranchTooFar;
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
  return it != MnemonicTable.end() ? it->first : InstructionType::KIL;
}

static OperandsFormat resolveNoOperandsMode(InstructionType instype) {
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(), [=](const Instruction& ins) {
    return ins.type == instype && (ins.mode == Implied || ins.mode == Accumulator);
  });
  return it->mode;
}

Assembly Assembly::create(const QString& line, const SymbolTable& symbols, Address lc) {
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
        assembly.instructionType = resolveInstructionType(instr);
        assembly.addressingMode = resolveNoOperandsMode(assembly.instructionType);
        break;
      }
      case LineFormat::InsBranch: {
        assembly.instructionType = resolveInstructionType(instr);
        assembly.addressingMode = Branch;
        assembly.operands.push_back(computeBranchDisplacement(opstr, symbols, lc));
        break;
      }
      case LineFormat::InsAbsolute: {
        assembly.instructionType = resolveInstructionType(instr);
        assembly.addressingMode = isByte(assembly.operand()) ? ZeroPage : Absolute;
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::InsIndirect: {
        assembly.instructionType = resolveInstructionType(instr);
        assembly.addressingMode = Indirect;
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::InsImmediate: {
        assembly.instructionType = resolveInstructionType(instr);
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        assembly.addressingMode = Immediate;
        break;
      }
      case LineFormat::InsAbsoluteIndexedX: {
        assembly.instructionType = resolveInstructionType(instr);
        assembly.addressingMode = isByte(assembly.operand()) ? ZeroPageX : AbsoluteX;
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::InsAbsoluteIndexedY: {
        assembly.instructionType = resolveInstructionType(instr);
        assembly.addressingMode = isByte(assembly.operand()) ? ZeroPageY : AbsoluteY;
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::InsIndexedIndirectX: {
        assembly.instructionType = resolveInstructionType(instr);
        assembly.addressingMode = IndexedIndirectX;
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::InsIndirectIndexedY: {
        assembly.instructionType = resolveInstructionType(instr);
        assembly.addressingMode = IndexedIndirectX;
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::CmdOrg: {
        assembly.controlCommand = ControlCommand::SetOrigin;
        assembly.operands.push_back(resolveOperandValue(opstr, symbols));
        break;
      }
      case LineFormat::CmdByte: {
        assembly.controlCommand = ControlCommand::EmitBytes;
        for (int i = FirstOperandGroup; i <= match.lastCapturedIndex(); i++) {
          const auto v = resolveOperandValue(match.captured(i), symbols);
          if (!isByte(v)) throw AssemblyResult::ValueOutOfRange;
          assembly.operands.push_back(v);
        }
        break;
      }
      case LineFormat::CmdWord: {
        assembly.controlCommand = ControlCommand::EmitWords;
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
