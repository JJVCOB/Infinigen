#include <engine/tools/GuiHooks.h>

namespace eng {

namespace { GuiHooks g_hooks; }
void SetGuiHooks(const GuiHooks& hooks) { g_hooks = hooks; }
const GuiHooks& GetGuiHooks() { return g_hooks; }

} // namespace eng