#pragma once

#include "liquid/rhi/TextureDescription.h"
#include "liquid/rhi/TextureViewDescription.h"

namespace quoll::rhi {

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
   * @brief Create mock texture view
   *
   * @param description Texture viewdescription
   */
  MockTexture(const TextureViewDescription &description);

  /**
   * @brief Get description
   *
   * @return Texture description
   */
  inline const TextureDescription &getDescription() const {
    return mDescription;
  }

  /**
   * @brief Get view description
   *
   * @return Texture view description
   */
  inline const TextureViewDescription &getViewDescription() const {
    return mViewDescription;
  }

private:
  std::vector<uint8_t> mData;
  TextureDescription mDescription;
  TextureViewDescription mViewDescription;
};

} // namespace quoll::rhi
