#include "quoll/core/Base.h"
#include "ScriptSignalView.h"

namespace quoll {

ScriptSignalView::ScriptSignalView(ScriptSignal &signal, Script &script)
    : mSignal(signal), mScript(script) {}

ScriptSignalSlot ScriptSignalView::connect(Handler handler) {
  auto slot = mSignal.connect(handler);
  mScript.signalSlots.push_back(slot);
  return slot;
}

} // namespace quoll
