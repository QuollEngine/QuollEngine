#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquid {

/**
 * @brief Render graph registry
 */
class RenderGraphRegistry {
public:
  /**
   * @brief Render graph registry
   *
   * @tparam THandle Handle type
   * @param index Index
   * @return Resource handle
   */
  template <class THandle> inline THandle get(size_t index) {
    return static_cast<rhi::TextureHandle>(mRealResources.at(index));
  }

  /**
   * @brief Get all real resources
   *
   * @return Real resources
   */
  inline std::vector<uint32_t> &getRealResources() { return mRealResources; }

private:
  std::vector<uint32_t> mRealResources;
};

} // namespace liquid
