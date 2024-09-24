#pragma once

#include "quoll/core/Result.h"
#include "quoll/asset/AssetData.h"
#include "FontAsset.h"

namespace quoll {

class MsdfLoader {
public:
  Result<AssetData<FontAsset>> loadFontData(const Path &path);
};

} // namespace quoll
