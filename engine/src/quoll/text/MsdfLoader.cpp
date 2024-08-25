#include "quoll/core/Base.h"
#include "quoll/rhi/RenderHandle.h"
#include "MsdfAtlas.h"
#include "MsdfLoader.h"
#include <freetype/freetype.h>
#include <ft2build.h>

namespace quoll {

Result<AssetData<FontAsset>> MsdfLoader::loadFontData(const Path &path) {
  static constexpr f64 MaxCornerAngle = 3.0;
  static constexpr f64 GlyphScale = 40.0;
  static constexpr f64 PixelRange = 2.0;
  static constexpr f64 FontScale = 1.0;

  using namespace msdf_atlas;

  auto *ft = msdfgen::initializeFreetype();
  if (!ft) {
    return Result<AssetData<FontAsset>>::Error("Failed to initialize freetype");
  }

  auto *font = msdfgen::loadFont(ft, path.string().c_str());

  if (font == nullptr) {
    msdfgen::deinitializeFreetype(ft);
    return Result<AssetData<FontAsset>>::Error("Failed to load font: " +
                                               path.string());
  }

  std::vector<GlyphGeometry> msdfGlyphs;
  FontGeometry fontGeometry(&msdfGlyphs);
  fontGeometry.loadCharset(font, FontScale, Charset::ASCII);

  for (auto &glyph : msdfGlyphs) {
    glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, MaxCornerAngle, 0);
  }

  TightAtlasPacker packer;
  packer.setDimensionsConstraint(
      TightAtlasPacker::DimensionsConstraint::SQUARE);
  packer.setDimensionsConstraint(
      TightAtlasPacker::DimensionsConstraint::POWER_OF_TWO_SQUARE);
  packer.setMinimumScale(GlyphScale);
  packer.setPixelRange(PixelRange);
  packer.setMiterLimit(1.0);
  packer.setPadding(0);
  packer.pack(msdfGlyphs.data(), static_cast<int>(msdfGlyphs.size()));

  int width = 0, height = 0;
  packer.getDimensions(width, height);

  static constexpr u32 NumChannels = 4;
  ImmediateAtlasGenerator<f32, NumChannels, mtsdfGenerator,
                          BitmapAtlasStorage<byte, NumChannels>>
      generator(width, height);

  GeneratorAttributes attributes;
  attributes.config.overlapSupport = true;
  attributes.scanlinePass = true;

  generator.setAttributes(attributes);
  generator.setThreadCount(1);
  generator.generate(msdfGlyphs.data(), static_cast<int>(msdfGlyphs.size()));

  std::unordered_map<u32, FontGlyph> glyphs;

  auto fWidth = static_cast<f32>(width);
  auto fHeight = static_cast<f32>(height);

  for (auto &msdfGlyph : msdfGlyphs) {
    FontGlyph glyph{};

    const auto &rect = msdfGlyph.getBoxRect();

    {
      f64 top = 0.0, left = 0.0, bottom = 0.0, right = 0.0;
      msdfGlyph.getQuadAtlasBounds(left, bottom, right, top);

      glyph.atlasBounds =
          glm::vec4(static_cast<f32>(left), static_cast<f32>(fHeight - bottom),
                    static_cast<f32>(right), static_cast<f32>(fHeight - top)) /
          fWidth;
    }

    {
      f64 top = 0.0, left = 0.0, bottom = 0.0, right = 0.0;
      msdfGlyph.getQuadPlaneBounds(left, top, right, bottom);

      glyph.planeBounds =
          glm::vec4(static_cast<f32>(left), static_cast<f32>(top),
                    static_cast<f32>(right), static_cast<f32>(bottom));
    }

    glyph.advanceX = static_cast<f32>(msdfGlyph.getAdvance());

    glyphs.insert_or_assign(msdfGlyph.getCodepoint(), glyph);
  }

  auto storage = generator.atlasStorage();

  msdfgen::BitmapConstRef<byte, NumChannels> bitmap = storage;

  std::vector<std::byte> pixels(static_cast<usize>(bitmap.width) *
                                bitmap.height * NumChannels);

  for (int y = 0; y < bitmap.height; ++y) {
    memcpy(&pixels[NumChannels * static_cast<usize>(bitmap.width) * y],
           bitmap(0, bitmap.height - y - 1),
           NumChannels * static_cast<usize>(bitmap.width));
  }

  AssetData<FontAsset> fontAsset{};
  fontAsset.path = path;
  fontAsset.type = AssetType::Font;
  fontAsset.data.size =
      sizeof(std::byte) * bitmap.width * bitmap.height * NumChannels;
  fontAsset.data.glyphs = glyphs;
  fontAsset.data.atlasBytes = pixels;
  fontAsset.data.atlasDimensions = glm::uvec2{bitmap.width, bitmap.height};
  fontAsset.data.fontScale = static_cast<f32>(FontScale);

  msdfgen::destroyFont(font);
  msdfgen::deinitializeFreetype(ft);

  return Result<AssetData<FontAsset>>::Ok(fontAsset);
}

} // namespace quoll
