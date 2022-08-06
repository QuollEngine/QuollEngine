#include "liquid/core/Base.h"
#include "Layout.h"

#include "liquid/imgui/Imgui.h"

#include "Toolbar.h"

namespace liquidator {

void Layout::setup() {
  const float WINDOW_AND_STATUS_BAR_HEIGHT = ImGui::GetFrameHeight() * 2.0f;

  const auto &viewport = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(
      ImVec2(viewport->Pos.x,
             viewport->Pos.y + ImGui::GetFrameHeight() + Toolbar::Height));
  ImGui::SetNextWindowSize(ImVec2(
      // Extract status bar from viewport size
      viewport->Size.x,
      viewport->Size.y - WINDOW_AND_STATUS_BAR_HEIGHT - Toolbar::Height));
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("LiquidatorDockspaceMain", nullptr, flags);
  ImGui::PopStyleVar();
  ImGui::PopStyleVar(2);

  auto dockspaceId = ImGui::GetID("LiquidatorDockspace");
  ImGui::DockSpace(dockspaceId, ImVec2{0.0f, 0.0f},
                   ImGuiDockNodeFlags_PassthruCentralNode);

  if (mFirstTime) {
    ImGui::DockBuilderRemoveNode(dockspaceId);
    ImGui::DockBuilderAddNode(dockspaceId,
                              ImGuiDockNodeFlags_DockSpace |
                                  ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);

    static constexpr float Ratio51 = 0.2f;
    static constexpr float Ratio14 = 0.25;

    ImGuiID topAreaId = -1;
    auto browserId = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Down,
                                                 Ratio14, nullptr, &topAreaId);

    ImGuiID topRightAreaId = -1;
    auto hierarchyId = ImGui::DockBuilderSplitNode(
        topAreaId, ImGuiDir_Left, Ratio51, nullptr, &topRightAreaId);

    ImGuiID viewId = -1;
    auto inspectorId = ImGui::DockBuilderSplitNode(
        topRightAreaId, ImGuiDir_Right, Ratio14, nullptr, &viewId);

    ImGui::DockBuilderDockWindow("Hierarchy", hierarchyId);
    ImGui::DockBuilderDockWindow("Entity", inspectorId);
    ImGui::DockBuilderDockWindow("Environment", inspectorId);
    ImGui::DockBuilderDockWindow("Scene", viewId);
    ImGui::DockBuilderDockWindow("Asset Browser", browserId);

    ImGui::DockBuilderFinish(dockspaceId);
    mFirstTime = false;
  } else {
    ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);
  }

  ImGui::End();
}

} // namespace liquidator
