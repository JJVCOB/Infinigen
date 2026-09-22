#include <engine/core/Log.h>
#include <engine/core/Subsystem.h>

namespace eng {

void SubsystemStack::Add(std::string name, Subsystem& subsystem) {
    Entry entry;
    entry.name = std::move(name);
    entry.system = &subsystem;
    m_entries.push_back(std::move(entry));
}

bool SubsystemStack::InitAll(const BootConfig& config) {
    m_startedCount = 0;

    for (std::size_t i = 0; i < m_entries.size(); i++) {
        const Entry& entry = m_entries[i];

        if (entry.system->Init(config)) {
            ENGINE_LOG_INFO(Channels::kCore, "Started subsystem: [{}/{}] {}", i + 1, m_entries.size(), entry.name);
            m_startedCount++;
            continue;
        }

        ENGINE_LOG_ERROR(Channels::kCore, "Subsystem '{}' failed to start, shutting down the {} subsystems that did start.", entry.name, m_startedCount);

        for (std::size_t j = i; j-- > 0;) {
            m_entries[j].system->Shutdown();
            ENGINE_LOG_INFO(Channels::kCore, "\tStopped subsystem: {}", m_entries[j].name);
        }

        m_startedCount = 0;
        return false;
    }

    return true;
}

void SubsystemStack::ShutdownAll() {
    for (std::size_t i = m_startedCount; i-- > 0;) {
        m_entries[i].system->Shutdown();
        ENGINE_LOG_INFO(Channels::kCore, "Shut down subsystem: [{}/{}] {}", i + 1, m_entries.size(), m_entries[i].name);
    }

    m_startedCount = 0;
}

} // namespace eng
