#include "assembler.h"
#include "instructiontable.h"
#include "mnemonics.h"

static constexpr auto LabelGroup = 1;
static constexpr auto OperationGroup = 2;
static constexpr auto FirstOperandGroup = 3;
static constexpr auto HexPrefix = '$';
static constexpr auto BinPrefix = '%';
static constexpr auto LoBytePrefix = '<';
static constexpr auto HiBytePrefix = '>';

static const std::string Symbol{"[a-z]\\w*"};
static const std::string Label{"^(?:(" + Symbol + "):)?\\s*"};
static const std::string Comment{"(?:;.*)?$"};
static const std::string OrgCmd("((?:\\.ORG\\s+)|(?:\\*\\s*\\=\\s*))");
static const std::string ByteCmd("(\\.BYTE|DCB)\\s+");
static const std::string WordCmd("(\\.WORD)\\s+");
static const std::string HexNum("\\$[\\d|a-h]{1,4}");
static const std::string DecNum("\\d{1,5}");
static const std::string BinNum("\\%[01]{1,16}");
static const std::string Mnemonic("([a-z]{3})\\s*");
static const std::string NumOrSymbol("(?:" + HexNum + ")|(?:" + DecNum + ")|(?:" + BinNum + ")|(?:" + Symbol + ")");
static const std::string LoHiPrefix("[" + std::string(1, LoBytePrefix) + "|" + std::string(1, HiBytePrefix) + "]?");
static const std::string Operand("(" + LoHiPrefix + "(?:" + NumOrSymbol + "))\\s*");
static const std::string OperandSeparator("\\s*,?\\s*");
static const std::string OperandList("((?:(?:" + LoHiPrefix + "(?:" + NumOrSymbol + "))" + OperandSeparator + ")+)\\s*");
static const std::string BranchMnemonic("(BCC|BCS|BNE|BEQ|BMI|BPL|BVC|BVS)\\s*");
static const std::string BranchTarget("((?:[+|-]?\\d{1,3})|(?:" + Symbol + "))\\s*");

Assembler::PatternEntry::PatternEntry(const std::string& pattern, const Handler handler)
    : regex(Label + pattern + Comment, std::regex_constants::icase), handler(handler) {
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

static const std::regex OperandItemRegEx(Operand + OperandSeparator, std::regex_constants::icase);

static InstructionType resolveInstructionType(const std::string_view str) {
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

static const Instruction* resolveInstruction(const std::string_view str, OperandsFormat mode, bool zp) {
  std::string sup(str);
  std::transform(sup.begin(), sup.end(), sup.begin(), toupper);
  const auto type = resolveInstructionType(sup);
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

static bool isNumber(const std::string_view str) {
  const auto front = str.front();
  return front == HexPrefix || front == BinPrefix || isdigit(front) || front == '+' || front == '-';
}

static int parseNumber(const std::string_view str) {
  switch (str.front()) {
  case HexPrefix: return strtol(str.begin() + 1, nullptr, 16);
  case BinPrefix: return strtol(str.begin() + 1, nullptr, 2);
  default: return strtol(str.begin(), nullptr, 10);
  }
}

static std::vector<std::string> split(const std::string_view str) {
  std::vector<std::string> items;
  std::cmatch match;
  auto begin = str.cbegin();
  const auto end = str.cend();
  while (std::regex_search(begin, end, match, OperandItemRegEx)) {
    begin += match.length();
    items.emplace_back(match.str(1));
  }
  return items;
}

Assembler::Assembler(MemoryChip& memory, SymbolTable& symbols) : m_memory(memory), m_symbols(symbols) {
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

AssemblyResult Assembler::processLine(const std::string_view str) {
  m_lastLocationCounter = m_locationCounter;
  std::cmatch match;
  for (const auto& entry : Patterns) {
    if (std::regex_match(str.begin(), str.end(), match, entry.regex)) {
      m_operation = match.str(OperationGroup);
      m_operand = match.str(FirstOperandGroup);
      try {
        if (auto label = match.str(LabelGroup); !label.empty()) defineSymbol(label, m_locationCounter);
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

static std::string_view removeLoHiPrefix(char& prefix, const std::string& str) {
  const auto f = str.front();
  if (f == LoBytePrefix || f == HiBytePrefix) {
    prefix = f;
    return std::string_view(str).substr(1, str.length() - 1);
  }
  prefix = 0;
  return str;
}

static int applyLoHiPrefix(char prefix, int num) {
  if (!prefix) return num;
  if (prefix == HiBytePrefix) return num >> 8;
  return num & 0xff;
}

OperandValue Assembler::operandValue(const std::string& ostr) const {
  char prefix;
  const auto str = removeLoHiPrefix(prefix, ostr);
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
  m_locationCounter = safeCast<Address>(operandValue());
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

void Assembler::defineSymbol(const std::string_view name, uint16_t value) {
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
