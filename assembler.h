#pragma once

#include "assemblerline.h"
#include "assemblerresult.h"
#include "commons.h"
#include "instruction.h"
#include <QString>
#include <iterator>
#include <map>
#include <optional>
#include <vector>

class Assembler
{
public:
  using Symbols = std::map<QString, uint16_t>;

  static constexpr uint16_t DefaultOrigin = 0;

  enum class Mode { ScanForSymbols, EmitCode };

  Assembler();

  uint16_t origin() const { return origin_; }
  const auto& symbols() const { return symbols_; }
  auto locationCounter() const { return locationCounter_; }
  const Data& code() const { return code_; }

  AssemblerResult defineOrigin(uint16_t addr);
  void resetOrigin(uint16_t addr = DefaultOrigin);
  void clearCode() { code_.clear(); }
  void clearSymbols() { symbols_.clear(); }
  void changeMode(Mode mode) { mode_ = mode; }
  uint8_t lastInstructionByte(size_t) const;
  AssemblerResult process(const QString&);
  std::optional<int> symbol(const QString&) const;

private:
  Mode mode_ = Mode::EmitCode;
  bool originDefined_ = false;
  uint16_t origin_ = DefaultOrigin;
  uint16_t locationCounter_ = DefaultOrigin;
  Data code_;
  size_t lastInstructionIndex_;
  std::back_insert_iterator<Data> iterator_;
  Symbols symbols_;

  AssemblerResult addSymbol(const QString&, uint16_t);
  AssemblerResult processControlCommand(const AssemblerLine&);
  AssemblerResult processInstruction(const AssemblerLine&);
  AssemblerResult cmdSetOrigin(const AssemblerLine&);
  AssemblerResult cmdEmitByte(const AssemblerLine&);
  AssemblerResult assemble(InstructionType type, OperandsFormat mode, int operand = 0);
  void addByte(uint8_t);
};
