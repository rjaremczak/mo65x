#include "system.h"
#include <algorithm>

System::System(QObject *parent) : QObject(parent),
    m_memory(),
    m_cpu(m_memory),
    m_cpuProbe(m_cpu)
{
    std::generate(m_memory.begin(), m_memory.end(), []{ return std::rand(); });
}

void System::fillMemory(uint16_t first, uint16_t last, uint8_t value)
{
    std::fill(m_memory.begin() + first, m_memory.begin() + last + 1, value);
    emit memoryContentChanged(first, last);
}

size_t System::loadMemory(uint16_t first, const QByteArray &data)
{
    auto size = static_cast<uint16_t>(std::min(static_cast<size_t>(data.size()), m_memory.size() - first));
    std::copy_n(data.begin(), size, m_memory.begin() + first);
    emit memoryContentChanged(first, first + size);
    return size;
}

QByteArray System::saveMemory(uint16_t first, uint16_t last)
{
    QByteArray data;
    std::copy(m_memory.cbegin() + first, m_memory.cbegin() + last + 1, std::back_inserter(data));
    return data;
}

void System::checkCpuState()
{
    emit cpuStateChanged(m_cpuProbe.registers(), m_cpuProbe.flags());
}

void System::changePC(uint16_t pc)
{
    if(m_cpuProbe.registers().pc != pc) {
        m_cpuProbe.registers().pc = pc;
        emit cpuStateChanged(m_cpuProbe.registers(), m_cpuProbe.flags());
    }
}
