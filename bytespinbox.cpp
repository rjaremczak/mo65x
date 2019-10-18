#include "bytespinbox.h"
#include <QFontDatabase>

ByteSpinBox::ByteSpinBox(QWidget *parent) : QSpinBox(parent)
{
    setRange(0,255);
    setDisplayIntegerBase(16);
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
}

QString ByteSpinBox::textFromValue(int val) const
{
    return QString("%1").arg(val, 2, 16, QChar('0')).toUpper();
}
