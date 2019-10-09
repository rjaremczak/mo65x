#pragma once

enum AddressingMode
{
    Accumulator,
    Immediate,
    Implied,

    Relative,
    Absolute,
    ZeroPage,
    Indirect,

    AbsoluteIndexed,
    ZeroPageIndexed,
    IndexedXIndirect,
    IndirectIndexedY,

    AbsoluteIndirect,
    IndexedAbsoluteIndirect,

    Unknown
};
