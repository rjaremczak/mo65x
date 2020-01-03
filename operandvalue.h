#pragma once

struct OperandValue {
  enum Type { Literal, Identifier, UndefinedIdentifier };

  const Type type = UndefinedIdentifier;
  const int value = 0;

  bool isLiteral() const { return type == Literal; }
  bool isIdentifier() const { return type == Identifier; }

  operator int() const { return value; }
};
