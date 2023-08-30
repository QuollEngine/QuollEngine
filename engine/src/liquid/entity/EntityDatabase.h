#pragma once

#include "Entity.h"
#include "EntityStorageSparseSet.h"

#include "liquid/core/Name.h"
#include "liquid/core/Id.h"
#include "liquid/core/Delete.h"
#include "liquid/scene/Mesh.h"
#include "liquid/scene/SkinnedMesh.h"
#include "liquid/scene/DirectionalLight.h"
#include "liquid/scene/PointLight.h"
#include "liquid/scene/CascadedShadowMap.h"
#include "liquid/scene/PerspectiveLens.h"
#include "liquid/scene/AutoAspectRatio.h"
#include "liquid/scene/Skeleton.h"
#include "liquid/scene/JointAttachment.h"
#include "liquid/scene/Camera.h"
#include "liquid/scene/Parent.h"
#include "liquid/scene/Children.h"
#include "liquid/scene/LocalTransform.h"
#include "liquid/scene/WorldTransform.h"
#include "liquid/scene/EnvironmentSkybox.h"
#include "liquid/scene/EnvironmentLighting.h"
#include "liquid/scene/Sprite.h"
#include "liquid/animation/Animator.h"
#include "liquid/animation/AnimatorEvent.h"
#include "liquid/audio/AudioSource.h"
#include "liquid/audio/AudioStart.h"
#include "liquid/audio/AudioStatus.h"
#include "liquid/physics/RigidBody.h"
#include "liquid/physics/Collidable.h"
#include "liquid/physics/Force.h"
#include "liquid/physics/Torque.h"
#include "liquid/physics/RigidBodyClear.h"
#include "liquid/physics/PhysxInstance.h"
#include "liquid/scripting/Script.h"
#include "liquid/text/Text.h"
#include "liquid/renderer/MeshRenderer.h"
#include "liquid/renderer/SkinnedMeshRenderer.h"

namespace quoll {

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

template <class TComponent>
using EntityDatabaseObserver = EntityStorageSparseSetObserver<TComponent>;

} // namespace quoll
