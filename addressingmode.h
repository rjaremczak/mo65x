#pragma once

enum AddressingMode {

  // no operands

  Implied,
  Accumulator,

  // 8-bit operand

  Relative,
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

  // no operands, implied or accumulator

  NoOperands
};
