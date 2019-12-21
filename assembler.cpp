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
static const QString LoHiPrefix("[<|>]?");
static const QString Operand("(" + LoHiPrefix + NumOrSymbol + ")\\s*");
static const QString OperandSeparator("\\s*,?\\s*");
static const QString OperandList("((?:(?:" + LoHiPrefix + NumOrSymbol + ")" + OperandSeparator + ")+)\\s*");
static const QString BranchMnemonic("(BCC|BCS|BNE|BEQ|BMI|BPL|BVC|BVS)\\s*");
static const QString BranchTarget("((?:[+|-]?\\d{1,3})|(?:" + Symbol + "))\\s*");

Assembler::PatternEntry::PatternEntry(const QString& pattern, const Handler handler)
    : regex(Label + pattern + Comment, QRegularExpression::CaseInsensitiveOption), handler(handler) {
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

static InstructionType resolveInstructionType(const QString& str) {
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

static const Instruction* resolveInstruction(const QString& str, OperandsFormat mode, bool zp) {
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

Assembler::Assembler(Memory& memory) : memory(memory) {
}

void Assembler::init(uint16_t addr) {
  locationCounter = addr;
  written = 0;
}

void Assembler::clearSymbols() {
  symbolTable.clear();
}

void Assembler::changeMode(Assembler::ProcessingMode mode) {
  this->mode = mode;
}

AssemblyResult Assembler::processLine(const QString& str) {
  lastLocationCounter = locationCounter;
  for (const auto& entry : Patterns) {
    match = entry.regex.match(str);
    if (match.hasMatch()) {
      try {
        if (auto label = match.captured(LabelGroup); !label.isEmpty()) defineSymbol(label, locationCounter);
        (this->*entry.handler)();
        return AssemblyResult::Ok;
      } catch (AssemblyResult result) { return result; }
    }
  }
  return AssemblyResult::SyntaxError;
}

AddressRange Assembler::affectedAddressRange() const {
  return addressRange;
}

int Assembler::bytesWritten() const {
  return written;
}

QString Assembler::operation() const {
  return match.captured(OperationGroup);
}

QString Assembler::operand() const {
  return match.captured(FirstOperandGroup);
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

int Assembler::resolveAsInt(const QString& ostr) const {
  QChar prefix;
  const QString str = removeLoHiPrefix(prefix, ostr);
  if (isNumber(str)) return applyLoHiPrefix(prefix, parseNumber(str));
  if (const auto& opt = symbolTable.get(str)) return applyLoHiPrefix(prefix, *opt);
  if (mode == ProcessingMode::EmitCode) throw AssemblyResult::SymbolNotDefined;
  return 0;
}

int8_t Assembler::operandAsBranchDisplacement() const {
  const auto& op = operand();
  if (isNumber(op)) return safeCast<int8_t>(resolveAsInt(op));
  if (const auto& opt = symbolTable.get(operand())) return safeCast<int8_t>(*opt - locationCounter - 2);
  if (mode == ProcessingMode::EmitCode) throw AssemblyResult::SymbolNotDefined;
  return 0;
}

void Assembler::handleNoOperation() {
}

void Assembler::handleSetLocationCounter() {
  locationCounter = safeCast<uint16_t>(resolveAsInt(operand()));
}

void Assembler::handleEmitBytes() {
  for (const auto& op : split(operand())) emitByte(safeCast<uint8_t>(resolveAsInt(op)));
}

void Assembler::handleEmitWords() {
  for (const auto& op : split(operand())) emitWord(safeCast<uint16_t>(resolveAsInt(op)));
}

void Assembler::handleImpliedOrAccumulator() {
  assemble(OperandsFormat::ImpliedOrAccumulator);
}

void Assembler::handleImmediate() {
  assemble(OperandsFormat::Immediate, safeCast<uint8_t>(resolveAsInt(operand())));
}

void Assembler::handleAbsolute() {
  assemble(OperandsFormat::Absolute, safeCast<Address>(resolveAsInt(operand())));
}

void Assembler::handleAbsoluteIndexedX() {
  assemble(OperandsFormat::AbsoluteX, safeCast<Address>(resolveAsInt(operand())));
}

void Assembler::handleAbsoluteIndexedY() {
  assemble(OperandsFormat::AbsoluteY, safeCast<Address>(resolveAsInt(operand())));
}

void Assembler::handleIndirect() {
  assemble(OperandsFormat::Indirect, safeCast<Address>(resolveAsInt(operand())));
}

void Assembler::handleIndexedIndirectX() {
  assemble(OperandsFormat::IndexedIndirectX, safeCast<uint8_t>(resolveAsInt(operand())));
}

void Assembler::handleIndirectIndexedY() {
  assemble(OperandsFormat::IndirectIndexedY, safeCast<uint8_t>(resolveAsInt(operand())));
}

void Assembler::handleBranch() {
  assemble(OperandsFormat::Branch, operandAsBranchDisplacement());
}

void Assembler::assemble(OperandsFormat mode, int operand) {
  const auto instruction = resolveInstruction(operation(), mode, operand >= 0 && operand <= 255);
  emitByte(static_cast<uint8_t>(std::distance(InstructionTable.begin(), instruction)));
  if (instruction->size == 2)
    emitByte(static_cast<uint8_t>(operand));
  else if (instruction->size == 3)
    emitWord(static_cast<uint16_t>(operand));
}

void Assembler::defineSymbol(const QString& name, uint16_t value) {
  if (mode == ProcessingMode::ScanForSymbols) {
    if (!symbolTable.put(name, value)) throw AssemblyResult::SymbolAlreadyDefined;
  }
}

void Assembler::emitByte(uint8_t b) {
  if (mode == ProcessingMode::EmitCode) {
    updateAddressRange(locationCounter);
    memory[locationCounter] = b;
    written++;
  }
  locationCounter++;
}

void Assembler::emitWord(uint16_t word) {
  emitByte(static_cast<uint8_t>(word));
  emitByte(word >> 8);
}

void Assembler::updateAddressRange(uint16_t addr) {
  addressRange.expand(addr);
}
