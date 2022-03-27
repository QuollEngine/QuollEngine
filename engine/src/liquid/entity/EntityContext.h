#pragma once

#include "Entity.h"

#include "liquid/core/NameComponent.h"
#include "liquid/scene/MeshComponent.h"
#include "liquid/scene/SkinnedMeshComponent.h"
#include "liquid/scene/LightComponent.h"
#include "liquid/scene/CameraComponent.h"
#include "liquid/scene/TransformComponent.h"
#include "liquid/scene/EnvironmentComponent.h"
#include "liquid/animation/AnimatorComponent.h"
#include "liquid/scene/SkeletonComponent.h"
#include "liquid/physics/RigidBodyComponent.h"
#include "liquid/physics/CollidableComponent.h"
#include "liquid/profiler/DebugComponent.h"
#include "liquid/scripting/ScriptingComponent.h"

#include "EntityError.h"
#include "EntityStorageSparseSet.h"

namespace liquid {

using EntityContext = EntityStorageSparseSet<
    MeshComponent, LightComponent, CameraComponent, TransformComponent,
    EnvironmentComponent, AnimatorComponent, NameComponent,
    SkinnedMeshComponent, SkeletonComponent, RigidBodyComponent,
    CollidableComponent, ScriptingComponent, DebugComponent>;

} // namespace liquid
