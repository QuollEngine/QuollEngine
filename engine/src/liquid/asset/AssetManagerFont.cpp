#include "liquid/core/Base.h"
#include "AssetManager.h"

#include <ft2build.h>
#include <freetype/freetype.h>

#include "liquid/font/MsdfAtlas.h"

#include "FontAsset.h"

namespace liquid {

Result<FontAssetHandle> AssetManager::loadFontFromFile(const Path &filePath) {
  constexpr double MAX_CORNER_ANGLE = 3.0;
  constexpr double MINIMUM_SCALE = 24.0;
  constexpr double PIXEL_RANGE = 2.0;

  using namespace msdf_atlas;

  auto *ft = msdfgen::initializeFreetype();
  if (!ft) {
    return Result<FontAssetHandle>::Error("Failed to initialize freetype");
  }

  auto *font = msdfgen::loadFont(ft, filePath.string().c_str());

  if (font == nullptr) {
    msdfgen::deinitializeFreetype(ft);
    return Result<FontAssetHandle>::Error("Failed to load font: " +
                                          filePath.string());
  }

  std::vector<GlyphGeometry> glyphs;
  FontGeometry fontGeometry(&glyphs);
  fontGeometry.loadCharset(font, 1.0, Charset::ASCII);

  for (auto &glyph : glyphs) {
    glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, MAX_CORNER_ANGLE, 0);
  }

  TightAtlasPacker packer;
  packer.setDimensionsConstraint(
      TightAtlasPacker::DimensionsConstraint::SQUARE);
  packer.setMinimumScale(MINIMUM_SCALE);
  packer.setPixelRange(PIXEL_RANGE);
  packer.setMiterLimit(1.0);
  packer.pack(glyphs.data(), static_cast<int>(glyphs.size()));

  int32_t width = 0, height = 0;
  packer.getDimensions(width, height);

  ImmediateAtlasGenerator<float, 3, &msdfGenerator, BitmapAtlasStorage<byte, 3>>
      generator(width, height);

  GeneratorAttributes attributes;
  generator.setAttributes(attributes);
  generator.setThreadCount(1);
  generator.generate(glyphs.data(), static_cast<int>(glyphs.size()));

  AssetData<FontAsset> fontAsset{};
  fontAsset.name = filePath.filename().string();
  fontAsset.path = filePath;
  fontAsset.relativePath = std::filesystem::relative(filePath, mAssetsPath);
  fontAsset.type = AssetType::Font;
  fontAsset.data.msdfGlyphs = glyphs;
  fontAsset.data.msdfAtlas = generator.atlasStorage();

  auto handle = mRegistry.getFonts().addAsset(fontAsset);

  msdfgen::destroyFont(font);
  msdfgen::deinitializeFreetype(ft);

  return Result<FontAssetHandle>::Ok(handle);
}

} // namespace liquid
