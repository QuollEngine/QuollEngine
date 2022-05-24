#pragma once

#include "Entity.h"

#include "liquid/core/NameComponent.h"
#include "liquid/core/IdComponent.h"
#include "liquid/scene/MeshComponent.h"
#include "liquid/scene/SkinnedMeshComponent.h"
#include "liquid/scene/DirectionalLightComponent.h"
#include "liquid/scene/PerspectiveLensComponent.h"
#include "liquid/scene/AutoAspectRatioComponent.h"
#include "liquid/scene/SkeletonComponent.h"
#include "liquid/scene/CameraComponent.h"
#include "liquid/scene/ParentComponent.h"
#include "liquid/scene/ChildrenComponent.h"
#include "liquid/scene/LocalTransformComponent.h"
#include "liquid/scene/WorldTransformComponent.h"
#include "liquid/scene/EnvironmentComponent.h"
#include "liquid/animation/AnimatorComponent.h"
#include "liquid/physics/RigidBodyComponent.h"
#include "liquid/physics/CollidableComponent.h"
#include "liquid/scripting/ScriptingComponent.h"

#include "EntityStorageSparseSet.h"

namespace liquid {

using EntityContext = EntityStorageSparseSet<
    IdComponent, MeshComponent, DirectionalLightComponent, CameraComponent,
    AutoAspectRatioComponent, PerspectiveLensComponent, LocalTransformComponent,
    WorldTransformComponent, ParentComponent, ChildrenComponent,
    EnvironmentComponent, AnimatorComponent, NameComponent,
    SkinnedMeshComponent, SkeletonComponent, SkeletonDebugComponent,
    RigidBodyComponent, CollidableComponent, ScriptingComponent>;

} // namespace liquid
