#ifndef BYTESPINBOX_H
#define BYTESPINBOX_H

#include <QSpinBox>

class ByteSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit ByteSpinBox(QWidget *parent = nullptr);
    uint8_t byteValue() const { return static_cast<uint8_t>(value()); }

signals:

public slots:

protected:
    QString textFromValue(int val) const override;
};

#endif // BYTESPINBOX_H
