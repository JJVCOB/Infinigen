#pragma once

#include <string>
#include <vector>

namespace eng {

struct BootConfig;

class Subsystem {
public:
    virtual ~Subsystem() = default;
    virtual bool Init(const BootConfig& config) = 0;
    virtual void Shutdown() = 0;
};

class SubsystemStack {
public:
    void Add(std::string name, Subsystem& subsystem);
    bool InitAll(const BootConfig& config);
    void ShutdownAll();
    std::size_t Count() const { return m_entries.size(); }

private:
    struct Entry
    {
        std::string name;
        Subsystem* system = nullptr;
    };
    std::vector<Entry> m_entries;
    std::size_t m_startedCount = 0;
};

} // namespace eng
