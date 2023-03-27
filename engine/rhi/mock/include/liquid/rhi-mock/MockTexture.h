#pragma once

#include "liquid/rhi/TextureDescription.h"

namespace liquid::rhi {

/**
 * @brief Mock texture
 */
class MockTexture {
public:
  /**
   * @brief Create mock texture
   *
   * @param description Texture description
   */
  MockTexture(const TextureDescription &description);

  /**
   * @brief Get description
   *
   * @return Texture description
   */
  inline const TextureDescription &getDescription() const {
    return mDescription;
  }

private:
  std::vector<uint8_t> mData;
  TextureDescription mDescription;
};

} // namespace liquid::rhi
