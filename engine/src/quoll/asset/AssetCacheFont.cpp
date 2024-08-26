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
    return Error("Invalid uuid provided");
  }

  using co = std::filesystem::copy_options;

  auto assetPath = getPathFromUuid(uuid);

  if (!std::filesystem::copy_file(sourcePath, assetPath,
                                  co::overwrite_existing)) {
    return Error("Cannot create font from source: " +
                 sourcePath.stem().string());
  }

  auto metaRes = createAssetMeta(AssetType::Font,
                                 sourcePath.filename().string(), assetPath);

  if (!metaRes) {
    std::filesystem::remove(assetPath);
    return Error("Cannot create font from source: " +
                 sourcePath.stem().string());
  }

  return assetPath;
}

Result<AssetHandle<FontAsset>> AssetCache::loadFont(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  MsdfLoader loader;

  auto res = loader.loadFontData(filePath);

  if (!res) {
    return res.error();
  }

  auto meta = getAssetMeta(uuid);

  auto &data = res.data();
  data.type = AssetType::Font;
  data.name = meta.name;
  data.uuid = Uuid(filePath.stem().string());

  auto handle = mRegistry.add(data);

  return handle;
}

} // namespace quoll
