#include "quoll/core/Base.h"
#include "ScriptSignalView.h"

namespace quoll::lua {

ScriptSignalView::ScriptSignalView(ScriptSignal &signal, LuaScript &script)
    : mSignal(signal), mScript(script) {}

ScriptSignalSlot ScriptSignalView::connect(Handler handler) {
  auto slot = mSignal.connect(handler);
  mScript.signalSlots.push_back(slot);
  return slot;
}

} // namespace quoll::lua
