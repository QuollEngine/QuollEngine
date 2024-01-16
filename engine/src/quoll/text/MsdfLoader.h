#pragma once

#include "quoll/asset/FontAsset.h"
#include "quoll/asset/Result.h"
#include "quoll/asset/AssetData.h"

namespace quoll {

class MsdfLoader {
public:
  Result<AssetData<FontAsset>> loadFontData(const Path &path);
};

} // namespace quoll
