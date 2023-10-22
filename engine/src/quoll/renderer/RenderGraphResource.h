#pragma once

#include "RenderGraphRegistry.h"

namespace quoll {

/**
 * @brief Render graph resource
 *
 * @tparam THandle Real resource handle
 */
template <class THandle> class RenderGraphResource {
public:
  /**
   * @brief Create render graph resource
   *
   * @param registry Render graph registry
   * @param index Resource index
   */
  RenderGraphResource(RenderGraphRegistry &registry, usize index)
      : mRegistry(registry), mIndex(index) {}

  /**
   * @brief Get real resource handle
   *
   * @return Real resource handle
   */
  inline operator THandle() const { return getHandle(); }

  /**
   * @brief Get render graph index
   *
   * @return Render graph index
   */
  inline usize getIndex() const { return mIndex; }

  /**
   * @brief Get real resource handle
   *
   * @return Real resource handle
   */
  inline THandle getHandle() const { return mRegistry.get<THandle>(mIndex); }

  /**
   * @brief Set on ready callback
   *
   * @param onReadyFn On ready function
   * @return This class
   */
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
