#include "assembler.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include <QRegularExpression>
#include <algorithm>

static constexpr auto LabelGroup = 1;
static constexpr auto OperationGroup = 2;
static constexpr auto FirstOperandGroup = 3;

static constexpr QChar LoBytePrefix('<');
static constexpr QChar HiBytePrefix('>');
static constexpr QChar HexPrefix('$');
static constexpr QChar BinPrefix('%');

static const QString Symbol("[a-z]\\w*");
static const QString Label("^(?:(" + Symbol + "):)?\\s*");
static const QString Comment("(?:;.*)?$");
static const QString OrgCmd("((?:\\.ORG\\s+)|(?:\\*\\s*\\=\\s*))");
static const QString ByteCmd("(\\.BYTE|DCB)\\s+");
static const QString WordCmd("(\\.WORD)\\s+");
static const QString HexNum("\\$[\\d|a-h]{1,4}");
static const QString DecNum("\\d{1,5}");
static const QString BinNum("\\%[01]{1,16}");
static const QString Mnemonic("([a-z]{3})\\s*");
static const QString NumOrSymbol("(?:" + HexNum + ")|(?:" + DecNum + ")|(?:" + BinNum + ")|(?:" + Symbol + ")");
static const QString LoHiPrefix("[" + QString(LoBytePrefix) + "|" + QString(HiBytePrefix) + "]?");
static const QString Operand("(" + LoHiPrefix + "(?:" + NumOrSymbol + "))\\s*");
static const QString OperandSeparator("\\s*,?\\s*");
static const QString OperandList("((?:(?:" + LoHiPrefix + "(?:" + NumOrSymbol + "))" + OperandSeparator + ")+)\\s*");
static const QString BranchMnemonic("(BCC|BCS|BNE|BEQ|BMI|BPL|BVC|BVS)\\s*");
static const QString BranchTarget("((?:[+|-]?\\d{1,3})|(?:" + Symbol + "))\\s*");

Assembler::PatternEntry::PatternEntry(const QString& pattern, const Handler handler)
    : regex(QString(Label + pattern + Comment).toStdString(), std::regex_constants::icase),
      // regex(Label + pattern + Comment, QRegularExpression::CaseInsensitiveOption),
      handler(handler) {
}

const Assembler::PatternEntry Assembler::Patterns[]{{"", &Assembler::handleNoOperation},
                                                    {OrgCmd + Operand, &Assembler::handleSetLocationCounter},
                                                    {ByteCmd + OperandList, &Assembler::handleEmitBytes},
                                                    {WordCmd + OperandList, &Assembler::handleEmitWords},
                                                    {Mnemonic, &Assembler::handleImpliedOrAccumulator},
                                                    {Mnemonic + "#" + Operand, &Assembler::handleImmediate},
                                                    {BranchMnemonic + BranchTarget, &Assembler::handleBranch},
                                                    {Mnemonic + Operand, &Assembler::handleAbsolute},
                                                    {Mnemonic + Operand + ",x", &Assembler::handleAbsoluteIndexedX},
                                                    {Mnemonic + Operand + ",y", &Assembler::handleAbsoluteIndexedY},
                                                    {Mnemonic + "\\(" + Operand + "\\)", &Assembler::handleIndirect},
                                                    {Mnemonic + "\\(" + Operand + ",x\\)", &Assembler::handleIndexedIndirectX},
                                                    {Mnemonic + "\\(" + Operand + "\\),y", &Assembler::handleIndirectIndexedY}};

static const QRegularExpression OperandItemRegEx(Operand + OperandSeparator, QRegularExpression::CaseInsensitiveOption);

static InstructionType resolveInstructionType(QString str) {
  const auto it = std::find_if(MnemonicTable.begin(), MnemonicTable.end(), [=](const auto& kv) { return str == kv.second; });
  if (it == MnemonicTable.end()) throw AssemblyResult::InvalidMnemonic;
  return it->first;
}

static OperandsFormat adjustAddressingMode(InstructionType type, OperandsFormat mode, bool zp) {
  if (zp && type != InstructionType::JMP && type != InstructionType::JSR) {
    switch (mode) {
    case OperandsFormat::Absolute: return OperandsFormat::ZeroPage;
    case OperandsFormat::AbsoluteX: return OperandsFormat::ZeroPageX;
    case OperandsFormat::AbsoluteY: return OperandsFormat::ZeroPageY;
    default: break;
    }
  }
  return mode;
}

static const Instruction* resolveInstruction(QString str, OperandsFormat mode, bool zp) {
  const auto type = resolveInstructionType(str.toUpper());
  const auto adjustedMode = adjustAddressingMode(type, mode, zp);
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(),
                               [=](const Instruction& ins) { return ins.type == type && ins.mode == adjustedMode; });
  if (it == InstructionTable.end()) { throw AssemblyResult::InvalidInstructionFormat; }
  return it;
}

template <typename T>
T safeCast(int n) {
  if (n >= std::numeric_limits<T>::min() && n <= std::numeric_limits<T>::max()) return static_cast<T>(n);
  qDebug("value %d out of range", n);
  throw AssemblyResult::ValueOutOfRange;
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

static std::vector<QString> split(const QString& str) {
  std::vector<QString> items;
  int offset = 0;
  QRegularExpressionMatch m;
  while ((m = OperandItemRegEx.match(str, offset)).hasMatch()) {
    offset = m.capturedEnd();
    items.push_back(m.captured(1));
  }
  return items;
}

Assembler::Assembler(Memory& memory) : m_memory(memory) {
  init();
}

void Assembler::initPreserveSymbols(Address addr) {
  m_addressRange = AddressRange::Invalid;
  m_mode = ProcessingMode::EmitCode;
  m_locationCounter = addr;
  m_lastLocationCounter = addr;
  m_written = 0;
}

void Assembler::init(Address addr) {
  initPreserveSymbols(addr);
  m_symbols.clear();
}

void Assembler::changeMode(Assembler::ProcessingMode mode) {
  this->m_mode = mode;
}

AssemblyResult Assembler::processLine(const QString& str) {
  m_lastLocationCounter = m_locationCounter;
  std::smatch match;
  for (const auto& entry : Patterns) {
    const auto s = str.toStdString();
    if (std::regex_match(s, match, entry.regex)) {
      m_operation = QString::fromStdString(match.str(OperationGroup));
      m_operand = QString::fromStdString(match.str(FirstOperandGroup));
      try {
        if (auto label = match.str(LabelGroup); !label.empty()) defineSymbol(QString::fromStdString(label), m_locationCounter);
        (this->*entry.handler)();
        return AssemblyResult::Ok;
      } catch (AssemblyResult result) { return result; }
    }
  }
  return AssemblyResult::SyntaxError;
}

AddressRange Assembler::affectedAddressRange() const {
  return m_addressRange;
}

int Assembler::bytesWritten() const {
  return m_written;
}

static QString removeLoHiPrefix(QChar& prefix, const QString& str) {
  const auto f = str.front();
  if (f == LoBytePrefix || f == HiBytePrefix) {
    prefix = f;
    return str.right(str.length() - 1);
  }
  prefix = QChar::Null;
  return str;
}

static int applyLoHiPrefix(QChar prefix, int num) {
  if (prefix.isNull()) return num;
  if (prefix == HiBytePrefix) return num >> 8;
  return num & 0xff;
}

OperandValue Assembler::operandValue(const QString& ostr) const {
  QChar prefix;
  const QString str = removeLoHiPrefix(prefix, ostr);
  if (isNumber(str)) return {OperandValue::Literal, applyLoHiPrefix(prefix, parseNumber(str))};
  if (const auto& opt = m_symbols.get(str)) return {OperandValue::Identifier, applyLoHiPrefix(prefix, *opt)};
  if (m_mode == Assembler::ProcessingMode::EmitCode) throw AssemblyResult::SymbolNotDefined;
  return {OperandValue::UndefinedIdentifier};
}

OperandValue Assembler::operandValue() const {
  return operandValue(m_operand);
}

int8_t Assembler::operandAsBranchDisplacement() const {
  const auto op = operandValue();
  if (m_mode == Assembler::ProcessingMode::ScanForSymbols) return 0;
  return safeCast<int8_t>(op.isLiteral() ? op : op - m_locationCounter - 2);
}
void Assembler::handleNoOperation() {
}

void Assembler::handleSetLocationCounter() {
  m_locationCounter = safeCast<uint16_t>(operandValue());
}

void Assembler::handleEmitBytes() {
  for (const auto& op : split(m_operand))
    emitByte(safeCast<uint8_t>(operandValue(op)));
}

void Assembler::handleEmitWords() {
  for (const auto& op : split(m_operand))
    emitWord(safeCast<uint16_t>(operandValue(op)));
}

void Assembler::handleImpliedOrAccumulator() {
  assemble(OperandsFormat::ImpliedOrAccumulator);
}

void Assembler::handleImmediate() {
  assemble(OperandsFormat::Immediate, operandValue());
}

void Assembler::handleAbsolute() {
  assemble(OperandsFormat::Absolute, operandValue());
}

void Assembler::handleAbsoluteIndexedX() {
  assemble(OperandsFormat::AbsoluteX, operandValue());
}

void Assembler::handleAbsoluteIndexedY() {
  assemble(OperandsFormat::AbsoluteY, operandValue());
}

void Assembler::handleIndirect() {
  assemble(OperandsFormat::Indirect, operandValue());
}

void Assembler::handleIndexedIndirectX() {
  assemble(OperandsFormat::IndexedIndirectX, operandValue());
}

void Assembler::handleIndirectIndexedY() {
  assemble(OperandsFormat::IndirectIndexedY, operandValue());
}

void Assembler::handleBranch() {
  assemble(OperandsFormat::Branch, {OperandValue::Literal, operandAsBranchDisplacement()});
}

void Assembler::assemble(OperandsFormat mode, OperandValue operand) {
  const auto instruction = resolveInstruction(m_operation, mode, operand.isLiteral() && operand >= 0 && operand <= 255);
  emitByte(static_cast<uint8_t>(std::distance(InstructionTable.begin(), instruction)));
  if (instruction->size == 2)
    emitByte(static_cast<uint8_t>(operand));
  else if (instruction->size == 3)
    emitWord(static_cast<uint16_t>(operand));
}

void Assembler::defineSymbol(QString name, uint16_t value) {
  if (m_mode == ProcessingMode::ScanForSymbols) {
    if (!m_symbols.put(name, value)) throw AssemblyResult::SymbolAlreadyDefined;
  }
}

void Assembler::emitByte(uint8_t b) {
  if (m_mode == ProcessingMode::EmitCode) {
    updateAddressRange(m_locationCounter);
    m_memory[m_locationCounter] = b;
    m_written++;
  }
  m_locationCounter++;
}

void Assembler::emitWord(uint16_t word) {
  emitByte(static_cast<uint8_t>(word));
  emitByte(word >> 8);
}

void Assembler::updateAddressRange(uint16_t addr) {
  m_addressRange.expand(addr);
}
