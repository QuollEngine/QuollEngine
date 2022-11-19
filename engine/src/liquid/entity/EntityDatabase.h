#pragma once

#include "Entity.h"
#include "EntityStorageSparseSet.h"

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
#include "liquid/physics/ForceComponent.h"
#include "liquid/physics/TorqueComponent.h"
#include "liquid/physics/RigidBodyClearComponent.h"
#include "liquid/scripting/ScriptingComponent.h"
#include "liquid/text/TextComponent.h"

namespace liquid {

/**
 * @brief Entity database
 *
 * Default entity database used within the engine
 *
 * Automatically registers engine components but allows
 * register other components for specific purposes.
 */
class EntityDatabase : public EntityStorageSparseSet {
public:
  /**
   * @brief Create entity database
   */
  EntityDatabase();
};

} // namespace liquid
