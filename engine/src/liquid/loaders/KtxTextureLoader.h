#pragma once

#include "liquid/renderer/Texture.h"
#include "liquid/renderer/ResourceAllocator.h"

namespace liquid {

class KtxTextureLoader {
public:
  /**
   * @brief Create KTX texture loader
   *
   * @param resourceAllocator Resource allocator
   */
  KtxTextureLoader(ResourceAllocator *resourceAllocator);

  /**
   * @brief Load texture from KTX file
   *
   * @param filename Filename
   * @return Texture
   */
  SharedPtr<Texture> loadFromFile(const String &filename);

private:
  ResourceAllocator *resourceAllocator;
};

} // namespace liquid
