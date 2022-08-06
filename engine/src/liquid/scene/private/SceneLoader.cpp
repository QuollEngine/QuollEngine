#include "liquid/core/Base.h"
#include "SceneLoader.h"

namespace liquid::detail {

SceneLoader::SceneLoader(AssetRegistry &assetRegistry,
                         EntityDatabase &entityDatabase)
    : mAssetRegistry(assetRegistry), mEntityDatabase(entityDatabase) {}

Result<bool> SceneLoader::loadComponents(const YAML::Node &node, Entity entity,
                                         EntityIdCache &entityIdCache) {
  if (node["components"]["name"] && node["components"]["name"].IsScalar()) {
    auto name = node["components"]["name"].as<liquid::String>();
    mEntityDatabase.setComponent<NameComponent>(entity, {name});
  } else {
    auto name = "Untitled " + node["id"].as<String>();
    mEntityDatabase.setComponent<NameComponent>(entity, {name});
  }

  LocalTransformComponent transform{};
  if (node["components"]["transform"] &&
      node["components"]["transform"].IsMap()) {
    transform.localPosition =
        node["components"]["transform"]["position"].as<glm::vec3>(
            transform.localPosition);

    transform.localRotation =
        node["components"]["transform"]["rotation"].as<glm::quat>(
            transform.localRotation);

    transform.localScale =
        node["components"]["transform"]["scale"].as<glm::vec3>(
            transform.localScale);

    if (node["components"]["transform"]["parent"]) {
      auto parentId = node["components"]["transform"]["parent"].as<uint64_t>(0);

      auto it = entityIdCache.find(parentId);
      Entity parentEntity = it != entityIdCache.end() ? it->second : EntityNull;

      if (parentEntity != EntityNull) {
        mEntityDatabase.setComponent<ParentComponent>(entity, {parentEntity});

        if (mEntityDatabase.hasComponent<ChildrenComponent>(parentEntity)) {
          mEntityDatabase.getComponent<ChildrenComponent>(parentEntity)
              .children.push_back(entity);
        } else {
          mEntityDatabase.setComponent<ChildrenComponent>(parentEntity,
                                                          {{entity}});
        }
      }
    }
  }

  mEntityDatabase.setComponent(entity, transform);
  mEntityDatabase.setComponent<WorldTransformComponent>(entity, {});

  if (node["components"]["mesh"]) {
    auto path = Path(node["components"]["mesh"].as<String>(""));
    auto handle = mAssetRegistry.getMeshes().findHandleByRelativePath(path);

    if (handle != MeshAssetHandle::Invalid) {
      mEntityDatabase.setComponent<MeshComponent>(entity, {handle});
    }
  }

  if (node["components"]["skinnedMesh"]) {
    auto path = Path(node["components"]["skinnedMesh"].as<String>(""));
    auto handle =
        mAssetRegistry.getSkinnedMeshes().findHandleByRelativePath(path);

    if (handle != SkinnedMeshAssetHandle::Invalid) {
      mEntityDatabase.setComponent<SkinnedMeshComponent>(entity, {handle});
    }
  }

  if (node["components"]["skeleton"]) {
    auto path = Path(node["components"]["skeleton"].as<String>(""));
    auto handle = mAssetRegistry.getSkeletons().findHandleByRelativePath(path);

    if (handle != SkeletonAssetHandle::Invalid) {
      const auto &skeleton =
          mAssetRegistry.getSkeletons().getAsset(handle).data;

      liquid::SkeletonComponent skeletonComponent{};
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

      mEntityDatabase.setComponent(entity, skeletonComponent);
    }
  }

  if (node["components"]["light"] && node["components"]["light"].IsMap()) {
    auto type = node["components"]["light"]["type"].as<uint32_t>(
        std::numeric_limits<uint32_t>::max());

    if (type == 0) {
      DirectionalLightComponent component{};
      component.intensity = node["components"]["light"]["intensity"].as<float>(
          component.intensity);
      component.color =
          node["components"]["light"]["color"].as<glm::vec4>(component.color);

      mEntityDatabase.setComponent(entity, component);
    }
  }

  if (node["components"]["camera"] && node["components"]["camera"].IsMap()) {
    PerspectiveLensComponent lens{};
    lens.fovY = node["components"]["camera"]["fov"].as<float>(lens.fovY);
    lens.near = node["components"]["camera"]["near"].as<float>(lens.near);
    lens.far = node["components"]["camera"]["far"].as<float>(lens.far);

    bool autoRatio = true;
    if (node["components"]["camera"]["aspectRatio"] &&
        node["components"]["camera"]["aspectRatio"].IsScalar()) {
      auto res = node["components"]["camera"]["aspectRatio"].as<String>("");
      if (res.empty()) {
        res = "auto";
      }
      autoRatio = res == "auto";
    }

    if (autoRatio) {
      mEntityDatabase.setComponent<AutoAspectRatioComponent>(entity, {});
    } else {
      lens.aspectRatio = node["components"]["camera"]["aspectRatio"].as<float>(
          lens.aspectRatio);
    }

    mEntityDatabase.setComponent<CameraComponent>(entity, {});
    mEntityDatabase.setComponent(entity, lens);
  }

  if (node["components"]["audio"] && node["components"]["audio"].IsMap()) {
    auto path = node["components"]["audio"]["source"].as<String>("");

    auto handle = mAssetRegistry.getAudios().findHandleByRelativePath(path);

    if (handle != AudioAssetHandle::Invalid) {
      mEntityDatabase.setComponent<AudioSourceComponent>(entity, {handle});
    }
  }

  if (node["components"]["script"]) {
    auto path = node["components"]["script"].as<String>("");

    auto handle = mAssetRegistry.getLuaScripts().findHandleByRelativePath(path);

    if (handle != LuaScriptAssetHandle::Invalid) {
      mEntityDatabase.setComponent<ScriptingComponent>(entity, {handle});
    }
  }

  if (node["components"]["text"] && node["components"]["text"].IsMap()) {
    auto path = node["components"]["text"]["font"].as<String>("");
    auto handle = mAssetRegistry.getFonts().findHandleByRelativePath(path);

    TextComponent textComponent{};
    textComponent.font = handle;

    if (handle != FontAssetHandle::Invalid) {
      if (node["components"]["text"]["content"] &&
          node["components"]["text"]["content"].IsScalar()) {
        textComponent.text = node["components"]["text"]["content"].as<String>(
            textComponent.text);
      }

      textComponent.lineHeight =
          node["components"]["text"]["lineHeight"].as<float>(
              textComponent.lineHeight);

      mEntityDatabase.setComponent(entity, textComponent);
    }
  }

  return Result<bool>::Ok(entity);
}

} // namespace liquid::detail
