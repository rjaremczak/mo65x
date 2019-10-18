#ifndef WORDSPINBOX_H
#define WORDSPINBOX_H

#include <QWidget>
#include <QSpinBox>

class WordSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit WordSpinBox(QWidget *parent = nullptr);
    uint16_t wordValue() const { return static_cast<uint16_t>(value()); }

signals:

public slots:

protected:
    QString textFromValue(int val) const override;
};

#endif // WORDSPINBOX_H
