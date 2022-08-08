#include "liquid/core/Base.h"
#include "SceneView.h"

#include "liquid/imgui/ImguiUtils.h"

namespace liquidator {

SceneView::SceneView(liquid::rhi::TextureHandle texture) {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  mExpanded = ImGui::Begin("Scene", nullptr);
  if (mExpanded) {
    liquid::imgui::image(texture, ImGui::GetContentRegionAvail());
  }
}

SceneView::~SceneView() {
  ImGui::End();
  ImGui::PopStyleVar();
}

} // namespace liquidator
