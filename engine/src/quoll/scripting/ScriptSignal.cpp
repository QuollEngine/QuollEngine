#include "quoll/core/Base.h"
#include "ScriptSignal.h"
#include "ScriptSignalView.h"

namespace quoll {

void createScriptSignalTables(sol::state_view state) {
  auto signalUserSlot =
      state.new_usertype<ScriptSignalSlot>("SignalSlot", sol::no_constructor);
  signalUserSlot["disconnect"] = &ScriptSignalSlot::disconnect;

  auto signalUserType =
      state.new_usertype<ScriptSignalView>("Signal", sol::no_constructor);
  signalUserType["connect"] = &ScriptSignalView::connect;
}

} // namespace quoll
