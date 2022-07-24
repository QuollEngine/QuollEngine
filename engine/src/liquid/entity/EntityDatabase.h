#pragma once

#include "Entity.h"

#include "liquid/core/NameComponent.h"
#include "liquid/core/IdComponent.h"
#include "liquid/core/DeleteComponent.h"
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
#include "liquid/audio/AudioSourceComponent.h"
#include "liquid/audio/AudioStartComponent.h"
#include "liquid/audio/AudioStatusComponent.h"
#include "liquid/physics/RigidBodyComponent.h"
#include "liquid/physics/CollidableComponent.h"
#include "liquid/scripting/ScriptingComponent.h"
#include "liquid/text/TextComponent.h"

#include "EntityStorageSparseSet.h"

namespace liquid {

// clang-format off
using EntityDatabase = EntityStorageSparseSet<
    IdComponent,
    DeleteComponent,
    MeshComponent,
    DirectionalLightComponent,
    CameraComponent,
    AutoAspectRatioComponent,
    PerspectiveLensComponent,
    LocalTransformComponent,
    WorldTransformComponent,
    ParentComponent,
    ChildrenComponent,
    EnvironmentComponent,
    AnimatorComponent,
    AudioSourceComponent,
    AudioStartComponent,
    AudioStatusComponent,
    NameComponent,
    SkinnedMeshComponent,
    SkeletonComponent,
    SkeletonDebugComponent,
    RigidBodyComponent,
    CollidableComponent,
    ScriptingComponent,
    TextComponent
>;
// clang-format on

} // namespace liquid
