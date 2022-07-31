#include "liquid/core/Base.h"
#include "SceneView.h"

#include "liquid/imgui/ImguiUtils.h"

namespace liquidator {

bool SceneView::begin(liquid::rhi::TextureHandle texture) {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  if (ImGui::Begin("Scene", nullptr)) {
    liquid::imgui::image(texture, ImGui::GetContentRegionAvail());

    return true;
  }

  return false;
}

void SceneView::end() {
  ImGui::End();
  ImGui::PopStyleVar();
}

} // namespace liquidator
