#pragma once

#include "quoll/entity/Entity.h"

namespace quoll::editor {

/**
 * @brief Asset syncer
 *
 * Syncs local representation of asset
 * with the file. The local representation
 * is always a scene based on entities
 */
class AssetSyncer {
public:
  /**
   * @brief Sync entities with the asset
   *
   * @param entities Entities
   */
  virtual void syncEntities(const std::vector<Entity> &entities) = 0;

  /**
   * @brief Delete entities from the asset
   *
   * @param entities Entities
   */
  virtual void deleteEntities(const std::vector<Entity> &entities) = 0;

  /**
   * @brief Sync scene with the asset
   */
  virtual void syncScene() = 0;
};

} // namespace quoll::editor
