#pragma once

#include "liquid/core/NameComponent.h"
#include "liquid/scene/MeshComponent.h"
#include "liquid/scene/LightComponent.h"
#include "liquid/scene/CameraComponent.h"
#include "liquid/scene/TransformComponent.h"
#include "liquid/scene/EnvironmentComponent.h"
#include "liquid/animation/AnimationComponent.h"

#include "Entity.h"
#include "EntityError.h"
#include "EntityStorageSparseSet.h"

namespace liquid {

using EntityContext =
    EntityStorageSparseSet<MeshComponent, LightComponent, CameraComponent,
                           TransformComponent, EnvironmentComponent,
                           AnimationComponent, NameComponent>;

} // namespace liquid
