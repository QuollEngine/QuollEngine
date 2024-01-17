#include "quoll/core/Base.h"
#include "quoll/core/Id.h"
#include "quoll/core/Name.h"
#include "quoll/core/NameSerializer.h"
#include "quoll/animation/AnimatorSerializer.h"
#include "quoll/audio/AudioSerializer.h"
#include "quoll/input/InputMapSerializer.h"
#include "quoll/lua-scripting/ScriptSerializer.h"
#include "quoll/physics/CollidableSerializer.h"
#include "quoll/physics/RigidBodySerializer.h"
#include "quoll/renderer/MeshRendererSerializer.h"
#include "quoll/renderer/MeshSerializer.h"
#include "quoll/renderer/SkinnedMeshRendererSerializer.h"
#include "quoll/scene/CameraSerializer.h"
#include "quoll/scene/EnvironmentLightingSerializer.h"
#include "quoll/scene/EnvironmentSkyboxSerializer.h"
#include "quoll/scene/LightSerializer.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/scene/SpriteSerializer.h"
#include "quoll/scene/TransformSerializer.h"
#include "quoll/skeleton/JointAttachmentSerializer.h"
#include "quoll/skeleton/SkeletonSerializer.h"
#include "quoll/text/TextSerializer.h"
#include "quoll/ui/UICanvasSerializer.h"
#include "SceneLoader.h"

namespace quoll::detail {

SceneLoader::SceneLoader(AssetRegistry &assetRegistry,
                         EntityDatabase &entityDatabase)
    : mAssetRegistry(assetRegistry), mEntityDatabase(entityDatabase) {}

Result<bool> SceneLoader::loadComponents(const YAML::Node &node, Entity entity,
                                         EntityIdCache &entityIdCache) {

  NameSerializer::deserialize(node, mEntityDatabase, entity, entityIdCache);
  TransformSerializer::deserialize(node, mEntityDatabase, entity,
                                   entityIdCache);
  SpriteSerializer::deserialize(node, mEntityDatabase, entity, mAssetRegistry);
  MeshSerializer::deserialize(node, mEntityDatabase, entity, mAssetRegistry);
  LightSerializer::deserialize(node, mEntityDatabase, entity);
  CameraSerializer::deserialize(node, mEntityDatabase, entity);
  SkeletonSerializer::deserialize(node, mEntityDatabase, entity,
                                  mAssetRegistry);
  EnvironmentLightingSerializer::deserialize(node, mEntityDatabase, entity);
  EnvironmentSkyboxSerializer::deserialize(node, mEntityDatabase, entity,
                                           mAssetRegistry);

  JointAttachmentSerializer::deserialize(node, mEntityDatabase, entity);

  AnimatorSerializer::deserialize(node, mEntityDatabase, entity,
                                  mAssetRegistry);

  RigidBodySerializer::deserialize(node, mEntityDatabase, entity);
  CollidableSerializer::deserialize(node, mEntityDatabase, entity);
  MeshRendererSerializer::deserialize(node, mEntityDatabase, entity,
                                      mAssetRegistry);
  SkinnedMeshRendererSerializer::deserialize(node, mEntityDatabase, entity,
                                             mAssetRegistry);

  AudioSerializer::deserialize(node, mEntityDatabase, entity, mAssetRegistry);
  ScriptSerializer::deserialize(node, mEntityDatabase, entity, mAssetRegistry);
  TextSerializer::deserialize(node, mEntityDatabase, entity, mAssetRegistry);
  InputMapSerializer::deserialize(node, mEntityDatabase, entity,
                                  mAssetRegistry);
  UICanvasSerializer::deserialize(node, mEntityDatabase, entity);

  return Result<bool>::Ok(true);
}

Result<Entity> SceneLoader::loadStartingCamera(const YAML::Node &node,
                                               EntityIdCache &entityIdCache) {
  Entity entity = Entity::Null;
  if (node && node.IsScalar()) {
    auto entityId = node.as<u64>(0);

    if (entityId > 0 && entityIdCache.find(entityId) != entityIdCache.end()) {
      auto foundEntity = entityIdCache.at(entityId);

      if (mEntityDatabase.has<PerspectiveLens>(foundEntity)) {
        entity = foundEntity;
      }
    }
  }

  if (entity == Entity::Null) {
    return Result<Entity>::Error("Camera entity not found");
  }

  return Result<Entity>::Ok(entity);
}

Result<Entity> SceneLoader::loadEnvironment(const YAML::Node &node,
                                            EntityIdCache &entityIdCache) {
  if (node && node.IsScalar()) {
    auto entityId = node.as<u64>(0);

    if (entityId > 0 && entityIdCache.contains(entityId)) {
      auto entity = entityIdCache.at(entityId);
      return Result<Entity>::Ok(entity);
    }
  }

  return Result<Entity>::Error("Environment entity not found");
}

} // namespace quoll::detail
