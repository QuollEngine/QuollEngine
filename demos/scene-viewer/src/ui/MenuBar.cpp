#include "MenuBar.h"
#include <imgui.h>

void MenuBar::render() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open Scene...", nullptr) && sceneOpenHandler) {
        sceneOpenHandler();
      }

      if (ImGui::MenuItem("Open Environment...", nullptr) &&
          environmentOpenHandler) {
        environmentOpenHandler();
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}
