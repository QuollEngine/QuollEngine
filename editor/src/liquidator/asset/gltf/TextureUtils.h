#pragma once

#include "GLTFImportData.h"

namespace quoll::editor {

enum class GLTFTextureColorSpace { Linear, Srgb };

/**
 * @brief Load texture into registry
 *
 * @param importData GLTF import data
 * @param index Texture index
 * @param colorSpace Texture color space
 * @param generateMipMaps Generate mip maps
 * @return Texture asset handle
 */
TextureAssetHandle loadTexture(GLTFImportData &importData, size_t index,
                               GLTFTextureColorSpace colorSpace,
                               bool generateMipMaps);

} // namespace quoll::editor
