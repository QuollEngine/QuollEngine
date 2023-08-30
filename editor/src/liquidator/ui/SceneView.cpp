#include "quoll/core/Base.h"
#include "SceneView.h"

#include "quoll/imgui/ImguiUtils.h"

namespace quoll::editor {

SceneView::SceneView(rhi::TextureHandle texture) {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  mExpanded = ImGui::Begin("View", nullptr);
  if (mExpanded) {
    imgui::image(texture, ImGui::GetContentRegionAvail());
  }
}

SceneView::~SceneView() {
  ImGui::End();
  ImGui::PopStyleVar();
}

} // namespace quoll::editor
