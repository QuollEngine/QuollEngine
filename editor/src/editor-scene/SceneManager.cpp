#include "SceneManager.h"

namespace liquidator {

SceneManager::SceneManager(liquid::EntityContext &context_,
                           EditorCamera &editorCamera_)
    : context(context_), editorCamera(editorCamera_) {}

void SceneManager::requestEmptyScene() { newSceneRequested = true; }

void SceneManager::createNewScene() {
  if (!newSceneRequested)
    return;

  if (activeScene) {
    delete activeScene;
    context.destroy();
  }

  editorCamera.reset();
  activeScene = new liquid::Scene(context);
  activeScene->setActiveCamera(editorCamera.getCamera().get());

  auto light1 = context.createEntity();
  context.setComponent<liquid::NameComponent>(light1, {"Light"});
  context.setComponent<liquid::LightComponent>(
      light1, {std::make_shared<liquid::Light>(
                  liquid::Light::DIRECTIONAL, glm::vec3{0.0f, 1.0f, 1.0f},
                  glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, 1.0f)});

  activeScene->getRootNode()->addChild(light1);

  newSceneRequested = false;
}

} // namespace liquidator
