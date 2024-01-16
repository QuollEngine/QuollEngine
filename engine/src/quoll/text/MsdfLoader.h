#pragma once

#include "quoll/asset/AssetData.h"
#include "quoll/asset/FontAsset.h"
#include "quoll/asset/Result.h"

namespace quoll {

class MsdfLoader {
public:
  Result<AssetData<FontAsset>> loadFontData(const Path &path);
};

} // namespace quoll
