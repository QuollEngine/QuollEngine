#pragma once

#include "liquid/scene/Scene.h"

namespace liquid {

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

} // namespace liquid
