#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"

#include "StatusBar.h"
#include "Theme.h"
#include "StyleStack.h"
#include "MenuBar.h"

namespace quoll::editor {

void StatusBar::render(EditorCamera &editorCamera) {
  const ImGuiViewport *viewport = ImGui::GetMainViewport();

  String state = "";
  switch (editorCamera.getInputState()) {
  case EditorCamera::InputState::Pan:
    state = "Panning";
    break;
  case EditorCamera::InputState::Rotate:
    state = "Rotating";
    break;
  case EditorCamera::InputState::Zoom:
  case EditorCamera::InputState::ZoomWheel:
    state = "Zooming";
    break;
  default:
    state = "";
  }

  ImGui::SetNextWindowPos(
      ImVec2(viewport->Pos.x,
             viewport->Pos.y + viewport->Size.y - ImGui::GetFrameHeight()));
  ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, ImGui::GetFrameHeight()));

  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse |
      ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground |
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

  StyleStack stack;
  stack.pushColor(ImGuiCol_MenuBarBg, Theme::getColor(ThemeColor::Charcoal800));
  stack.pushStyle(ImGuiStyleVar_WindowRounding, 0.0f);

  if (ImGui::Begin("StatusBar", nullptr, flags)) {
    if (auto _ = MenuBar()) {
      ImGui::Text("%s", state.c_str());
    }
    ImGui::End();
  }
}

} // namespace quoll::editor
