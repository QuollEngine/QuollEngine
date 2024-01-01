#include "quoll/core/Base.h"
#include "SignalSlot.h"

namespace quoll {

SignalSlot::SignalSlot(std::function<void()> &&disconnect)
    : mDisconnect(disconnect) {}

} // namespace quoll
