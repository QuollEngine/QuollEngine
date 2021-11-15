#include "MenuBar.h"
#include <imgui.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_access.hpp>

namespace liquidator {

MenuBar::MenuBar(const liquid::TinyGLTFLoader &loader_) : loader(loader_) {}

void MenuBar::render(liquid::Scene *scene) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Import GLTF...", nullptr)) {
        handleGLTFImport(fileDialog.getFilePathFromDialog({"gltf"}), scene);
      }

      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void MenuBar::handleGLTFImport(const liquid::String &filePath,
                               liquid::Scene *scene) {
  constexpr glm::vec3 distanceFromEye = {0.0f, 0.0f, -10.0f};
  auto *sceneNode = loader.loadFromFile(filePath);

  const auto &invViewMatrix =
      glm::inverse(scene->getActiveCamera()->getViewMatrix());

  sceneNode->setTransform(invViewMatrix * glm::translate(distanceFromEye));

  scene->getRootNode()->addChild(sceneNode);
}

} // namespace liquidator
