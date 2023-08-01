#include "liquid/core/Base.h"
#include "SceneView.h"

#include "liquid/imgui/ImguiUtils.h"

namespace liquid::editor {

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

} // namespace liquid::editor
