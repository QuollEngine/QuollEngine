#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"

#include "liquidator/core/CameraLookAt.h"
#include "EntityManager.h"

namespace liquid::editor {

EntityManager::EntityManager(AssetManager &assetManager, SceneIO &sceneIO,
                             WorkspaceState &state,
                             const std::filesystem::path &scenePath)
    : mScenePath(scenePath), mAssetManager(assetManager), mState(state),
      mSceneIO(sceneIO) {
  mState.scene.entityDatabase.reg<CameraLookAt>();
}

void EntityManager::save(Entity entity) {
  if (mState.mode == WorkspaceMode::Simulation)
    return;

  mSceneIO.saveEntity(entity, mScenePath / "main.lqscene");
}

Entity EntityManager::createEmptyEntity(Entity parent,
                                        const LocalTransform &transform,
                                        const String &name) {

  auto &entityDatabase = getActiveEntityDatabase();

  auto entity = entityDatabase.create();
  entityDatabase.set(entity, transform);
  entityDatabase.set<WorldTransform>(entity, {});

  if (entityDatabase.exists(parent)) {
    entityDatabase.set<Parent>(entity, {parent});

    if (!entityDatabase.has<Children>(parent)) {
      entityDatabase.set<Children>(parent, {});
    }

    entityDatabase.get<Children>(parent).children.push_back(entity);
  }

  setName(entity, name);

  return entity;
}

bool EntityManager::loadScene() {
  mSceneIO.loadScene(mScenePath / "main.lqscene");

  return true;
}

void EntityManager::setName(Entity entity, const String &name) {
  if (name.empty()) {
    return;
  }

  getActiveEntityDatabase().set<Name>(entity, {name});
}

void EntityManager::updateLocalTransformUsingWorld(
    Entity entity, const glm::mat4 &worldTransform) {
  auto &entityDatabase = getActiveEntityDatabase();

  glm::vec3 worldPosition;
  glm::quat worldRotation;
  glm::vec3 worldScale;

  glm::vec3 noopSkew;
  glm::vec4 noopPerspective;

  glm::decompose(worldTransform, worldScale, worldRotation, worldPosition,
                 noopSkew, noopPerspective);

  auto &transform = entityDatabase.get<LocalTransform>(entity);

  if (entityDatabase.has<Parent>(entity)) {
    const auto parent = entityDatabase.get<Parent>(entity).parent;
    if (entityDatabase.exists(parent) &&
        entityDatabase.has<WorldTransform>(parent)) {
      const auto &parentWorld =
          entityDatabase.get<WorldTransform>(parent).worldTransform;

      glm::vec3 parentPosition;
      glm::quat parentRotation;
      glm::vec3 parentScale;

      glm::decompose(parentWorld, parentScale, parentRotation, parentPosition,
                     noopSkew, noopPerspective);

      transform.localPosition = worldPosition - parentPosition;
      transform.localScale =
          glm::vec3(worldScale.x / parentScale.x, worldScale.y / parentScale.y,
                    worldScale.z / parentScale.z);
      transform.localRotation =
          glm::toQuat(glm::inverse(parentWorld) * glm::toMat4(worldRotation));
    }
  } else {
    transform.localPosition = worldPosition;
    transform.localScale = worldScale;
    transform.localRotation = worldRotation;
  }

  save(entity);
}

} // namespace liquid::editor
