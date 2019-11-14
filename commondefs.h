#pragma once

#include <QMetaType>
#include <stdint.h>
#include <vector>

using Byte = uint8_t;
using Bytes = std::vector<Byte>;
using BytesIterator = Bytes::iterator;
using Address = uint16_t;

Q_DECLARE_METATYPE(Byte)
Q_DECLARE_METATYPE(Bytes)
Q_DECLARE_METATYPE(BytesIterator)
Q_DECLARE_METATYPE(Address)
