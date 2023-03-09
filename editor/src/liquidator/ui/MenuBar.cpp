#include "liquid/core/Base.h"
#include "MenuBar.h"
#include "ConfirmationDialog.h"

#include "liquid/imgui/Imgui.h"
#include "Widgets.h"

namespace liquid::editor {

void MenuBar::render(WorkspaceState &state, EditorManager &editorManager,
                     EntityManager &entityManager) {
  if (ImGui::BeginMenu("Project")) {
    if (ImGui::MenuItem("Export as game", nullptr)) {
      editorManager.startGameExport();
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Objects")) {
    if (ImGui::MenuItem("Create empty entity", nullptr)) {
      entityManager.createEmptyEntity(state.camera, Entity::Null);
    }

    ImGui::EndMenu();
  }
}

} // namespace liquid::editor
