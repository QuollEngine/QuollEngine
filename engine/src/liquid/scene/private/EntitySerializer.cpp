#include "liquid/core/Base.h"
#include "EntitySerializer.h"

namespace liquid::detail {

EntitySerializer::EntitySerializer(AssetRegistry &assetRegistry,
                                   EntityDatabase &entityDatabase)
    : mAssetRegistry(assetRegistry), mEntityDatabase(entityDatabase) {}

Result<YAML::Node> EntitySerializer::serialize(Entity entity) {
  YAML::Node node;

  if (!mEntityDatabase.hasComponent<IdComponent>(entity)) {
    return Result<YAML::Node>::Error("Entity does not have an ID");
  }

  node["id"] = mEntityDatabase.getComponent<IdComponent>(entity).id;
  node["version"] = "0.1";
  node["components"] = createComponentsNode(entity);

  return Result<YAML::Node>::Ok(node);
}

YAML::Node EntitySerializer::createComponentsNode(Entity entity) {
  YAML::Node components;

  // Set name component if name is empty
  if (!mEntityDatabase.hasComponent<NameComponent>(entity) ||
      mEntityDatabase.getComponent<NameComponent>(entity).name.empty()) {

    if (mEntityDatabase.hasComponent<IdComponent>(entity)) {
      auto id = mEntityDatabase.getComponent<IdComponent>(entity).id;
      mEntityDatabase.setComponent<NameComponent>(
          entity, {"Untitled " + std::to_string(id)});
    } else {
      mEntityDatabase.setComponent<NameComponent>(entity, {"Untitled"});
    }
  }
  components["name"] = mEntityDatabase.getComponent<NameComponent>(entity).name;

  if (!mEntityDatabase.hasComponent<LocalTransformComponent>(entity)) {
    mEntityDatabase.setComponent<LocalTransformComponent>(entity, {});
  }
  const auto &component =
      mEntityDatabase.getComponent<LocalTransformComponent>(entity);

  components["transform"]["position"] = component.localPosition;
  components["transform"]["rotation"] = component.localRotation;
  components["transform"]["scale"] = component.localScale;

  if (mEntityDatabase.hasComponent<ParentComponent>(entity)) {
    auto parent = mEntityDatabase.getComponent<ParentComponent>(entity).parent;

    if (mEntityDatabase.hasEntity(parent) &&
        mEntityDatabase.hasComponent<IdComponent>(parent)) {
      components["transform"]["parent"] =
          mEntityDatabase.getComponent<IdComponent>(parent).id;
    }
  }

  if (mEntityDatabase.hasComponent<DirectionalLightComponent>(entity)) {
    const auto &light =
        mEntityDatabase.getComponent<DirectionalLightComponent>(entity);

    components["light"]["type"] = 0;
    components["light"]["color"] = light.color;
    components["light"]["intensity"] = light.intensity;
  }

  if (mEntityDatabase.hasComponent<PerspectiveLensComponent>(entity)) {
    const auto &camera =
        mEntityDatabase.getComponent<PerspectiveLensComponent>(entity);

    components["camera"]["type"] = 0;
    components["camera"]["fov"] = camera.fovY;
    components["camera"]["near"] = camera.near;
    components["camera"]["far"] = camera.far;

    if (mEntityDatabase.hasComponent<AutoAspectRatioComponent>(entity)) {
      components["camera"]["aspectRatio"] = "auto";
    } else {
      components["camera"]["aspectRatio"] = camera.aspectRatio;
    }
  }

  if (mEntityDatabase.hasComponent<TextComponent>(entity)) {
    const auto &text = mEntityDatabase.getComponent<TextComponent>(entity);

    if (!text.text.empty() && mAssetRegistry.getFonts().hasAsset(text.font)) {
      components["text"]["content"] = text.text;
      components["text"]["lineHeight"] = text.lineHeight;
      components["text"]["font"] =
          mAssetRegistry.getFonts().getAsset(text.font).relativePath.string();
    }
  }

  if (mEntityDatabase.hasComponent<RigidBodyComponent>(entity)) {
    const auto &rigidBodyDesc =
        mEntityDatabase.getComponent<RigidBodyComponent>(entity).dynamicDesc;

    components["rigidBody"]["applyGravity"] = rigidBodyDesc.applyGravity;
    components["rigidBody"]["inertia"] = rigidBodyDesc.inertia;
    components["rigidBody"]["mass"] = rigidBodyDesc.mass;
  }

  if (mEntityDatabase.hasComponent<CollidableComponent>(entity)) {
    const auto &component =
        mEntityDatabase.getComponent<CollidableComponent>(entity);

    auto type = component.geometryDesc.type;

    components["collidable"]["shape"] = getPhysicsGeometryTypeString(type);

    if (type == PhysicsGeometryType::Box) {
      components["collidable"]["halfExtents"] =
          std::get<PhysicsGeometryBox>(component.geometryDesc.params)
              .halfExtents;
    } else if (type == PhysicsGeometryType::Sphere) {
      components["collidable"]["radius"] =
          std::get<PhysicsGeometrySphere>(component.geometryDesc.params).radius;
    } else if (type == PhysicsGeometryType::Capsule) {
      const auto &capsule =
          std::get<PhysicsGeometryCapsule>(component.geometryDesc.params);
      components["collidable"]["radius"] = capsule.radius;
      components["collidable"]["halfHeight"] = capsule.halfHeight;
    }

    components["collidable"]["dynamicFriction"] =
        component.materialDesc.dynamicFriction;
    components["collidable"]["restitution"] =
        component.materialDesc.restitution;
    components["collidable"]["staticFriction"] =
        component.materialDesc.staticFriction;
  }

  if (mEntityDatabase.hasComponent<MeshComponent>(entity)) {
    auto handle = mEntityDatabase.getComponent<MeshComponent>(entity).handle;
    if (mAssetRegistry.getMeshes().hasAsset(handle)) {
      components["mesh"] =
          mAssetRegistry.getMeshes().getAsset(handle).relativePath.string();
    }
  }

  if (mEntityDatabase.hasComponent<SkinnedMeshComponent>(entity)) {
    auto handle =
        mEntityDatabase.getComponent<SkinnedMeshComponent>(entity).handle;
    if (mAssetRegistry.getSkinnedMeshes().hasAsset(handle)) {
      components["skinnedMesh"] = mAssetRegistry.getSkinnedMeshes()
                                      .getAsset(handle)
                                      .relativePath.string();
    }
  }

  if (mEntityDatabase.hasComponent<SkeletonComponent>(entity)) {
    auto handle =
        mEntityDatabase.getComponent<SkeletonComponent>(entity).assetHandle;
    if (mAssetRegistry.getSkeletons().hasAsset(handle)) {
      components["skeleton"] =
          mAssetRegistry.getSkeletons().getAsset(handle).relativePath.string();
    }
  }

  if (mEntityDatabase.hasComponent<ScriptingComponent>(entity)) {
    auto handle =
        mEntityDatabase.getComponent<ScriptingComponent>(entity).handle;
    if (mAssetRegistry.getLuaScripts().hasAsset(handle)) {
      components["script"] =
          mAssetRegistry.getLuaScripts().getAsset(handle).relativePath.string();
    }
  }

  if (mEntityDatabase.hasComponent<AudioSourceComponent>(entity)) {
    auto handle =
        mEntityDatabase.getComponent<AudioSourceComponent>(entity).source;
    if (mAssetRegistry.getAudios().hasAsset(handle)) {
      components["audio"]["source"] =
          mAssetRegistry.getAudios().getAsset(handle).relativePath.string();
    }
  }

  return components;
}

} // namespace liquid::detail
