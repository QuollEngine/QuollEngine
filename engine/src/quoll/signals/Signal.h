#pragma once

#include "quoll/core/SparseSet.h"
#include "SignalSlot.h"

namespace quoll {

/**
 * @brief Signal
 *
 * @tparam ...TArgs Signal argument types
 */
template <class... TArgs> class Signal {
  using Handler = std::function<void(TArgs &...)>;

public:
  /**
   * @brief Connect signal slot
   *
   * @param handler Handler
   * @return Signal slot
   */
  SignalSlot connect(Handler handler) {
    auto id = mHandlers.insert(handler);
    auto disconnect = std::bind(&Signal::disconnect, this, id);

    return SignalSlot(disconnect);
  }

  /**
   * @brief Notify all slots
   *
   * @param ...args Arguments
   */
  void notify(TArgs... args) {
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

} // namespace quoll
