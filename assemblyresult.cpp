#include "assemblyresult.h"

const char* formatAssemblyResult(AssemblyResult ar) {
  switch (ar) {
  case AssemblyResult::Ok: return "Ok";
  case AssemblyResult::SymbolAlreadyDefined: return "symbol already defined";
  case AssemblyResult::SymbolNotDefined: return "undefined symbol";
  case AssemblyResult::MissingOperand: return "missing operand";
  case AssemblyResult::NumericOperandRequired: return "numeric operand required";
  case AssemblyResult::SyntaxError: return "syntax error";
  case AssemblyResult::CommandProcessingError: return "command processing error";
  }
  return nullptr;
}
