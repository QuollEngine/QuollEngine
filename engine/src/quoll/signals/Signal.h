#pragma once

#include "quoll/core/SparseSet.h"
#include "SignalSlot.h"

namespace quoll {

template <class... TArgs> class Signal {
  using Handler = std::function<void(TArgs &...)>;

public:
  SignalSlot connect(Handler handler) {
    auto id = mHandlers.insert(handler);
    auto disconnect = std::bind(&Signal::disconnect, this, id);

    return SignalSlot(disconnect);
  }

  void notify(TArgs... args) {
    for (auto handler : mHandlers) {
      handler(args...);
    }
  }

  void disconnect(usize id) {
    if (mHandlers.contains(id)) {
      mHandlers.erase(id);
    }
  }

private:
  SparseSet<Handler> mHandlers;
};

} // namespace quoll
