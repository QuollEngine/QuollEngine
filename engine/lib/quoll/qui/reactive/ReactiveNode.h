#pragma once

#include "ReactiveArena.h"
#include "ReactiveNodeObserver.h"

namespace qui::rgraph {

class ReactiveNode;

struct ReactiveNodeGlobals {
  ReactiveArena arena;
  ReactiveNode *activeNode = nullptr;
};

class ReactiveNode {
public:
  virtual ~ReactiveNode() = default;

  virtual void update() = 0;

public:
  void addDependent(ReactiveNode *node);

  void removeDependent(ReactiveNode *node);

  ReactiveNodeObserver observe(ReactiveNodeObserver::ObserverFn &&observer) {
    auto id = mObservers.insert(
        std::forward<ReactiveNodeObserver::ObserverFn>(observer));
    return ReactiveNodeObserver(&mObservers, id);
  }

  void notify();

private:
  ReactiveNodeObserver::ObserverSet mObservers;
  std::unordered_map<ReactiveNode *, bool> mDependents;
};

template <typename TData> class ReactiveNodeWithData : public ReactiveNode {
public:
  virtual TData &get() = 0;
};

} // namespace qui::rgraph
