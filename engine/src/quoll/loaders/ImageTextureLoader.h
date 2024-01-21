#pragma once

#include "quoll/rhi/RenderHandle.h"

namespace quoll {

class RenderStorage;

class ImageTextureLoader {
public:
  ImageTextureLoader(RenderStorage &renderStorage);

  rhi::TextureHandle loadFromFile(const Path &filename);

private:
  RenderStorage &mRenderStorage;
};

} // namespace quoll
