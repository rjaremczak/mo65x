#include "bytespinbox.h"
#include <QFontDatabase>

static int fieldWidth(int base) {
  switch (base) {
  case 16: return 2;
  case 10: return 3;
  default: return 8;
  }
}
ByteSpinBox::ByteSpinBox(QWidget* parent) : QSpinBox(parent) {
  setRange(0, 255);
  setDisplayIntegerBase(16);
}

QString ByteSpinBox::textFromValue(int val) const {
  const auto base = displayIntegerBase();
  return QString("%1").arg(val, fieldWidth(base), base, QChar('0')).toUpper();
}
