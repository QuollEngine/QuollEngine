#include "MenuBar.h"
#include <imgui.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_access.hpp>

namespace liquidator {

MenuBar::MenuBar(const liquid::TinyGLTFLoader &loader_) : loader(loader_) {}

void MenuBar::render(SceneManager &sceneManager) {
  bool createNewSceneConfirmationDialogOpen = false;
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("New", nullptr)) {
        createNewSceneConfirmationDialogOpen = true;
      }

      if (ImGui::MenuItem("Import GLTF...", nullptr)) {
        handleGLTFImport(fileDialog.getFilePathFromDialog({"gltf"}),
                         sceneManager.getActiveScene());
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  if (createNewSceneConfirmationDialogOpen) {
    ImGui::OpenPopup("Create New Scene");
    createNewSceneConfirmationDialogOpen = false;
  }

  bool open = true;
  if (ImGui::BeginPopupModal("Create New Scene", &open)) {
    ImGui::Text("Are you sure you want to create a new scene?");
    if (ImGui::Button("Create")) {
      handleNewScene(sceneManager);
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
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

void MenuBar::handleNewScene(SceneManager &sceneManager) {
  sceneManager.requestEmptyScene();
}

} // namespace liquidator
