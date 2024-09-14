#pragma once

#include "quoll/entity/Entity.h"

namespace quoll::editor {

/**
 * Asset syncer
 *
 * Syncs local representation of asset
 * with the file. The local representation
 * is always a scene based on entities
 */
class AssetSyncer {
public:
  virtual void syncEntities(const std::vector<Entity> &entities) = 0;

  virtual void deleteEntities(const std::vector<Entity> &entities) = 0;

  virtual void syncScene() = 0;
};

} // namespace quoll::editor
