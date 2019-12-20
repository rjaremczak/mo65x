#include "assembler.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include <QRegularExpression>
#include <algorithm>

static constexpr auto LabelGroup = 1;
static constexpr auto OperationGroup = 2;
static constexpr auto FirstOperandGroup = 3;
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
static const QString Operand("(" + NumOrSymbol + ")\\s*");
static const QString OperandSeparator("\\s*,?\\s*");
static const QString OperandList("((?:(?:" + NumOrSymbol + ")" + OperandSeparator + ")+)\\s*");
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

static const Instruction* resolveInstruction(const QString& str, OperandsFormat mode) {
  const auto type = resolveInstructionType(str.toUpper());
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(),
                               [=](const Instruction& ins) { return ins.type == type && ins.mode == mode; });
  if (it == InstructionTable.end()) throw AssemblyResult::InvalidInstructionFormat;
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

size_t Assembler::bytesWritten() const {
  return written;
}

QString Assembler::operation() const {
  return match.captured(OperationGroup);
}

QString Assembler::operand() const {
  return match.captured(FirstOperandGroup);
}

int Assembler::resolveAsInt(const QString& str) const {
  if (isNumber(str)) return parseNumber(str);
  if (const auto& opt = symbolTable.get(str)) return *opt;
  throw AssemblyResult::SymbolNotDefined;
}

int8_t Assembler::operandAsBranchDisplacement() const {
  const auto& op = operand();
  if (isNumber(op)) return safeCast<int8_t>(resolveAsInt(op));
  if (const auto& opt = symbolTable.get(operand())) return safeCast<int8_t>(*opt - locationCounter - 2);
  throw AssemblyResult::SymbolNotDefined;
}

void Assembler::handleNoOperation() {
}

void Assembler::handleSetLocationCounter() {
  locationCounter = safeCast<uint16_t>(resolveAsInt(operand()));
}

static std::vector<QString> splitOperandList(const QString& str) {
  std::vector<QString> items;
  int offset = 0;
  QRegularExpressionMatch m;
  while ((m = OperandItemRegEx.match(str, offset)).hasMatch()) {
    offset = m.capturedEnd();
    items.push_back(m.captured(1));
  }
  return items;
}

void Assembler::handleEmitBytes() {
  int offset = 0;
  auto str = operand();
  QRegularExpressionMatch opMatch;
  while ((opMatch = OperandItemRegEx.match(str, offset)).hasMatch()) {
    offset = opMatch.capturedEnd();
    emitByte(safeCast<uint8_t>(resolveAsInt(opMatch.captured(1))));
  }
}

void Assembler::handleEmitWords() {
  int offset = 0;
  auto str = operand();
  QRegularExpressionMatch opMatch;
  while ((opMatch = OperandItemRegEx.match(str, offset)).hasMatch()) {
    offset = opMatch.capturedEnd();
    auto value = safeCast<uint16_t>(resolveAsInt(opMatch.captured(1)));
    emitByte(static_cast<uint8_t>(value));
    emitByte(value >> 8);
  }
}

void Assembler::handleImpliedOrAccumulator() {
  assemble(OperandsFormat::ImpliedOrAccumulator);
}

void Assembler::handleImmediate() {
  assemble(OperandsFormat::Immediate, safeCast<uint8_t>(resolveAsInt(operand())));
}

void Assembler::handleAbsolute() {
  auto addr = safeCast<Address>(resolveAsInt(operand()));
  assemble(addr < 256 ? OperandsFormat::ZeroPage : OperandsFormat::Absolute, addr);
}

void Assembler::handleAbsoluteIndexedX() {
  auto addr = safeCast<Address>(resolveAsInt(operand()));
  assemble(addr < 256 ? OperandsFormat::ZeroPageX : OperandsFormat::AbsoluteX, addr);
}

void Assembler::handleAbsoluteIndexedY() {
  auto addr = safeCast<Address>(resolveAsInt(operand()));
  assemble(addr < 256 ? OperandsFormat::ZeroPageY : OperandsFormat::AbsoluteY, addr);
}

void Assembler::handleIndirect() {
  auto addr = safeCast<Address>(resolveAsInt(operand()));
  assemble(OperandsFormat::Indirect, addr);
}

void Assembler::handleIndexedIndirectX() {
  auto addr = safeCast<uint8_t>(resolveAsInt(operand()));
  assemble(OperandsFormat::IndexedIndirectX, addr);
}

void Assembler::handleIndirectIndexedY() {
  auto addr = safeCast<uint8_t>(resolveAsInt(operand()));
  assemble(OperandsFormat::IndirectIndexedY, addr);
}

void Assembler::handleBranch() {
  assemble(OperandsFormat::Branch, operandAsBranchDisplacement());
}

void Assembler::assemble(OperandsFormat mode, int operand) {
  const auto instruction = resolveInstruction(operation(), mode);
  emitByte(static_cast<uint8_t>(std::distance(InstructionTable.begin(), instruction)));
  if (instruction->size > 1) emitByte(static_cast<uint8_t>(operand));
  if (instruction->size > 2) emitByte(static_cast<uint8_t>(operand >> 8));
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

void Assembler::updateAddressRange(uint16_t addr) {
  addressRange.expand(addr);
}
