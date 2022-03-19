#include "liquid/core/Base.h"
#include "Layout.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace liquidator {

void Layout::setup() {
  const float WINDOW_AND_STATUS_BAR_HEIGHT = ImGui::GetFrameHeight() * 2.0f;
  const auto &viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(
      ImVec2(viewport->Pos.x, viewport->Pos.y + ImGui::GetFrameHeight()));
  ImGui::SetNextWindowSize(ImVec2(
      // Extract status bar from viewport size
      viewport->Size.x, viewport->Size.y - WINDOW_AND_STATUS_BAR_HEIGHT));
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

    constexpr float TWENTY_EIGHTY_SPLIT = 0.2f;
    constexpr float HALF_SPLIT = 0.5f;

    ImGuiID viewId = -1;
    ImGuiID leftSidebarBottomId = -1;
    auto leftSidebarId = ImGui::DockBuilderSplitNode(
        dockspaceId, ImGuiDir_Left, TWENTY_EIGHTY_SPLIT, nullptr, &viewId);
    auto leftSidebarTopId = ImGui::DockBuilderSplitNode(
        leftSidebarId, ImGuiDir_Up, HALF_SPLIT, nullptr, &leftSidebarBottomId);

    ImGui::DockBuilderDockWindow("Scene", leftSidebarTopId);
    ImGui::DockBuilderDockWindow("Properties", leftSidebarBottomId);
    ImGui::DockBuilderDockWindow("View", viewId);

    ImGui::DockBuilderFinish(dockspaceId);
    mFirstTime = false;
  }

  ImGui::End();
}

} // namespace liquidator
