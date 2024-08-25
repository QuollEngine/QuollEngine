#pragma once

#include "quoll/rhi/RenderHandle.h"
#include "AssetType.h"

namespace quoll {

template <class TData> struct AssetData {
  AssetType type = AssetType::None;

  TData data;

  Path path;

  Uuid uuid;

  String name;

  rhi::TextureHandle preview = rhi::TextureHandle::Null;
};

} // namespace quoll
