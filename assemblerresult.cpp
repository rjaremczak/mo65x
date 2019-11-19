#include "assemblerresult.h"

const char* assemblerResultStr(AssemblerResult ar) {
  switch (ar) {
  case AssemblerResult::Ok: return "Ok";
  case AssemblerResult::OriginAlreadyDefined: return "origin already defined";
  case AssemblerResult::SymbolAlreadyDefined: return "symbol already defined";
  case AssemblerResult::UndefinedSymbol: return "undefined symbol";
  case AssemblerResult::MissingOperand: return "missing operand";
  case AssemblerResult::NumericOperandRequired: return "numeric operand required";
  case AssemblerResult::SyntaxError: return "syntax error";
  case AssemblerResult::CommandProcessingError: return "command processing error";
  }
  return nullptr;
}
