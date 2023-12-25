#pragma once

#include "quoll/asset/Asset.h"

namespace quoll {

/**
 * @brief Workspace match parameters
 */
struct WorkspaceMatchParams {
  /**
   * Workspace type
   */
  String type;

  /**
   * Asset handle
   */
  u32 asset;

  /**
   * Asset type
   */
  AssetType assetType;
};

} // namespace quoll
