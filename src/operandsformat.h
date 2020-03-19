#pragma once

enum OperandsFormat {

  // no operands

  ImpliedOrAccumulator,

  // 8-bit operand

  Branch,
  Immediate,
  ZeroPage,
  ZeroPageX,
  ZeroPageY,
  IndexedIndirectX,
  IndirectIndexedY,

  // 16-bit operand

  Indirect,
  Absolute,
  AbsoluteX,
  AbsoluteY,
};
