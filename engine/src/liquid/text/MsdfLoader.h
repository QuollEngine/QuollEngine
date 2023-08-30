#pragma once

#include "liquid/asset/FontAsset.h"
#include "liquid/asset/Result.h"
#include "liquid/asset/AssetData.h"

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
