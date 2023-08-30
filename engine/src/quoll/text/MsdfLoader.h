#pragma once

#include "quoll/asset/FontAsset.h"
#include "quoll/asset/Result.h"
#include "quoll/asset/AssetData.h"

namespace quoll {

/**
 * @brief Msdf loader
 */
class MsdfLoader {
public:
  /**
   * @brief Load msdf font data
   *
   * @param path Font path
   * @return Msdf data
   */
  Result<AssetData<FontAsset>> loadFontData(const Path &path);
};

} // namespace quoll
