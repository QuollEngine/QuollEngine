#include "liquid/core/Base.h"
#include "MenuBar.h"
#include "ConfirmationDialog.h"

#include <imgui.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_access.hpp>

namespace liquidator {

void MenuBar::render(SceneManager &sceneManager) {
  ConfirmationDialog confirmCreateNewScene(
      "Create New Scene", "Are you sure you want to create a new scene?",
      [this](SceneManager &sceneManager) { handleNewScene(sceneManager); });

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("New", nullptr)) {
        confirmCreateNewScene.show();
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  confirmCreateNewScene.render(sceneManager);
}

void MenuBar::handleNewScene(SceneManager &sceneManager) {
  sceneManager.requestEmptyScene();
}

} // namespace liquidator
