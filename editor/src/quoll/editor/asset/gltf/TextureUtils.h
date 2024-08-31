#pragma once

#include "GLTFImportData.h"

namespace quoll::editor {

enum class GLTFTextureColorSpace { Linear, Srgb };

AssetRef<TextureAsset> loadTexture(GLTFImportData &importData, usize index,
                                   GLTFTextureColorSpace colorSpace,
                                   bool generateMipMaps);

} // namespace quoll::editor
