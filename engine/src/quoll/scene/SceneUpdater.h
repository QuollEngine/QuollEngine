#pragma once

#include "quoll/entity/Entity.h"
#include "quoll/entity/EntityDatabase.h"

namespace quoll {

class SceneUpdater {
public:
  void update(EntityDatabase &entityDatabase);

private:
  void updateTransforms(EntityDatabase &entityDatabase);

  void updateCameras(EntityDatabase &entityDatabase);

  void updateLights(EntityDatabase &entityDatabase);
};

} // namespace quoll
