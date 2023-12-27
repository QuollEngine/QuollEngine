#include "quoll/core/Base.h"
#include "SceneView.h"

#include "quoll/imgui/ImguiUtils.h"
#include "quoll/editor/ui/Theme.h"

namespace quoll::editor {

SceneView::SceneView(rhi::TextureHandle texture) {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  mExpanded = ImGui::Begin(
      "View", nullptr,
      ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus |
          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar);
  ImGui::PopStyleVar();

  if (mExpanded) {
    imgui::image(texture, ImGui::GetContentRegionAvail());
  }
}

SceneView::~SceneView() { ImGui::End(); }

} // namespace quoll::editor
