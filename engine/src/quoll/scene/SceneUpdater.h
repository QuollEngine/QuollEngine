#pragma once

#include "quoll/entity/Entity.h"

namespace quoll {

class EntityDatabase;

class SceneUpdater {
public:
  void update(EntityDatabase &entityDatabase);

private:
  void updateTransforms(EntityDatabase &entityDatabase);

  void updateCameras(EntityDatabase &entityDatabase);

  void updateLights(EntityDatabase &entityDatabase);
};

} // namespace quoll
