#pragma once

enum class AssemblyResult {
  Ok,
  SymbolAlreadyDefined,
  SymbolNotDefined,
  MissingOperand,
  NumericOperandRequired,
  SyntaxError,
  CommandProcessingError,
  BranchTooFar,
  ValueOutOfRange
};

const char* formatAssemblyResult(AssemblyResult);
