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
      [](SceneManager &sceneManager) { sceneManager.requestEmptyScene(); });

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("New", nullptr)) {
        confirmCreateNewScene.show();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Objects")) {
      if (ImGui::MenuItem("Create empty entity", nullptr)) {
        sceneManager.createEntityAtView();
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  confirmCreateNewScene.render(sceneManager);
}

} // namespace liquidator
