#include "wordspinbox.h"
#include <QFontDatabase>

WordSpinBox::WordSpinBox(QWidget *parent) : QSpinBox(parent)
{
    setRange(0, 65535);
    setDisplayIntegerBase(16);
}

QString WordSpinBox::textFromValue(int val) const
{
    return QString("%1").arg(val, 4, 16, QChar('0')).toUpper();
}
