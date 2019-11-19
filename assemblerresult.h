#pragma once

enum class AssemblerResult {
  Ok,
  OriginAlreadyDefined,
  SymbolAlreadyDefined,
  UndefinedSymbol,
  MissingOperand,
  NumericOperandRequired,
  SyntaxError,
  CommandProcessingError
};

const char* assemblerResultStr(AssemblerResult);
