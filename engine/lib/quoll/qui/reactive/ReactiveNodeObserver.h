#pragma once

#include "quoll/core/SparseSet.h"

namespace qui::rgraph {

class ReactiveNodeObserver {
public:
  using ObserverFn = std::function<void()>;
  using ObserverSet = quoll::SparseSet<ObserverFn>;

public:
  constexpr ReactiveNodeObserver() = default;
  constexpr ReactiveNodeObserver(ObserverSet *observers, usize id)
      : mObservers(observers), mId(id) {}

  void unobserve();

  constexpr usize getId() const { return mId; }

  constexpr operator bool() const { return mId != ObserverSet::Empty; }

private:
  ObserverSet *mObservers = nullptr;
  usize mId = ObserverSet::Empty;
};

} // namespace qui::rgraph
