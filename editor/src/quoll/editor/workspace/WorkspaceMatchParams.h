#pragma once

#include "quoll/asset/Asset.h"

namespace quoll::editor {

struct WorkspaceMatchParams {
  String type;

  u32 asset;

  AssetType assetType;
};

} // namespace quoll::editor
