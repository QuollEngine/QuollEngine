#include "quoll/core/Base.h"
#include "ScriptSignalSlot.h"

namespace quoll {

ScriptSignalSlot::ScriptSignalSlot(std::function<void()> &&disconnect)
    : mDisconnect(disconnect) {}

} // namespace quoll
