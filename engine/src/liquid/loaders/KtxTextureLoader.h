#pragma once

#include "liquid/rhi/ResourceRegistry.h"

namespace liquid {

/**
 * @brief KTX texture loader
 */
class KtxTextureLoader {
public:
  /**
   * @brief Create KTX texture loader
   *
   * @param registry Resource registry
   */
  KtxTextureLoader(rhi::ResourceRegistry &registry);

  /**
   * @brief Load texture from KTX file
   *
   * @param filename Filename
   * @return Texture
   */
  rhi::TextureHandle loadFromFile(const String &filename);

private:
  rhi::ResourceRegistry &mRegistry;
};

} // namespace liquid
