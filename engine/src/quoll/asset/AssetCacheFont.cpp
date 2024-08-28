#include "quoll/core/Base.h"
#include "quoll/text/FontAsset.h"
#include "quoll/text/MsdfAtlas.h"
#include "quoll/text/MsdfLoader.h"
#include "AssetCache.h"
#include <freetype/freetype.h>
#include <ft2build.h>

namespace quoll {

Result<FontAsset> AssetCache::loadFont(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  MsdfLoader loader;

  auto res = loader.loadFontData(filePath);

  if (!res) {
    return res.error();
  }

  return res.data().data;
}

} // namespace quoll
