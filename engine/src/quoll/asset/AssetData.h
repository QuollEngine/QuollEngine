#pragma once

#include "quoll/rhi/RenderHandle.h"
#include "AssetType.h"

namespace quoll {

template <class TData> struct AssetData {
  AssetType type = AssetType::None;

  TData data;

  Uuid uuid;

  String name;
};

} // namespace quoll
