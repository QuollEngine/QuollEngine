#pragma once

#include "Asset.h"

namespace liquid {

template <class TData> struct AssetData {
  AssetType type = AssetType::None;
  String name;
  size_t size = 0;
  TData data;
  std::filesystem::path path;
};

} // namespace liquid
