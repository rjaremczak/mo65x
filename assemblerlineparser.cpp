#include "assemblerlineparser.h"
#include <QRegularExpression>

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

struct LinePattern {
  const QRegularExpression regex;
  const AssemblerLineParser::Format format;

  LinePattern(const QString& pattern, AssemblerLineParser::Format format)
      : regex(Label + pattern + "\\s*" + Comment, QRegularExpression::CaseInsensitiveOption), format(format) {}
};

static const LinePattern ValidLinePatterns[]{{"", AssemblerLineParser::Empty},
                                             {OrgCmd, AssemblerLineParser::CmdOrg},
                                             {ByteCmd, AssemblerLineParser::CmdByte},
                                             {WordCmd, AssemblerLineParser::CmdByte},
                                             {Mnemonic, AssemblerLineParser::InsNoOperands},
                                             {Mnemonic + "#" + Operand, AssemblerLineParser::InsImmediate},
                                             {Mnemonic + Operand, AssemblerLineParser::InsAbsolute},
                                             {Mnemonic + Operand + ",x", AssemblerLineParser::InsAbsoluteIndexedX},
                                             {Mnemonic + Operand + ",y", AssemblerLineParser::InsAbsoluteIndexedY},
                                             {Mnemonic + "\\(" + Operand + "\\)", AssemblerLineParser::InsIndirect},
                                             {Mnemonic + "\\(" + Operand + ",x\\)", AssemblerLineParser::InsIndexedIndirectX},
                                             {Mnemonic + "\\(" + Operand + "\\),y", AssemblerLineParser::InsIndirectIndexedY},
                                             {Mnemonic + BranchTarget, AssemblerLineParser::InsBranch}};

AssemblerLineParser AssemblerLineParser::parse(const QString& line) {
  for (auto& lp : ValidLinePatterns) {
    if (auto match = lp.regex.match(line); match.hasMatch()) { return AssemblerLineParser(match, lp.format); }
  }
  return {};
}

AssemblerLineParser::AssemblerLineParser(QRegularExpressionMatch match, AssemblerLineParser::Format format)
    : match(match), format(format) {
}
