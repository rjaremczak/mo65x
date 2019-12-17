#pragma once

enum class AssemblyResult {
  Ok,
  SymbolAlreadyDefined,
  SymbolNotDefined,
  MissingOperand,
  NumericOperandRequired,
  SyntaxError,
  CommandProcessingError,
  ValueOutOfRange,
  InvalidMnemonic,
  InvalidInstructionFormat
};

const char* formatAssemblyResult(AssemblyResult);
