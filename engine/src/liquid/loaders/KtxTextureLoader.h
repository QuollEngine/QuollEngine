#pragma once

#include "liquid/rhi/ResourceRegistry.h"

namespace liquid {

class KtxTextureLoader {
public:
  /**
   * @brief Create KTX texture loader
   *
   * @param registry Resource registry
   */
  KtxTextureLoader(experimental::ResourceRegistry &registry);

  /**
   * @brief Load texture from KTX file
   *
   * @param filename Filename
   * @return Texture
   */
  TextureHandle loadFromFile(const String &filename);

private:
  experimental::ResourceRegistry &registry;
};

} // namespace liquid
