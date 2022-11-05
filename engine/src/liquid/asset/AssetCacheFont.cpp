#include "liquid/core/Base.h"
#include "AssetCache.h"

#include <ft2build.h>
#include <freetype/freetype.h>

#include "liquid/text/MsdfAtlas.h"
#include "liquid/text/MsdfLoader.h"

#include "FontAsset.h"

namespace liquid {

Result<FontAssetHandle> AssetCache::loadFontFromFile(const Path &filePath) {
  MsdfLoader loader;

  auto res = loader.loadFontData(filePath);

  if (res.hasError()) {
    return Result<FontAssetHandle>::Error(res.getError());
  }

  auto &data = res.getData();
  data.relativePath = std::filesystem::relative(filePath, mAssetsPath);

  auto handle = mRegistry.getFonts().addAsset(data);

  return Result<FontAssetHandle>::Ok(handle);
}

} // namespace liquid
