#pragma once

#include "quoll/asset/AssetHandle.h"
#include "FontAsset.h"

namespace quoll {

struct Text {
  String content;

  f32 lineHeight = 1.0f;

  AssetHandle<FontAsset> font;
};

} // namespace quoll
