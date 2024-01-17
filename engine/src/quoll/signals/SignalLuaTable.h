#pragma once

#include "quoll/core/Engine.h"
#include "quoll/lua-scripting/LuaScript.h"
#include "quoll/lua-scripting/LuaUserTypeBase.h"
#include "Signal.h"

namespace quoll {

class SignalLuaTable {
public:
  template <class... TArgs>
  SignalLuaTable(Signal<TArgs...> &signal, LuaScript &script) {
    mConnector = [&signal, &script](sol::protected_function fn) {
      auto slot = signal.connect([fn](TArgs &...args) {
        auto res = fn(args...);
        if (!res.valid()) {
          sol::error error = res;
          Engine::getUserLogger().error() << error.what();
        }
      });
      script.signalSlots.push_back(slot);
      return slot;
    };
  }

  inline SignalSlot connect(sol::protected_function fn) {
    return mConnector(fn);
  }

  static void create(sol::state_view state);

private:
  std::function<SignalSlot(sol::protected_function)> mConnector;
};

} // namespace quoll
