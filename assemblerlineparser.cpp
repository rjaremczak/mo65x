#include "assemblerlineparser.h"
#include <QRegularExpression>

static constexpr QChar HexPrefix('$');
static constexpr QChar BinPrefix('%');
static constexpr auto LabelGroup = 1;
static constexpr auto OperationGroup = 2;
static constexpr auto FirstOperandGroup = 3;

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

struct LinePattern {
  const QRegularExpression regex;
  const LineFormat format;

  LinePattern(const QString& pattern, LineFormat format)
      : regex(Label + pattern + "\\s*" + Comment, QRegularExpression::CaseInsensitiveOption), format(format) {}
};

static const LinePattern ValidLinePatterns[]{{"", LineFormat::Empty},
                                             {OrgCmd, LineFormat::CmdOrg},
                                             {ByteCmd, LineFormat::CmdByte},
                                             {WordCmd, LineFormat::CmdByte},
                                             {Mnemonic, LineFormat::InsNoOperands},
                                             {Mnemonic + "#" + Operand, LineFormat::InsImmediate},
                                             {Mnemonic + Operand, LineFormat::InsAbsolute},
                                             {Mnemonic + Operand + ",x", LineFormat::InsAbsoluteIndexedX},
                                             {Mnemonic + Operand + ",y", LineFormat::InsAbsoluteIndexedY},
                                             {Mnemonic + "\\(" + Operand + "\\)", LineFormat::InsIndirect},
                                             {Mnemonic + "\\(" + Operand + ",x\\)", LineFormat::InsIndexedIndirectX},
                                             {Mnemonic + "\\(" + Operand + "\\),y", LineFormat::InsIndirectIndexedY},
                                             {Mnemonic + BranchTarget, LineFormat::InsBranch}};

bool isNumber(const QString& str) {
  const auto front = str.front();
  return front == HexPrefix || front == BinPrefix || front.isDigit() || front == '+' || front == '-';
}

int parseNumber(const QString& str) {
  const auto op = str;
  const auto front = op.front();
  if (front == HexPrefix) return op.right(op.length() - 1).toInt(nullptr, 16);
  if (front == BinPrefix) return op.right(op.length() - 1).toInt(nullptr, 2);
  return op.toInt(nullptr, 10);
}

static ControlCommand resolveControlCommand(const QString& str) {
}

static InstructionType resolveInstructionType(const QString& str) {
}

static OperandsFormat resolveAddressingMode(LinePattern pattern) {
}

static int resolveOperandValue(const QString& str, const SymbolTable& symbols) {
}

AssemblerLineModel AssemblerLineModel::create(const QString& line, const SymbolTable& symbols, Address pc) {
  for (auto& lp : ValidLinePatterns) {
    if (auto match = lp.regex.match(line); match.hasMatch()) {
      AssemblerLineModel alp;
      alp.label = match.captured(LabelGroup);

      const auto operation = match.captured(OperationGroup);
      alp.controlCommand = resolveControlCommand(operation);
      if (alp.controlCommand == ControlCommand::None) {
        alp.instructionType = resolveInstructionType(operation);
        switch (lp.format) {
        case LineFormat::InsNoOperands: break;
        case LineFormat::InsBranch: break;
        default: {
          // add all operand values, all symbols resolved
        }
        }
        // alp.addressingMode = resolveAddressingMode(match);
      }
      return alp;
    }
  }
  throw AssemblyResult::SyntaxError;
}
