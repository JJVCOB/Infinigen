#pragma once

namespace eng {

struct GuiHooks {
    bool (*ProcessEvent)(const void* platformEvent) = nullptr;
    bool (*WantsKeyboard)() = nullptr;
    bool (*WantsMouse)() = nullptr;
};

void SetGuiHooks(const GuiHooks& hooks);
const GuiHooks& GetGuiHooks();

} // namespace eng
