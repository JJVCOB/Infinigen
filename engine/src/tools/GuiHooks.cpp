#include <engine/tools/GuiHooks.h>

namespace eng {

void SetGuiHooks(const GuiHooks& hooks) {
}

const GuiHooks& GetGuiHooks() {
    static const GuiHooks none{};
    return none;
}

} // namespace eng
