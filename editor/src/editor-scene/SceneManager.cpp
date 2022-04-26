#include "liquid/core/Base.h"
#include "SceneManager.h"

namespace liquidator {

SceneManager::SceneManager(liquid::EntityContext &entityContext,
                           EditorCamera &editorCamera, EditorGrid &editorGrid)
    : mEntityContext(entityContext), mEditorCamera(editorCamera),
      mEditorGrid(editorGrid) {}

void SceneManager::requestEmptyScene() { mNewSceneRequested = true; }

liquid::SceneNode *SceneManager::createEntityAtView() {
  const auto &viewMatrix =
      mEntityContext
          .getComponent<liquid::CameraComponent>(mEditorCamera.getCamera())
          .camera->getViewMatrix();

  constexpr glm::vec3 distanceFromEye = {0.0f, 0.0f, -10.0f};
  const auto &invViewMatrix = glm::inverse(viewMatrix);
  const auto &orientation = invViewMatrix * glm::translate(distanceFromEye);

  liquid::TransformComponent transform;
  transform.localPosition = orientation[3];

  auto entity = mEntityContext.createEntity();
  mEntityContext.setComponent<liquid::DebugComponent>(entity, {});

  auto *node = mActiveScene->getRootNode()->addChild(entity, transform);

  return node;
}

void SceneManager::createNewScene() {
  static constexpr glm::vec3 LIGHT_START_POS(0.0f, 5.0f, 0.0f);

  if (!mNewSceneRequested)
    return;

  if (mActiveScene) {
    delete mActiveScene;
    mEntityContext.destroy();
  }

  mEditorCamera.reset();
  mActiveScene = new liquid::Scene(mEntityContext);

  {
    auto light1 = mEntityContext.createEntity();
    mEntityContext.setComponent<liquid::NameComponent>(light1, {"Light"});
    mEntityContext.setComponent<liquid::LightComponent>(
        light1, {std::make_shared<liquid::Light>(
                    liquid::Light::DIRECTIONAL,
                    glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, 1.0f)});
    mEntityContext.setComponent<liquid::DebugComponent>(light1, {});

    liquid::TransformComponent transform{};
    transform.localPosition = LIGHT_START_POS;
    transform.localRotation =
        glm::quat(glm::vec3(0.0f, 0.0f, glm::pi<float>()));

    mActiveScene->getRootNode()->addChild(light1, transform);
  }

  mNewSceneRequested = false;
}

} // namespace liquidator
