#pragma once

enum class AssemblyResult {
  Ok,
  SymbolAlreadyDefined,
  SymbolNotDefined,
  MissingOperand,
  NumericOperandRequired,
  SyntaxError,
  CommandProcessingError
};

const char* formatAssemblyResult(AssemblyResult);
