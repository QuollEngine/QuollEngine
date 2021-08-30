#pragma once

#include "core/NameComponent.h"
#include "scene/MeshComponent.h"
#include "scene/LightComponent.h"
#include "scene/TransformComponent.h"
#include "scene/EnvironmentComponent.h"

#include "Entity.h"
#include "EntityError.h"
#include "EntityStorageSparseSet.h"

namespace liquid {

using EntityContext =
    EntityStorageSparseSet<MeshComponent, LightComponent, TransformComponent,
                           EnvironmentComponent, NameComponent>;

} // namespace liquid
