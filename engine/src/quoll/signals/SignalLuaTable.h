#pragma once

#include "Signal.h"
#include "quoll/core/Engine.h"
#include "quoll/lua-scripting/LuaUserTypeBase.h"
#include "quoll/lua-scripting/LuaScript.h"

namespace quoll {

/**
 * @brief Signal lua table
 */
class SignalLuaTable {
public:
  /**
   * @brief Create signal lua table
   *
   * @tparam ...TArgs Signal argument types
   * @param signal Signal
   * @param script Lua script
   */
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

  /**
   * @brief Connect lua function
   *
   * @param fn Lua function
   * @return Signal slot
   */
  inline SignalSlot connect(sol::protected_function fn) {
    return mConnector(fn);
  }

  /**
   * @brief Create signal lua user type
   *
   * @param state Sol state
   */
  static void create(sol::state_view state);

private:
  std::function<SignalSlot(sol::protected_function)> mConnector;
};

} // namespace quoll
