#include "liquid/core/Base.h"
#include "SceneManager.h"

namespace liquidator {

SceneManager::SceneManager(liquid::EntityContext &entityContext,
                           EditorCamera &editorCamera, EditorGrid &editorGrid)
    : mEntityContext(entityContext), mEditorCamera(editorCamera),
      mEditorGrid(editorGrid) {}

void SceneManager::requestEmptyScene() { mNewSceneRequested = true; }

void SceneManager::createNewScene() {
  if (!mNewSceneRequested)
    return;

  if (mActiveScene) {
    delete mActiveScene;
    mEntityContext.destroy();
  }

  mEditorCamera.reset();
  mActiveScene = new liquid::Scene(mEntityContext);
  mActiveScene->setActiveCamera(mEditorCamera.getCamera());

  auto light1 = mEntityContext.createEntity();
  mEntityContext.setComponent<liquid::NameComponent>(light1, {"Light"});
  mEntityContext.setComponent<liquid::LightComponent>(
      light1, {std::make_shared<liquid::Light>(
                  liquid::Light::DIRECTIONAL, glm::vec3{0.0f, 1.0f, 1.0f},
                  glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, 1.0f)});

  mActiveScene->getRootNode()->addChild(light1);

  mNewSceneRequested = false;
}

} // namespace liquidator
