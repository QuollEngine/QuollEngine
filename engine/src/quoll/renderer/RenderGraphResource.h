#pragma once

#include "RenderGraphRegistry.h"

namespace quoll {

template <class THandle> class RenderGraphResource {
public:
  RenderGraphResource(RenderGraphRegistry &registry, usize index)
      : mRegistry(registry), mIndex(index) {}

  inline operator THandle() const { return getHandle(); }

  inline usize getIndex() const { return mIndex; }

  inline THandle getHandle() const { return mRegistry.get<THandle>(mIndex); }

  template <class TFunction>
  inline RenderGraphResource &onReady(TFunction &&onReadyFn) {
    mRegistry.setResourceReady<THandle, TFunction>(
        mIndex, std::forward<TFunction>(onReadyFn));
    return *this;
  }

private:
  RenderGraphRegistry &mRegistry;
  usize mIndex;
};

} // namespace quoll
