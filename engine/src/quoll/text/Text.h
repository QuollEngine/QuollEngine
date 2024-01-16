#pragma once

#include "quoll/asset/Asset.h"

namespace quoll {

struct Text {
  String content;

  f32 lineHeight = 1.0f;

  FontAssetHandle font = FontAssetHandle::Null;
};

} // namespace quoll
