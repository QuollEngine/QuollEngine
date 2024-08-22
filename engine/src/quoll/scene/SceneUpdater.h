#pragma once

#include "quoll/entity/Entity.h"

namespace quoll {

struct SystemView;

class SceneUpdater {
public:
  void update(SystemView &view);

private:
  void updateTransforms(SystemView &view);

  void updateCameras(SystemView &view);

  void updateLights(SystemView &view);
};

} // namespace quoll
