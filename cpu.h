#pragma once

#include <QObject>

#include "cpustate.h"

class Cpu : public QObject
{
    Q_OBJECT

public:
    explicit Cpu(QObject *parent = nullptr);

signals:
    void stateChanged(CpuState);

public slots:
    void checkState();

private:
    CpuState m_state;
};
