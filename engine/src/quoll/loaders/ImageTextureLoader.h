#pragma once

#include "quoll/renderer/RenderStorage.h"

namespace quoll {

class ImageTextureLoader {
public:
  ImageTextureLoader(RenderStorage &renderStorage);

  rhi::TextureHandle loadFromFile(const Path &filename);

private:
  RenderStorage &mRenderStorage;
};

} // namespace quoll
