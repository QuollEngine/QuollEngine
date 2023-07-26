#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "liquidator/ui/Toolbar.h"

#include "WorkspaceLayoutRenderer.h"

namespace liquid::editor {

bool WorkspaceLayoutRenderer::begin() {
  const float WINDOW_AND_STATUS_BAR_HEIGHT = ImGui::GetFrameHeight() * 2.0f;

  const auto &viewport = ImGui::GetMainViewport();

  if (viewport->Size.x <= 0.0f || viewport->Size.y <= 0.0f)
    return false;

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

  return true;
}

void WorkspaceLayoutRenderer::end() { ImGui::End(); }

void WorkspaceLayoutRenderer::reset() {
  const auto &viewport = ImGui::GetMainViewport();
  auto dockspaceId = ImGui::GetID("LiquidatorDockspace");

  ImGui::DockBuilderRemoveNode(dockspaceId);
  ImGui::DockBuilderAddNode(dockspaceId,
                            ImGuiDockNodeFlags_DockSpace |
                                ImGuiDockNodeFlags_PassthruCentralNode);
  ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);

  static constexpr float RatioSide = 1.0f / 8.0f;
  static constexpr float RatioBottom = 1.0f / 4.0f;

  ImGuiID topAreaId = -1;
  auto browserId = ImGui::DockBuilderSplitNode(
      dockspaceId, ImGuiDir_Down, RatioBottom, nullptr, &topAreaId);

  ImGuiID topRightAreaId = -1;
  auto hierarchyId = ImGui::DockBuilderSplitNode(
      topAreaId, ImGuiDir_Left, RatioSide, nullptr, &topRightAreaId);

  ImGuiID viewId = -1;
  auto inspectorId = ImGui::DockBuilderSplitNode(topRightAreaId, ImGuiDir_Right,
                                                 RatioSide, nullptr, &viewId);

  ImGui::DockBuilderDockWindow("Hierarchy", hierarchyId);
  ImGui::DockBuilderDockWindow("Entity", inspectorId);
  ImGui::DockBuilderDockWindow("Environment", inspectorId);
  ImGui::DockBuilderDockWindow("Scene", viewId);
  ImGui::DockBuilderDockWindow("Asset Browser", browserId);
  ImGui::DockBuilderDockWindow("Logs", browserId);

  ImGui::DockBuilderFinish(dockspaceId);
}

void WorkspaceLayoutRenderer::resize() {
  const auto &viewport = ImGui::GetMainViewport();
  auto dockspaceId = ImGui::GetID("LiquidatorDockspace");
  ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);
}

} // namespace liquid::editor
