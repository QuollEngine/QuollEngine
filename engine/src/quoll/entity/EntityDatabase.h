#pragma once

#include "Entity.h"
#include "EntityStorageSparseSet.h"

#include "quoll/core/Name.h"
#include "quoll/core/Id.h"
#include "quoll/core/Delete.h"
#include "quoll/scene/Mesh.h"
#include "quoll/scene/SkinnedMesh.h"
#include "quoll/scene/DirectionalLight.h"
#include "quoll/scene/PointLight.h"
#include "quoll/scene/CascadedShadowMap.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/scene/AutoAspectRatio.h"
#include "quoll/scene/Skeleton.h"
#include "quoll/scene/JointAttachment.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/WorldTransform.h"
#include "quoll/scene/EnvironmentSkybox.h"
#include "quoll/scene/EnvironmentLighting.h"
#include "quoll/scene/Sprite.h"
#include "quoll/animation/Animator.h"
#include "quoll/animation/AnimatorEvent.h"
#include "quoll/audio/AudioSource.h"
#include "quoll/audio/AudioStart.h"
#include "quoll/audio/AudioStatus.h"
#include "quoll/physics/RigidBody.h"
#include "quoll/physics/Collidable.h"
#include "quoll/physics/Force.h"
#include "quoll/physics/Torque.h"
#include "quoll/physics/RigidBodyClear.h"
#include "quoll/physics/PhysxInstance.h"
#include "quoll/scripting/Script.h"
#include "quoll/text/Text.h"
#include "quoll/renderer/MeshRenderer.h"
#include "quoll/renderer/SkinnedMeshRenderer.h"

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
