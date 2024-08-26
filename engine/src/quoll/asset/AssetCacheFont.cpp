#include "quoll/core/Base.h"
#include "quoll/text/FontAsset.h"
#include "quoll/text/MsdfAtlas.h"
#include "quoll/text/MsdfLoader.h"
#include "AssetCache.h"
#include <freetype/freetype.h>
#include <ft2build.h>

namespace quoll {

Result<Path> AssetCache::createFontFromSource(const Path &sourcePath,
                                              const Uuid &uuid) {
  if (uuid.isEmpty()) {
    QuollAssert(false, "Invalid uuid provided");
    return Result<Path>::Error("Invalid uuid provided");
  }

  using co = std::filesystem::copy_options;

  auto assetPath = getPathFromUuid(uuid);

  if (!std::filesystem::copy_file(sourcePath, assetPath,
                                  co::overwrite_existing)) {
    return Result<Path>::Error("Cannot create font from source: " +
                               sourcePath.stem().string());
  }

  auto metaRes = createAssetMeta(AssetType::Font,
                                 sourcePath.filename().string(), assetPath);

  if (!metaRes.hasData()) {
    std::filesystem::remove(assetPath);
    return Result<Path>::Error("Cannot create font from source: " +
                               sourcePath.stem().string());
  }

  return Result<Path>::Ok(assetPath);
}

Result<AssetHandle<FontAsset>> AssetCache::loadFont(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  MsdfLoader loader;

  auto res = loader.loadFontData(filePath);

  if (res.hasError()) {
    return Result<AssetHandle<FontAsset>>::Error(res.getError());
  }

  auto meta = getAssetMeta(uuid);

  auto &data = res.getData();
  data.type = AssetType::Font;
  data.name = meta.name;
  data.uuid = Uuid(filePath.stem().string());

  auto handle = mRegistry.add(data);

  return Result<AssetHandle<FontAsset>>::Ok(handle);
}

} // namespace quoll
