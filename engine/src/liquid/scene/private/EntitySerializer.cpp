#include "liquid/core/Base.h"
#include "EntitySerializer.h"

namespace liquid::detail {

EntitySerializer::EntitySerializer(AssetRegistry &assetRegistry,
                                   EntityDatabase &entityDatabase)
    : mAssetRegistry(assetRegistry), mEntityDatabase(entityDatabase) {}

Result<YAML::Node> EntitySerializer::serialize(Entity entity) {
  YAML::Node node;

  if (!mEntityDatabase.has<Id>(entity)) {
    return Result<YAML::Node>::Error("Entity does not have an ID");
  }

  node["id"] = mEntityDatabase.get<Id>(entity).id;
  node["version"] = "0.1";
  node["components"] = createComponentsNode(entity);

  return Result<YAML::Node>::Ok(node);
}

YAML::Node EntitySerializer::createComponentsNode(Entity entity) {
  YAML::Node components;

  // Set name component if name is empty
  if (!mEntityDatabase.has<Name>(entity) ||
      mEntityDatabase.get<Name>(entity).name.empty()) {

    if (mEntityDatabase.has<Id>(entity)) {
      auto id = mEntityDatabase.get<Id>(entity).id;
      mEntityDatabase.set<Name>(entity, {"Untitled " + std::to_string(id)});
    } else {
      mEntityDatabase.set<Name>(entity, {"Untitled"});
    }
  }
  components["name"] = mEntityDatabase.get<Name>(entity).name;

  if (!mEntityDatabase.has<LocalTransform>(entity)) {
    mEntityDatabase.set<LocalTransform>(entity, {});
  }
  const auto &component = mEntityDatabase.get<LocalTransform>(entity);

  components["transform"]["position"] = component.localPosition;
  components["transform"]["rotation"] = component.localRotation;
  components["transform"]["scale"] = component.localScale;

  if (mEntityDatabase.has<Parent>(entity)) {
    auto parent = mEntityDatabase.get<Parent>(entity).parent;

    if (mEntityDatabase.exists(parent) && mEntityDatabase.has<Id>(parent)) {
      components["transform"]["parent"] = mEntityDatabase.get<Id>(parent).id;
    }
  }

  if (mEntityDatabase.has<DirectionalLight>(entity)) {
    const auto &light = mEntityDatabase.get<DirectionalLight>(entity);

    components["light"]["type"] = 0;
    components["light"]["color"] = light.color;
    components["light"]["intensity"] = light.intensity;

    if (mEntityDatabase.has<CascadedShadowMap>(entity)) {
      const auto &shadow = mEntityDatabase.get<CascadedShadowMap>(entity);
      components["light"]["shadow"]["softShadows"] = shadow.softShadows;
      components["light"]["shadow"]["splitLambda"] = shadow.splitLambda;
      components["light"]["shadow"]["numCascades"] = shadow.numCascades;
    }
  } else if (mEntityDatabase.has<PointLight>(entity)) {
    const auto &light = mEntityDatabase.get<PointLight>(entity);

    components["light"]["type"] = 1;
    components["light"]["color"] = light.color;
    components["light"]["intensity"] = light.intensity;
    components["light"]["range"] = light.range;
  }

  if (mEntityDatabase.has<PerspectiveLens>(entity)) {
    const auto &camera = mEntityDatabase.get<PerspectiveLens>(entity);

    components["camera"]["type"] = 0;
    components["camera"]["near"] = camera.near;
    components["camera"]["far"] = camera.far;
    components["camera"]["aperture"] = camera.aperture;
    components["camera"]["sensorSize"] = camera.sensorSize;
    components["camera"]["focalLength"] = camera.focalLength;
    components["camera"]["shutterSpeed"] = camera.shutterSpeed;
    components["camera"]["sensitivity"] = camera.sensitivity;

    if (mEntityDatabase.has<AutoAspectRatio>(entity)) {
      components["camera"]["aspectRatio"] = "auto";
    } else {
      components["camera"]["aspectRatio"] = camera.aspectRatio;
    }
  }

  if (mEntityDatabase.has<Text>(entity)) {
    const auto &text = mEntityDatabase.get<Text>(entity);

    if (!text.text.empty() && mAssetRegistry.getFonts().hasAsset(text.font)) {
      auto path =
          mAssetRegistry.getFonts().getAsset(text.font).relativePath.string();
      std::replace(path.begin(), path.end(), '\\', '/');

      components["text"]["content"] = text.text;
      components["text"]["lineHeight"] = text.lineHeight;
      components["text"]["font"] = path;
    }
  }

  if (mEntityDatabase.has<RigidBody>(entity)) {
    const auto &rigidBodyDesc =
        mEntityDatabase.get<RigidBody>(entity).dynamicDesc;

    components["rigidBody"]["applyGravity"] = rigidBodyDesc.applyGravity;
    components["rigidBody"]["inertia"] = rigidBodyDesc.inertia;
    components["rigidBody"]["mass"] = rigidBodyDesc.mass;
  }

  if (mEntityDatabase.has<Collidable>(entity)) {
    const auto &component = mEntityDatabase.get<Collidable>(entity);

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

  if (mEntityDatabase.has<Sprite>(entity)) {
    auto handle = mEntityDatabase.get<Sprite>(entity).handle;
    if (mAssetRegistry.getTextures().hasAsset(handle)) {
      auto path =
          mAssetRegistry.getTextures().getAsset(handle).relativePath.string();
      std::replace(path.begin(), path.end(), '\\', '/');

      components["sprite"] = path;
    }
  }

  if (mEntityDatabase.has<Mesh>(entity)) {
    auto handle = mEntityDatabase.get<Mesh>(entity).handle;
    if (mAssetRegistry.getMeshes().hasAsset(handle)) {
      auto path =
          mAssetRegistry.getMeshes().getAsset(handle).relativePath.string();
      std::replace(path.begin(), path.end(), '\\', '/');

      components["mesh"] = path;
    }
  }

  if (mEntityDatabase.has<SkinnedMesh>(entity)) {
    auto handle = mEntityDatabase.get<SkinnedMesh>(entity).handle;
    if (mAssetRegistry.getSkinnedMeshes().hasAsset(handle)) {
      auto path = mAssetRegistry.getSkinnedMeshes()
                      .getAsset(handle)
                      .relativePath.string();
      std::replace(path.begin(), path.end(), '\\', '/');

      components["skinnedMesh"] = path;
    }
  }

  if (mEntityDatabase.has<Skeleton>(entity)) {
    auto handle = mEntityDatabase.get<Skeleton>(entity).assetHandle;
    if (mAssetRegistry.getSkeletons().hasAsset(handle)) {
      auto path =
          mAssetRegistry.getSkeletons().getAsset(handle).relativePath.string();
      std::replace(path.begin(), path.end(), '\\', '/');

      components["skeleton"] = path;
    }
  }

  if (mEntityDatabase.has<Animator>(entity)) {
    auto handle = mEntityDatabase.get<Animator>(entity).asset;

    if (mAssetRegistry.getAnimators().hasAsset(handle)) {
      auto path =
          mAssetRegistry.getAnimators().getAsset(handle).relativePath.string();
      std::replace(path.begin(), path.end(), '\\', '/');

      components["animator"]["asset"] = path;
    }
  }

  if (mEntityDatabase.has<Script>(entity)) {
    const auto &script = mEntityDatabase.get<Script>(entity);
    if (mAssetRegistry.getLuaScripts().hasAsset(script.handle)) {
      const auto &asset =
          mAssetRegistry.getLuaScripts().getAsset(script.handle);

      auto path = asset.relativePath.string();
      std::replace(path.begin(), path.end(), '\\', '/');

      components["script"]["asset"] = path;

      for (auto &[name, value] : script.variables) {
        auto it = asset.data.variables.find(name);
        if (it == asset.data.variables.end() ||
            !value.isType(it->second.type)) {
          continue;
        }

        if (value.isType(LuaScriptVariableType::String)) {
          components["script"]["variables"][name]["type"] = "string";
          components["script"]["variables"][name]["value"] =
              value.get<String>();
        } else if (value.isType(LuaScriptVariableType::AssetPrefab)) {
          auto handle = value.get<PrefabAssetHandle>();
          if (mAssetRegistry.getPrefabs().hasAsset(handle)) {
            auto path = mAssetRegistry.getPrefabs()
                            .getAsset(handle)
                            .relativePath.string();
            std::replace(path.begin(), path.end(), '\\', '/');

            components["script"]["variables"][name]["type"] = "prefab";
            components["script"]["variables"][name]["value"] = path;
          }
        }
      }
    }
  }

  if (mEntityDatabase.has<AudioSource>(entity)) {
    auto handle = mEntityDatabase.get<AudioSource>(entity).source;
    if (mAssetRegistry.getAudios().hasAsset(handle)) {
      auto path =
          mAssetRegistry.getAudios().getAsset(handle).relativePath.string();
      std::replace(path.begin(), path.end(), '\\', '/');

      components["audio"]["source"] = path;
    }
  }

  return components;
}

} // namespace liquid::detail
