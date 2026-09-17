#include <engine/core/Subsystem.h>

namespace eng {

void SubsystemStack::Add(std::string, Subsystem& subsystem) {
}

bool SubsystemStack::InitAll(const BootConfig& config) {
    return false;
}

void SubsystemStack::ShutdownAll() {

}

} // namespace eng
