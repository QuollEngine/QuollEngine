#include "quoll/core/Base.h"
#include "ReactiveNodeObserver.h"

namespace qui::rgraph {

void ReactiveNodeObserver::unobserve() {
  if (mObservers->contains(mId)) {
    mObservers->erase(mId);
  }
  mId = ObserverSet::Empty;
}

} // namespace qui::rgraph
