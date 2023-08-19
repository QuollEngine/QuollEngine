#include "liquid/core/Base.h"
#include "SceneLoader.h"

namespace liquid::detail {

SceneLoader::SceneLoader(AssetRegistry &assetRegistry,
                         EntityDatabase &entityDatabase)
    : mAssetRegistry(assetRegistry), mEntityDatabase(entityDatabase) {}

Result<bool> SceneLoader::loadComponents(const YAML::Node &node, Entity entity,
                                         EntityIdCache &entityIdCache) {
  if (node["name"] && node["name"].IsScalar()) {
    auto name = node["name"].as<liquid::String>();
    mEntityDatabase.set<Name>(entity, {name});
  } else {
    auto name = "Untitled " + node["id"].as<String>();
    mEntityDatabase.set<Name>(entity, {name});
  }

  LocalTransform transform{};
  if (node["transform"] && node["transform"].IsMap()) {
    transform.localPosition =
        node["transform"]["position"].as<glm::vec3>(transform.localPosition);

    transform.localRotation =
        node["transform"]["rotation"].as<glm::quat>(transform.localRotation);

    transform.localScale =
        node["transform"]["scale"].as<glm::vec3>(transform.localScale);

    if (node["transform"]["parent"]) {
      auto parentId = node["transform"]["parent"].as<uint64_t>(0);

      auto it = entityIdCache.find(parentId);
      Entity parentEntity =
          it != entityIdCache.end() ? it->second : Entity::Null;

      if (parentEntity != Entity::Null) {
        mEntityDatabase.set<Parent>(entity, {parentEntity});

        if (mEntityDatabase.has<Children>(parentEntity)) {
          mEntityDatabase.get<Children>(parentEntity)
              .children.push_back(entity);
        } else {
          mEntityDatabase.set<Children>(parentEntity, {{entity}});
        }
      }
    }
  }

  mEntityDatabase.set(entity, transform);
  mEntityDatabase.set<WorldTransform>(entity, {});

  if (node["sprite"]) {
    auto uuid = node["sprite"].as<String>("");

    auto handle = mAssetRegistry.getTextures().findHandleByUuid(uuid);

    if (handle != TextureAssetHandle::Null) {
      mEntityDatabase.set<Sprite>(entity, {handle});
    }
  }

  if (node["rigidBody"] && node["rigidBody"].IsMap()) {
    RigidBody rigidBody{};
    rigidBody.dynamicDesc.mass =
        node["rigidBody"]["mass"].as<float>(rigidBody.dynamicDesc.mass);
    rigidBody.dynamicDesc.inertia = node["rigidBody"]["inertia"].as<glm::vec3>(
        rigidBody.dynamicDesc.inertia);
    rigidBody.dynamicDesc.applyGravity =
        node["rigidBody"]["applyGravity"].as<bool>(
            rigidBody.dynamicDesc.applyGravity);

    mEntityDatabase.set(entity, rigidBody);
  }

  static const std::unordered_map<String, PhysicsGeometryType> ValidShapes{
      {"box", PhysicsGeometryType::Box},
      {"sphere", PhysicsGeometryType::Sphere},
      {"capsule", PhysicsGeometryType::Capsule},
      {"plane", PhysicsGeometryType::Plane}};

  if (node["collidable"] && node["collidable"].IsMap() &&
      ValidShapes.find(node["collidable"]["shape"].as<String>("unknown")) !=
          ValidShapes.end()) {
    Collidable collidable{};
    auto shape = ValidShapes.at(node["collidable"]["shape"].as<String>());
    collidable.geometryDesc.type = shape;

    if (shape == PhysicsGeometryType::Box) {
      liquid::PhysicsGeometryBox box{};
      box.halfExtents =
          node["collidable"]["halfExtents"].as<glm::vec3>(box.halfExtents);

      collidable.geometryDesc.params = box;
    } else if (shape == PhysicsGeometryType::Sphere) {
      liquid::PhysicsGeometrySphere sphere{};
      sphere.radius = node["collidable"]["radius"].as<float>(sphere.radius);

      collidable.geometryDesc.params = sphere;
    } else if (shape == PhysicsGeometryType::Capsule) {
      liquid::PhysicsGeometryCapsule capsule{};
      capsule.radius = node["collidable"]["radius"].as<float>(capsule.radius);
      capsule.halfHeight =
          node["collidable"]["halfHeight"].as<float>(capsule.halfHeight);

      collidable.geometryDesc.params = capsule;
    } else if (shape == PhysicsGeometryType::Plane) {
      collidable.geometryDesc.params = PhysicsGeometryPlane{};
    }

    collidable.materialDesc.dynamicFriction =
        node["collidable"]["dynamicFriction"].as<float>(
            collidable.materialDesc.dynamicFriction);
    collidable.materialDesc.restitution =
        node["collidable"]["restitution"].as<float>(
            collidable.materialDesc.restitution);
    collidable.materialDesc.staticFriction =
        node["collidable"]["staticFriction"].as<float>(
            collidable.materialDesc.staticFriction);

    mEntityDatabase.set(entity, collidable);
  }

  if (node["mesh"]) {
    auto uuid = node["mesh"].as<String>("");
    auto handle = mAssetRegistry.getMeshes().findHandleByUuid(uuid);

    if (handle != MeshAssetHandle::Null) {
      auto type = mAssetRegistry.getMeshes().getAsset(handle).type;

      if (type == AssetType::Mesh) {
        mEntityDatabase.set<Mesh>(entity, {handle});
      } else if (type == AssetType::SkinnedMesh) {
        mEntityDatabase.set<SkinnedMesh>(entity, {handle});
      }
    }
  }

  if (node["meshRenderer"] && node["meshRenderer"].IsMap()) {
    MeshRenderer renderer{};
    auto materials = node["meshRenderer"]["materials"];

    if (materials.IsSequence()) {
      for (auto material : materials) {
        auto uuid = material.as<String>("");
        auto handle = mAssetRegistry.getMaterials().findHandleByUuid(uuid);
        if (handle == MaterialAssetHandle::Null) {
          continue;
        }

        renderer.materials.push_back(handle);
      }
    }

    mEntityDatabase.set(entity, renderer);
  }

  if (node["skinnedMeshRenderer"] && node["skinnedMeshRenderer"].IsMap()) {
    SkinnedMeshRenderer renderer{};
    auto materials = node["skinnedMeshRenderer"]["materials"];

    if (materials.IsSequence()) {
      for (auto material : materials) {
        auto uuid = material.as<String>("");
        auto handle = mAssetRegistry.getMaterials().findHandleByUuid(uuid);
        if (handle == MaterialAssetHandle::Null) {
          continue;
        }

        renderer.materials.push_back(handle);
      }
    }

    mEntityDatabase.set(entity, renderer);
  }

  if (node["skeleton"]) {
    auto uuid = node["skeleton"].as<String>("");
    auto handle = mAssetRegistry.getSkeletons().findHandleByUuid(uuid);

    if (handle != SkeletonAssetHandle::Null) {
      const auto &skeleton =
          mAssetRegistry.getSkeletons().getAsset(handle).data;

      liquid::Skeleton skeletonComponent{};
      skeletonComponent.jointLocalPositions = skeleton.jointLocalPositions;
      skeletonComponent.jointLocalRotations = skeleton.jointLocalRotations;
      skeletonComponent.jointLocalScales = skeleton.jointLocalScales;
      skeletonComponent.jointParents = skeleton.jointParents;
      skeletonComponent.jointInverseBindMatrices =
          skeleton.jointInverseBindMatrices;
      skeletonComponent.jointNames = skeleton.jointNames;
      skeletonComponent.assetHandle = handle;
      skeletonComponent.numJoints =
          static_cast<uint32_t>(skeleton.jointLocalPositions.size());
      skeletonComponent.jointFinalTransforms.resize(skeletonComponent.numJoints,
                                                    glm::mat4{1.0f});
      skeletonComponent.jointWorldTransforms.resize(skeletonComponent.numJoints,
                                                    glm::mat4{1.0f});

      mEntityDatabase.set(entity, skeletonComponent);
    }
  }

  if (node["animator"] && node["animator"].IsMap() &&
      node["animator"]["asset"]) {
    auto assetPath = node["animator"]["asset"].as<String>("");
    auto handle = mAssetRegistry.getAnimators().findHandleByUuid(assetPath);

    if (handle != AnimatorAssetHandle::Null) {
      const auto &asset = mAssetRegistry.getAnimators().getAsset(handle);
      Animator animator;
      animator.asset = handle;
      mEntityDatabase.set(entity, animator);
    }
  }

  if (node["light"] && node["light"].IsMap()) {
    auto light = node["light"];
    auto type =
        light["type"].as<uint32_t>(std::numeric_limits<uint32_t>::max());

    if (type == 0) {
      DirectionalLight component{};
      component.intensity = light["intensity"].as<float>(component.intensity);
      component.color = light["color"].as<glm::vec4>(component.color);

      mEntityDatabase.set(entity, component);

      if (light["shadow"] && light["shadow"].IsMap()) {
        CascadedShadowMap shadowComponent{};
        shadowComponent.softShadows = light["shadow"]["softShadows"].as<bool>(
            shadowComponent.softShadows);
        shadowComponent.splitLambda = light["shadow"]["splitLambda"].as<float>(
            shadowComponent.splitLambda);
        shadowComponent.numCascades =
            light["shadow"]["numCascades"].as<uint32_t>(
                shadowComponent.numCascades);

        shadowComponent.numCascades = glm::clamp(
            shadowComponent.numCascades, 1u, shadowComponent.MaxCascades);

        shadowComponent.splitLambda =
            glm::clamp(shadowComponent.splitLambda, 0.0f, 1.0f);

        mEntityDatabase.set(entity, shadowComponent);
      }
    } else if (type == 1) {
      PointLight component{};
      component.intensity = light["intensity"].as<float>(component.intensity);
      component.color = light["color"].as<glm::vec4>(component.color);
      component.range = light["range"].as<glm::float32>(component.range);

      mEntityDatabase.set(entity, component);
    }
  }

  if (node["camera"] && node["camera"].IsMap()) {
    PerspectiveLens lens{};
    float near = node["camera"]["near"].as<float>(lens.near);
    if (near >= 0.0f) {
      lens.near = near;
    }

    float far = node["camera"]["far"].as<float>(lens.far);
    if (far >= 0.0f) {
      lens.far = far;
    }

    glm::vec2 sensorSize =
        node["camera"]["sensorSize"].as<glm::vec2>(lens.sensorSize);

    if (sensorSize.x >= 0.0f && sensorSize.y >= 0.0f) {
      lens.sensorSize = sensorSize;
    }

    float focalLength =
        node["camera"]["focalLength"].as<float>(lens.focalLength);
    if (focalLength >= 0.0f) {
      lens.focalLength = focalLength;
    }

    float aperture = node["camera"]["aperture"].as<float>(lens.aperture);
    if (aperture >= 0.0f) {
      lens.aperture = aperture;
    }

    float shutterSpeed =
        node["camera"]["shutterSpeed"].as<float>(lens.shutterSpeed);
    if (shutterSpeed >= 0.0f) {
      lens.shutterSpeed = shutterSpeed;
    }

    lens.sensitivity =
        node["camera"]["sensitivity"].as<uint32_t>(lens.sensitivity);

    bool autoRatio = true;
    if (node["camera"]["aspectRatio"] &&
        node["camera"]["aspectRatio"].IsScalar()) {
      auto res = node["camera"]["aspectRatio"].as<String>("");
      if (res.empty()) {
        res = "auto";
      }
      autoRatio = res == "auto";
    }

    if (autoRatio) {
      mEntityDatabase.set<AutoAspectRatio>(entity, {});
    } else {
      float aspectRatio =
          node["camera"]["aspectRatio"].as<float>(lens.aspectRatio);
      if (aspectRatio >= 0.0f) {
        lens.aspectRatio = aspectRatio;
      }
    }

    mEntityDatabase.set<Camera>(entity, {});
    mEntityDatabase.set(entity, lens);
  }

  if (node["audio"] && node["audio"].IsMap()) {
    auto uuid = node["audio"]["source"].as<String>("");

    auto handle = mAssetRegistry.getAudios().findHandleByUuid(uuid);

    if (handle != AudioAssetHandle::Null) {
      mEntityDatabase.set<AudioSource>(entity, {handle});
    }
  }

  if (node["script"]) {
    Script script{};
    String uuid;
    if (node["script"].IsScalar()) {
      uuid = node["script"].as<String>("");
    } else if (node["script"].IsMap()) {
      uuid = node["script"]["asset"].as<String>("");

      if (node["script"]["variables"] && node["script"]["variables"].IsMap()) {
        for (const auto &var : node["script"]["variables"]) {
          if (!var.second.IsMap()) {
            continue;
          }
          auto name = var.first.as<String>("");
          auto type = var.second["type"].as<String>("");
          auto value = var.second["value"].as<String>("");

          if (type == "string") {
            script.variables.insert_or_assign(name, value);
          } else if (type == "prefab") {
            auto handle = mAssetRegistry.getPrefabs().findHandleByUuid(value);
            if (handle != PrefabAssetHandle::Null) {
              script.variables.insert_or_assign(name, handle);
            }
          }
        }
      }
    }

    script.handle = mAssetRegistry.getLuaScripts().findHandleByUuid(uuid);

    if (script.handle != LuaScriptAssetHandle::Null) {
      mEntityDatabase.set(entity, script);
    }
  }

  if (node["text"] && node["text"].IsMap()) {
    auto uuid = node["text"]["font"].as<String>("");
    auto handle = mAssetRegistry.getFonts().findHandleByUuid(uuid);

    Text textComponent{};
    textComponent.font = handle;

    if (handle != FontAssetHandle::Null) {
      if (node["text"]["content"] && node["text"]["content"].IsScalar()) {
        textComponent.text =
            node["text"]["content"].as<String>(textComponent.text);
      }

      textComponent.lineHeight =
          node["text"]["lineHeight"].as<float>(textComponent.lineHeight);

      mEntityDatabase.set(entity, textComponent);
    }
  }

  return Result<bool>::Ok(true);
}

Result<Entity> SceneLoader::loadStartingCamera(const YAML::Node &node,
                                               EntityIdCache &entityIdCache) {
  Entity entity = Entity::Null;
  if (node && node.IsScalar()) {
    auto entityId = node.as<uint64_t>(0);

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

} // namespace liquid::detail
