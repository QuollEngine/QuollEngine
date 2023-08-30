#pragma once

#include "liquid/scene/Scene.h"

namespace quoll {

/**
 * @brief Entity deleter
 *
 * Deletes entities that are
 * flagged for deletion
 */
class EntityDeleter {
public:
  /**
   * @brief Delete entities
   *
   * @param scene Scene
   */
  void update(Scene &scene);
};

} // namespace quoll
