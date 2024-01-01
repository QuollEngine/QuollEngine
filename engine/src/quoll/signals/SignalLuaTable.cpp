#include "quoll/core/Base.h"
#include "SignalLuaTable.h"

namespace quoll {

void SignalLuaTable::create(sol::state_view state) {
  auto signalUserSlot =
      state.new_usertype<SignalSlot>("SignalSlot", sol::no_constructor);
  signalUserSlot["disconnect"] = &SignalSlot::disconnect;

  auto signalUserType =
      state.new_usertype<SignalLuaTable>("Signal", sol::no_constructor);
  signalUserType["connect"] = &SignalLuaTable::connect;
}

} // namespace quoll
