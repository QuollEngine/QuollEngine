#include "quoll/core/Base.h"
#include "quoll/core/Id.h"
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
#include "quoll/scene/SpriteSerializer.h"
#include "quoll/scene/TransformSerializer.h"
#include "quoll/skeleton/JointAttachmentSerializer.h"
#include "quoll/skeleton/SkeletonSerializer.h"
#include "quoll/text/TextSerializer.h"
#include "quoll/ui/UICanvasSerializer.h"
#include "EntitySerializer.h"

namespace quoll::detail {

EntitySerializer::EntitySerializer(AssetRegistry &assetRegistry,
                                   EntityDatabase &entityDatabase)
    : mAssetRegistry(assetRegistry), mEntityDatabase(entityDatabase) {}

Result<YAML::Node> EntitySerializer::serialize(Entity entity) {
  if (!mEntityDatabase.has<Id>(entity)) {
    return Error("Entity does not have an ID");
  }

  auto node = createComponentsNode(entity);

  return node;
}

YAML::Node EntitySerializer::createComponentsNode(Entity entity) {
  YAML::Node components;

  if (mEntityDatabase.has<Id>(entity)) {
    components["id"] = mEntityDatabase.get<Id>(entity).id;
  }

  NameSerializer::serialize(components, mEntityDatabase, entity);
  TransformSerializer::serialize(components, mEntityDatabase, entity);
  SpriteSerializer::serialize(components, mEntityDatabase, entity);
  MeshSerializer::serialize(components, mEntityDatabase, entity);
  LightSerializer::serialize(components, mEntityDatabase, entity);
  CameraSerializer::serialize(components, mEntityDatabase, entity);
  SkeletonSerializer::serialize(components, mEntityDatabase, entity);
  EnvironmentLightingSerializer::serialize(components, mEntityDatabase, entity);
  EnvironmentSkyboxSerializer::serialize(components, mEntityDatabase, entity);

  JointAttachmentSerializer::serialize(components, mEntityDatabase, entity);

  AnimatorSerializer::serialize(components, mEntityDatabase, entity);

  RigidBodySerializer::serialize(components, mEntityDatabase, entity);
  CollidableSerializer::serialize(components, mEntityDatabase, entity);
  MeshRendererSerializer::serialize(components, mEntityDatabase, entity);
  SkinnedMeshRendererSerializer::serialize(components, mEntityDatabase, entity);

  AudioSerializer::serialize(components, mEntityDatabase, entity);
  ScriptSerializer::serialize(components, mEntityDatabase, entity);
  TextSerializer::serialize(components, mEntityDatabase, entity);
  InputMapSerializer::serialize(components, mEntityDatabase, entity);
  UICanvasSerializer::serialize(components, mEntityDatabase, entity);

  return components;
}

} // namespace quoll::detail
