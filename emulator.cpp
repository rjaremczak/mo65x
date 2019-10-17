#include "emulator.h"
#include <algorithm>

Emulator::Emulator(QObject *parent) : QObject(parent),
    m_memory(),
    m_cpu(m_memory)
{
    std::generate(m_memory.begin(), m_memory.end(), []{ return std::rand(); });
}

void Emulator::fillMemory(size_t start, size_t size, uint8_t value)
{
    std::fill_n(m_memory.begin() + start, size, value);
}

void Emulator::loadMemory(size_t start, const QByteArray &data)
{
    const auto size = std::min(static_cast<size_t>(data.size()), m_memory.size() - start);
    std::copy_n(data.begin(), size, m_memory.begin() + start);
    emit memoryContentChanged(start, size);
}

QByteArray Emulator::saveMemory(size_t start, size_t size)
{
    QByteArray data;
    std::copy_n(m_memory.cbegin() + start, size, std::back_inserter(data));
    return data;
}

void Emulator::checkCpuRegisters()
{
    emit cpuRegistersChanged(m_cpu.registers());
}
