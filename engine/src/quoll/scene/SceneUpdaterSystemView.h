#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "LocalTransform.h"
#include "Parent.h"
#include "PerspectiveLens.h"
#include "WorldTransform.h"

namespace quoll {

struct SceneUpdaterSystemView {
  flecs::query<LocalTransform, WorldTransform> queryTransformsWithoutParent;
  flecs::query<LocalTransform, WorldTransform, Parent>
      queryTransformsWithParent;
  flecs::query<PerspectiveLens, WorldTransform, Camera> queryCameras;
  flecs::query<DirectionalLight, WorldTransform> queryDirectionalLights;
};

} // namespace quoll
