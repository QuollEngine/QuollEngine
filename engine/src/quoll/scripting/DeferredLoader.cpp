#include "quoll/core/Base.h"
#include "DeferredLoader.h"

namespace quoll {

DeferredLoader &DeferredLoader::operator=(std::function<void()> callback) {
  mCallback = callback;
  return *this;
}

void DeferredLoader::wait() {
  if (!mIsExecuted && mCallback) {
    mCallback();
    mIsExecuted = true;
  }
}

} // namespace quoll
