#pragma once

#include "quoll/core/SparseSet.h"
#include "LuaHeaders.h"
#include "Script.h"
#include "ScriptSignalSlot.h"

namespace quoll {

/**
 * @brief Script signal
 */
class ScriptSignal {
  using Handler = sol::function;

public:
  /**
   * @brief Connect signal slot
   *
   * @param handler Handler
   * @return Signal slot
   */
  ScriptSignalSlot connect(Handler handler) {
    auto id = mHandlers.insert(handler);
    auto disconnect = std::bind(&ScriptSignal::disconnect, this, id);

    return ScriptSignalSlot(disconnect);
  }

  /**
   * @brief Notify all slots
   *
   * @tparam ...TArgs Argument types
   * @param ...args Arguments
   */
  template <class... TArgs> void notify(TArgs... args) {
    for (auto handler : mHandlers) {
      handler(args...);
    }
  }

  /**
   * @brief Disconnect slot
   *
   * @param id Slot id
   */
  void disconnect(usize id) {
    if (mHandlers.contains(id)) {
      mHandlers.erase(id);
    }
  }

private:
  SparseSet<Handler> mHandlers;
};

/**
 * @brief Create Lua tables for script signals
 *
 * @param state Sol state
 */
void createScriptSignalTables(sol::state_view state);

} // namespace quoll
