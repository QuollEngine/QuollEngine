#pragma once

#include "quoll/asset/AssetRef.h"
#include "FontAsset.h"

namespace quoll {

struct Text {
  String content;

  f32 lineHeight = 1.0f;

  AssetRef<FontAsset> font;
};

} // namespace quoll
