#include "MenuBar.h"
#include <imgui.h>

namespace liquidator {

void MenuBar::render() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("New Scene", nullptr)) {
      }

      if (ImGui::MenuItem("Import GLTF...", nullptr)) {
      }

      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

} // namespace liquidator
