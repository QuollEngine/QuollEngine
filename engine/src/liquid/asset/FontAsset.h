#pragma once

#include <msdf-atlas-gen/BitmapAtlasStorage.h>
#include <msdf-atlas-gen/GlyphGeometry.h>

namespace liquid {

/**
 * @brief Font asset data
 */
struct FontAsset {
  /**
   * MSDF font atlas
   */
  msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3> msdfAtlas;

  /**
   * Information about all glyphs
   */
  std::vector<msdf_atlas::GlyphGeometry> msdfGlyphs;

  /**
   * Device handle
   */
  rhi::TextureHandle deviceHandle = rhi::TextureHandle::Invalid;
};

} // namespace liquid
