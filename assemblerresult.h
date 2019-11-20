#pragma once

enum class AssemblerResult {
  Ok,
  SymbolAlreadyDefined,
  SymbolNotDefined,
  MissingOperand,
  NumericOperandRequired,
  SyntaxError,
  CommandProcessingError
};

const char* assemblerResultStr(AssemblerResult);
