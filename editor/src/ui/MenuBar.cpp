#include "liquid/core/Base.h"
#include "MenuBar.h"
#include "ConfirmationDialog.h"

#include <imgui.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_access.hpp>

namespace liquidator {

MenuBar::MenuBar(const liquid::GLTFLoader &loader) : mLoader(loader) {}

void MenuBar::render(SceneManager &sceneManager) {
  ConfirmationDialog confirmCreateNewScene(
      "Create New Scene", "Are you sure you want to create a new scene?",
      [this](SceneManager &sceneManager) { handleNewScene(sceneManager); });

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("New", nullptr)) {
        confirmCreateNewScene.show();
      }

      if (ImGui::MenuItem("Import GLTF...", nullptr)) {
        handleGLTFImport(mFileDialog.getFilePathFromDialog({"gltf"}),
                         sceneManager.getActiveScene());
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  confirmCreateNewScene.render(sceneManager);
}

void MenuBar::handleGLTFImport(const liquid::String &filePath,
                               liquid::Scene *scene) {
  constexpr glm::vec3 distanceFromEye = {0.0f, 0.0f, -10.0f};
  auto &&result = mLoader.loadFromFile(filePath);

  if (result.hasError())
    return;

  const auto &invViewMatrix =
      glm::inverse(scene->getActiveCamera()->getViewMatrix());

  const auto &orientation = invViewMatrix * glm::translate(distanceFromEye);

  auto &transform = result.getResult()->getTransform();
  transform.localPosition = orientation[3];

  scene->getRootNode()->addChild(result.getResult());
}

void MenuBar::handleNewScene(SceneManager &sceneManager) {
  sceneManager.requestEmptyScene();
}

} // namespace liquidator
